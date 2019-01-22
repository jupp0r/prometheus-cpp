#pragma once

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <prometheus/text_serializer.h>

#include "prometheus/collectable.h"

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

static const std::string MIME_TYPE = "text/plain";



std::vector<prometheus::MetricFamily> CollectMetrics(const std::vector<std::weak_ptr<prometheus::Collectable>>& collectables) {
    auto collected_metrics = std::vector<prometheus::MetricFamily>{};

    for (auto&& wcollectable : collectables) {
        auto collectable = wcollectable.lock();
        if (!collectable) {
            continue;
        }

        auto&& metrics = collectable->Collect();
        collected_metrics.insert(collected_metrics.end(),
                                 std::make_move_iterator(metrics.begin()),
                                 std::make_move_iterator(metrics.end()));
    }

    return collected_metrics;
}


// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template<
        class Body, class Allocator,
        class Send>
void
handle_request(
        const std::string& uri,
        const std::vector<std::weak_ptr<prometheus::Collectable>>& collectables,
        http::request<Body, http::basic_fields<Allocator>>&& req,
        Send&& send)
{
    // Returns a bad request response
    auto const bad_request =
            [&req](boost::beast::string_view why)
            {
                http::response<http::string_body> res{http::status::bad_request, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, MIME_TYPE);
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
                res.set(http::field::content_type, MIME_TYPE);
                res.keep_alive(req.keep_alive());
                res.body() = "The resource '" + target.to_string() + "' was not found.";
                res.prepare_payload();
                return res;
            };

    // Returns collected metrics
    auto const metrics =
            [&req](const std::vector<std::weak_ptr<prometheus::Collectable>>& collectables)
            {
                auto metrics = CollectMetrics(collectables);
                auto serializer = prometheus::TextSerializer{};

                http::response<http::string_body> res{http::status::ok, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, MIME_TYPE);
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

    if( req.target() != uri ) {
        return send(not_found("Unknown URI"));
    }

    return send(metrics(collectables));
}

//------------------------------------------------------------------------------

// Report a failure
void
fail(boost::system::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

// Handles an HTTP server connection
class session : public std::enable_shared_from_this<session>
{
    // This is the C++11 equivalent of a generic lambda.
    // The function object is used to send an HTTP message.
    struct send_lambda
    {
        session& self_;

        explicit
        send_lambda(session& self)
                : self_(self)
        {
        }

        template<bool isRequest, class Body, class Fields>
        void
        operator()(http::message<isRequest, Body, Fields>&& msg) const
        {
            // The lifetime of the message has to extend
            // for the duration of the async operation so
            // we use a shared_ptr to manage it.
            auto sp = std::make_shared<
                    http::message<isRequest, Body, Fields>>(std::move(msg));

            // Store a type-erased version of the shared
            // pointer in the class to keep it alive.
            self_.res_ = sp;

            // Write the response
            http::async_write(
                    self_.socket_,
                    *sp,
                    boost::asio::bind_executor(
                            self_.strand_,
                            std::bind(
                                    &session::on_write,
                                    self_.shared_from_this(),
                                    std::placeholders::_1,
                                    std::placeholders::_2,
                                    sp->need_eof())));
        }
    };

    tcp::socket socket_;
    boost::asio::strand<
            boost::asio::io_context::executor_type> strand_;
    boost::beast::flat_buffer buffer_;
    const std::string& uri_;
    const std::vector<std::weak_ptr<prometheus::Collectable>>& collectables_;
    http::request<http::string_body> req_;
    std::shared_ptr<void> res_;
    send_lambda lambda_;

public:
    // Take ownership of the socket
    explicit
    session(
            tcp::socket socket,
            std::shared_ptr<BeastSharedState> const& state)
            : socket_(std::move(socket))
            , strand_(socket_.get_executor())
            , uri_(uri)
            , collectables_(collectables)
            , lambda_(*this)
    {
    }

    // Start the asynchronous operation
    void
    run()
    {
        do_read();
    }

    void
    do_read()
    {
        // Read a request
        http::async_read(socket_, buffer_, req_,
                         boost::asio::bind_executor(
                                 strand_,
                                 std::bind(
                                         &session::on_read,
                                         shared_from_this(),
                                         std::placeholders::_1,
                                         std::placeholders::_2)));
    }

    void
    on_read(
            boost::system::error_code ec,
            std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        // This means they closed the connection
        if(ec == http::error::end_of_stream)
            return do_close();

        if(ec)
            return fail(ec, "read");

        // Send the response
        handle_request(uri_, collectables_, std::move(req_), lambda_);
    }

    void
    on_write(
            boost::system::error_code ec,
            std::size_t bytes_transferred,
            bool close)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "write");

        if(close)
        {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            return do_close();
        }

        // We're done with the response so delete it
        res_ = nullptr;

        // Read another request
        do_read();
    }

    void
    do_close()
    {
        // Send a TCP shutdown
        boost::system::error_code ec;
        socket_.shutdown(tcp::socket::shutdown_send, ec);

        // At this point the connection is closed gracefully
    }
};

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener>
{
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    const std::string& uri_;
    const std::vector<std::weak_ptr<prometheus::Collectable>>& collectables_;

public:
    listener(
            boost::asio::io_context& ioc,
            tcp::endpoint endpoint,
            const std::string& uri,
            const std::vector<std::weak_ptr<prometheus::Collectable>>& collectables)
            : acceptor_(ioc)
            , socket_(ioc), uri_(uri)
            , collectables_(collectables)
    {
        boost::system::error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if(ec)
        {
            fail(ec, "open");
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if(ec)
        {
            fail(ec, "bind");
            return;
        }
        std::cerr << "bound to: " << endpoint << "\n";

        // Start listening for connections
        acceptor_.listen(
                boost::asio::socket_base::max_listen_connections, ec);
        if(ec)
        {
            fail(ec, "listen");
            return;
        }
    }

    // Start accepting incoming connections
    void
    run()
    {
        if(! acceptor_.is_open())
            return;
        do_accept();
    }

    void
    do_accept()
    {
        acceptor_.async_accept(
                socket_,
                std::bind(
                        &listener::on_accept,
                        shared_from_this(),
                        std::placeholders::_1));
    }

    void
    on_accept(boost::system::error_code ec)
    {
        if(ec)
        {
            fail(ec, "accept");
        }
        else
        {
            // Create the session and run it
            std::make_shared<session>(
                    std::move(socket_), uri_,
                    collectables_)->run();
        }

        // Accept another connection
        do_accept();
    }
};
