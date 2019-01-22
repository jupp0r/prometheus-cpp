#pragma once

#include <cstdlib>
#include <memory>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "beast_shared_state.h"

/** Represents an established HTTP connection
*/
class http_session : public std::enable_shared_from_this<http_session>
{
    boost::asio::ip::tcp::socket socket_;
    boost::beast::flat_buffer buffer_;
    std::shared_ptr<BeastSharedState> state_;
    boost::beast::http::request<boost::beast::http::string_body> req_;

    struct send_lambda
    {
        http_session& self_;

        explicit
        send_lambda(http_session& self)
                : self_(self)
        {
        }

        template<bool isRequest, class Body, class Fields>
        void
        operator()(boost::beast::http::message<isRequest, Body, Fields>&& msg) const;
    };

    void fail(boost::beast::error_code ec, char const* what);
    void on_read(boost::beast::error_code ec, std::size_t);
    void on_write(boost::beast::error_code ec, std::size_t, bool close);

public:
    http_session(
            boost::asio::ip::tcp::socket socket,
            std::shared_ptr<BeastSharedState> const& state);

    void run();
};