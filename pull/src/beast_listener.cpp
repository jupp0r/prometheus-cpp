#include "beast_listener.h"
#include "beast_session.h"
#include <iostream>

BeastListener::
BeastListener(
        boost::asio::io_context& ioc,
        boost::asio::ip::tcp::endpoint endpoint,
        std::shared_ptr<BeastSharedState> const& state)
        : acceptor_(ioc)
        , socket_(ioc)
        , state_(state)
{
    boost::beast::error_code ec;

    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if(ec)
    {
        fail(ec, "open");
        return;
    }

    // Allow address reuse
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
    if(ec)
    {
        fail(ec, "set_option");
        return;
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if(ec)
    {
        fail(ec, "bind");
        return;
    }

    // Start listening for connections
    acceptor_.listen(
            boost::asio::socket_base::max_listen_connections, ec);
    if(ec)
    {
        fail(ec, "listen");
        return;
    }
}

void
BeastListener::
run()
{
    // Start accepting a connection
    acceptor_.async_accept(
            socket_,
            std::bind(
                    &BeastListener::on_accept,
                    shared_from_this(),
                    std::placeholders::_1));
}

// Report a failure
void
BeastListener::
fail(boost::beast::error_code ec, char const* what)
{
    // Don't report on canceled operations
    if(ec == boost::asio::error::operation_aborted)
        return;
    std::cerr << what << ": " << ec.message() << "\n";
}

// Handle a connection
void
BeastListener::
on_accept(boost::beast::error_code ec)
{
    if(ec)
        return fail(ec, "accept");
    else
        // Launch a new session for this connection
        std::make_shared<http_session>(
                std::move(socket_),
                state_)->run();

    // Accept another connection
    acceptor_.async_accept(
            socket_,
            std::bind(
                    &BeastListener::on_accept,
                    shared_from_this(),
                    std::placeholders::_1));
}