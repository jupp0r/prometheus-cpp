#include <iostream>

#include <boost/config.hpp>

#include "beast_shared_state.h"
#include "beast_session.h"
#include "metric_collector.h"
#include "prometheus/text_serializer.h"

//#define BOOST_NO_CXX14_GENERIC_LAMBDAS

using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace http = beast::http;

static const std::string TEXT_PLAIN = "text/plain";

//------------------------------------------------------------------------------
#if 0
// Return a reasonable mime type based on the extension of a file.
beast::string_view
mime_type(beast::string_view path)
{
    using beast::iequals;
    auto const ext = [&path]
    {
        auto const pos = path.rfind(".");
        if(pos == beast::string_view::npos)
            return beast::string_view{};
        return path.substr(pos);
    }();
    if(iequals(ext, ".htm"))  return "text/html";
    if(iequals(ext, ".html")) return "text/html";
    if(iequals(ext, ".php"))  return "text/html";
    if(iequals(ext, ".css"))  return "text/css";
    if(iequals(ext, ".txt"))  return "text/plain";
    if(iequals(ext, ".js"))   return "application/javascript";
    if(iequals(ext, ".json")) return "application/json";
    if(iequals(ext, ".xml"))  return "application/xml";
    if(iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if(iequals(ext, ".flv"))  return "video/x-flv";
    if(iequals(ext, ".png"))  return "image/png";
    if(iequals(ext, ".jpe"))  return "image/jpeg";
    if(iequals(ext, ".jpeg")) return "image/jpeg";
    if(iequals(ext, ".jpg"))  return "image/jpeg";
    if(iequals(ext, ".gif"))  return "image/gif";
    if(iequals(ext, ".bmp"))  return "image/bmp";
    if(iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if(iequals(ext, ".tiff")) return "image/tiff";
    if(iequals(ext, ".tif"))  return "image/tiff";
    if(iequals(ext, ".svg"))  return "image/svg+xml";
    if(iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string
path_cat(
        beast::string_view base,
        beast::string_view path)
{
    if(base.empty())
        return path.to_string();
    std::string result = base.to_string();
#if BOOST_MSVC
    char constexpr path_separator = '\\';
    if(result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    for(auto& c : result)
        if(c == '/')
            c = path_separator;
#else
    char constexpr path_separator = '/';
    if(result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
#endif
    return result;
}
#endif
// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template<
        class Body, class Allocator,
        class Send>
void
handle_request(
        std::shared_ptr<BeastSharedState> const& state,
        http::request<Body, http::basic_fields<Allocator>>&& req,
        Send&& send)
{
    // Returns a bad request response
    auto const bad_request =
            [&req](boost::beast::string_view why)
            {
                http::response<http::string_body> res{http::status::bad_request, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, TEXT_PLAIN);
                res.keep_alive(req.keep_alive());
                res.body() = why.to_string();
                res.prepare_payload();
                return res;
            };

    // Returns a not found response
    auto const not_found =
            [&req](boost::beast::string_view target)
            {
                http::response<http::string_body> res{http::status::not_found, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, TEXT_PLAIN);
                res.keep_alive(req.keep_alive());
                res.body() = "The resource '" + target.to_string() + "' was not found.";
                res.prepare_payload();
                return res;
            };

    // Returns collected metrics
    auto const metrics =
            [&req](const std::vector<std::weak_ptr<prometheus::Collectable>>& collectables)
            {
                auto metrics = prometheus::CollectMetrics(collectables);
                auto serializer = prometheus::TextSerializer{};

                http::response<http::string_body> res{http::status::ok, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, TEXT_PLAIN);
                res.keep_alive(req.keep_alive());
                res.body() = serializer.Serialize(metrics);
                res.prepare_payload();
                return res;
            };

// Make sure we can handle the method
if( req.method() != http::verb::get )
return send(bad_request("Unknown HTTP-method"));

// Request path must be absolute and not contain "..".
if( req.target().empty() ||
req.target()[0] != '/' ||
req.target().find("..") != boost::beast::string_view::npos)
return send(bad_request("Illegal request-target"));

if( req.target() != state->get_uri() ) {
return send(not_found("Unknown URI"));
}

return send(metrics(state->get_collectables()));
}

//------------------------------------------------------------------------------

http_session::
http_session(
        tcp::socket socket,
        std::shared_ptr<BeastSharedState> const& state)
        : socket_(std::move(socket))
        , state_(state)
{
}

void
http_session::
run()
{
    // Read a request
    http::async_read(socket_, buffer_, req_,
                     std::bind(
                             &http_session::on_read,
                             shared_from_this(),
                             std::placeholders::_1,
                             std::placeholders::_2));
}

// Report a failure
void
http_session::
fail(beast::error_code ec, char const* what)
{
    // Don't report on canceled operations
    if(ec == boost::asio::error::operation_aborted)
        return;

    std::cerr << what << ": " << ec.message() << "\n";
}

template<bool isRequest, class Body, class Fields>
void
http_session::
send_lambda::
operator()(http::message<isRequest, Body, Fields>&& msg) const
{
    // The lifetime of the message has to extend
    // for the duration of the async operation so
    // we use a shared_ptr to manage it.
    auto sp = std::make_shared<
            http::message<isRequest, Body, Fields>>(std::move(msg));

    // Write the response
    auto self = self_.shared_from_this();
    http::async_write(
            self_.socket_,
            *sp,
            [self, sp](beast::error_code ec, std::size_t bytes)
            {
                self->on_write(ec, bytes, sp->need_eof());
            });
}

void
http_session::
on_read(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    // This means they closed the connection
    if(ec == http::error::end_of_stream)
    {
        socket_.shutdown(tcp::socket::shutdown_send, ec);
        return;
    }

    // Handle the error, if any
    if(ec)
        return fail(ec, "read");

    // Send the response
#ifndef BOOST_NO_CXX14_GENERIC_LAMBDAS
    //
    // The following code requires generic
    // lambdas, available in C++14 and later.
    //
    handle_request(
        state_,
        std::move(req_),
        [this](auto&& response)
        {
            // The lifetime of the message has to extend
            // for the duration of the async operation so
            // we use a shared_ptr to manage it.
            using response_type = typename std::decay<decltype(response)>::type;
            auto sp = std::make_shared<response_type>(std::forward<decltype(response)>(response));

        #if 0
            // NOTE This causes an ICE in gcc 7.3
            // Write the response
            http::async_write(this->socket_, *sp,
				[self = shared_from_this(), sp](
					beast::error_code ec, std::size_t bytes)
				{
					self->on_write(ec, bytes, sp->need_eof()); 
				});
        #else
            // Write the response
            auto self = shared_from_this();
            http::async_write(this->socket_, *sp,
				[self, sp](
					beast::error_code ec, std::size_t bytes)
				{
					self->on_write(ec, bytes, sp->need_eof()); 
				});
        #endif
        });
#else
    //
    // This code uses the function object type send_lambda in
    // place of a generic lambda which is not available in C++11
    //
    handle_request(
            state_,
            std::move(req_),
            send_lambda(*this));

#endif
}

void
http_session::
on_write(boost::beast::error_code ec, std::size_t bytes_transferred, bool close)
{
    // Handle the error, if any
    if(ec)
        return fail(ec, "write");

    if(close)
    {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        socket_.shutdown(tcp::socket::shutdown_send, ec);
        return;
    }

    // Clear contents of the request message,
    // otherwise the read behavior is undefined.
    req_ = {};

    // Read another request
    http::async_read(socket_, buffer_, req_,
                     std::bind(
                             &http_session::on_read,
                             shared_from_this(),
                             std::placeholders::_1,
                             std::placeholders::_2));
}