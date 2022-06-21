#include "curl_wrapper.h"

#include <stdexcept>

namespace prometheus {
namespace detail {

static const char CONTENT_TYPE[] =
    "Content-Type: text/plain; version=0.0.4; charset=utf-8";

CurlWrapper::CurlWrapper(const std::string& username,
                         const std::string& password) {
  /* In windows, this will init the winsock stuff */
  auto error = curl_global_init(CURL_GLOBAL_ALL);
  if (error) {
    throw std::runtime_error("Cannot initialize global curl!");
  }

  curl_ = curl_easy_init();
  if (!curl_) {
    curl_global_cleanup();
    throw std::runtime_error("Cannot initialize easy curl!");
  }

  optHttpHeader_ = nullptr;

  if (!username.empty()) {
    auth_ = username + ":" + password;
  }
}

CurlWrapper::~CurlWrapper() {
  curl_slist_free_all(optHttpHeader_);
  curl_easy_cleanup(curl_);
  curl_global_cleanup();
}

int CurlWrapper::performHttpRequest(HttpMethod method, const std::string& uri,
                                    const std::string& body) {
  std::lock_guard<std::mutex> l(mutex_);

  curl_easy_reset(curl_);
  curl_easy_setopt(curl_, CURLOPT_URL, uri.c_str());

  curl_slist* header_chunk = nullptr;
  header_chunk = curl_slist_append(header_chunk, CONTENT_TYPE); 
  if (nullptr == header_chunk)
  {
    return -1;
  }

  curl_slist* optHeader_tmp = optHttpHeader_;
  while (optHeader_tmp)
  {
    curl_slist* header_tmp = curl_slist_append(header_chunk, optHeader_tmp->data);
    if (nullptr == header_tmp)
    {
      curl_slist_free_all(header_chunk);
      return -1;
    }

    header_chunk = header_tmp;
    optHeader_tmp = optHeader_tmp->next;
  }

  curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header_chunk);

  if (!body.empty()) {
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, body.size());
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, body.data());
  } else {
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, 0L);
  }

  if (!auth_.empty()) {
    curl_easy_setopt(curl_, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl_, CURLOPT_USERPWD, auth_.c_str());
  }

  switch (method) {
    case HttpMethod::Post:
      curl_easy_setopt(curl_, CURLOPT_POST, 1L);
      break;

    case HttpMethod::Put:
      curl_easy_setopt(curl_, CURLOPT_NOBODY, 0L);
      curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "PUT");
      break;

    case HttpMethod::Delete:
      curl_easy_setopt(curl_, CURLOPT_HTTPGET, 0L);
      curl_easy_setopt(curl_, CURLOPT_NOBODY, 0L);
      curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "DELETE");
      break;
  }

  auto curl_error = curl_easy_perform(curl_);

  long response_code;
  curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &response_code);

  curl_slist_free_all(header_chunk);

  if (curl_error != CURLE_OK) {
    return -curl_error;
  }

  return response_code;
}

int CurlWrapper::addHttpHeader(const std::string& header)
{
  std::lock_guard<std::mutex> lock{mutex_};
  auto header_tmp = curl_slist_append(optHttpHeader_, header.c_str());
  if (nullptr == header_tmp)
  {
    return -1;
  }

  optHttpHeader_ = header_tmp;
  return 0;
}

}  // namespace detail
}  // namespace prometheus
