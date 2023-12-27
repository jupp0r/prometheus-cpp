#include "handler.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstring>
#include <iterator>
#include <numeric>
#include <string>

#ifdef HAVE_ZLIB
#include <zconf.h>
#include <zlib.h>
#endif

#include "civetweb.h"
#include "metrics_collector.h"
#include "prometheus/counter.h"
#include "prometheus/iovector.h"
#include "prometheus/metric_family.h"
#include "prometheus/summary.h"
#include "prometheus/text_serializer.h"

#if CIVETWEB_VERSION_MAJOR < 1 || \
    (CIVETWEB_VERSION_MAJOR == 1 && CIVETWEB_VERSION_MINOR < 14)
// https://github.com/civetweb/civetweb/issues/954
#error "Civetweb version 1.14 or higher required"
#endif

namespace prometheus {
namespace detail {

MetricsHandler::MetricsHandler(Registry& registry)
    : bytes_transferred_family_(
          BuildCounter()
              .Name("exposer_transferred_bytes_total")
              .Help("Transferred bytes to metrics services")
              .Register(registry)),
      bytes_transferred_(bytes_transferred_family_.Add({})),
      num_scrapes_family_(BuildCounter()
                              .Name("exposer_scrapes_total")
                              .Help("Number of times metrics were scraped")
                              .Register(registry)),
      num_scrapes_(num_scrapes_family_.Add({})),
      request_latencies_family_(
          BuildSummary()
              .Name("exposer_request_latencies")
              .Help("Latencies of serving scrape requests, in microseconds")
              .Register(registry)),
      request_latencies_(request_latencies_family_.Add(
          {}, Summary::Quantiles{{0.5, 0.05}, {0.9, 0.01}, {0.99, 0.001}})) {}

#ifdef HAVE_ZLIB
static bool IsEncodingAccepted(struct mg_connection* conn,
                               const char* encoding) {
  auto accept_encoding = mg_get_header(conn, "Accept-Encoding");
  if (!accept_encoding) {
    return false;
  }
  return std::strstr(accept_encoding, encoding) != nullptr;
}

static IOVector GZipCompress(const IOVector& input) {
  auto zs = z_stream{};
  auto windowSize = 16 + MAX_WBITS;
  auto memoryLevel = 9;

  if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowSize,
                   memoryLevel, Z_DEFAULT_STRATEGY) != Z_OK) {
    return {};
  }

  auto s = input.size();

  int ret;

  IOVector output;

  for (std::size_t i = 0; i < input.data.size(); ++i) {
    bool last = i == input.data.size() - 1U;
    auto chunk = input.data[i];

    zs.next_in =
        const_cast<Bytef*>(reinterpret_cast<const Bytef*>(chunk.data()));
    zs.avail_in = chunk.size();

    do {
      static constexpr std::size_t maximumChunkSize = 1 * 1024 * 1024;
      if (output.data.empty() ||
          output.data.back().size() >= maximumChunkSize) {
        output.data.emplace_back();
        output.data.back().reserve(maximumChunkSize);
      }

      auto&& chunk = output.data.back();

      const auto previouslyUsed = chunk.size();
      const auto remainingChunkSize = maximumChunkSize - previouslyUsed;

      zs.avail_out = remainingChunkSize;
      chunk.resize(chunk.size() + remainingChunkSize);
      zs.next_out = reinterpret_cast<Bytef*>(chunk.data() + previouslyUsed);

      ret = deflate(&zs, last ? Z_FINISH : Z_NO_FLUSH);
      assert(ret != Z_STREAM_ERROR);

      chunk.resize(maximumChunkSize - zs.avail_out);
    } while (zs.avail_out == 0U);
    assert(zs.avail_in == 0);
  }
  assert(ret == Z_STREAM_END);
  assert(zs.total_out == output.size());

  deflateEnd(&zs);

  if (ret != Z_STREAM_END) {
    return {};
  }

  return output;
}
#endif

static std::size_t WriteResponse(struct mg_connection* conn,
                                 const IOVector& body) {
  mg_printf(conn,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain; charset=utf-8\r\n");

#ifdef HAVE_ZLIB
  auto acceptsGzip = IsEncodingAccepted(conn, "gzip");

  if (acceptsGzip) {
    auto compressed = GZipCompress(body);
    if (!compressed.empty()) {
      const std::size_t contentSize = compressed.size();
      mg_printf(conn,
                "Content-Encoding: gzip\r\n"
                "Content-Length: %s\r\n\r\n",
                std::to_string(contentSize).c_str());
      for (auto&& chunk : compressed.data) {
        mg_write(conn, chunk.data(), chunk.size());
      }
      return contentSize;
    }
  }
#endif

  std::size_t contentSize = body.size();

  mg_printf(conn, "Content-Length: %s\r\n\r\n",
            std::to_string(contentSize).c_str());
  for (auto&& chunk : body.data) {
    mg_write(conn, chunk.data(), chunk.size());
  }
  return contentSize;
}

void MetricsHandler::RegisterCollectable(
    const std::weak_ptr<Collectable>& collectable) {
  std::lock_guard<std::mutex> lock{collectables_mutex_};
  CleanupStalePointers(collectables_);
  collectables_.push_back(collectable);
}

void MetricsHandler::RemoveCollectable(
    const std::weak_ptr<Collectable>& collectable) {
  std::lock_guard<std::mutex> lock{collectables_mutex_};

  auto locked = collectable.lock();
  auto same_pointer = [&locked](const std::weak_ptr<Collectable>& candidate) {
    return locked == candidate.lock();
  };

  collectables_.erase(std::remove_if(std::begin(collectables_),
                                     std::end(collectables_), same_pointer),
                      std::end(collectables_));
}

bool MetricsHandler::handleGet(CivetServer*, struct mg_connection* conn) {
  auto start_time_of_request = std::chrono::steady_clock::now();

  IOVector ioVector;
  const auto serializer = TextSerializer{ioVector};

  {
    std::lock_guard<std::mutex> lock{collectables_mutex_};
    CollectMetrics(serializer, collectables_);
  }

  auto bodySize = WriteResponse(conn, ioVector);

  auto stop_time_of_request = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      stop_time_of_request - start_time_of_request);
  request_latencies_.Observe(duration.count());

  bytes_transferred_.Increment(bodySize);
  num_scrapes_.Increment();
  return true;
}

void MetricsHandler::CleanupStalePointers(
    std::vector<std::weak_ptr<Collectable>>& collectables) {
  collectables.erase(
      std::remove_if(std::begin(collectables), std::end(collectables),
                     [](const std::weak_ptr<Collectable>& candidate) {
                       return candidate.expired();
                     }),
      std::end(collectables));
}
}  // namespace detail
}  // namespace prometheus
