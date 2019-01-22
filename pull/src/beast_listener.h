#pragma once

#include <memory>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

class BeastSharedState;

class BeastListener : public std::enable_shared_from_this<BeastListener>
{
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    std::shared_ptr<BeastSharedState> state_;

    void fail(boost::beast::error_code ec, char const* what);
    void on_accept(boost::beast::error_code ec);

public:
    BeastListener(
            boost::asio::io_context& ioc,
            boost::asio::ip::tcp::endpoint endpoint,
            std::shared_ptr<BeastSharedState> const& state);

    // Start accepting incoming connections
    void run();
};
