#pragma once

#include <iostream>
#include <utility>
#include <functional>
#include <memory>

#include "asio.hpp"

using asio::ip::tcp;

namespace alice {
    namespace network {
        class Connection : public std::enable_shared_from_this<Connection> {
            tcp::socket socket_;
            char read_buffer_[1024]{};

            void Read() {
                socket_.async_read_some(
                        asio::buffer(read_buffer_, 1024),
                        [this](const asio::error_code &ec, std::size_t bytes_transferred) {
                            if (!ec) {
                                Read();

                                std::string str(read_buffer_, bytes_transferred);

                                for (const auto &on_read_callback: on_read_callbacks) {
                                    on_read_callback(str);
                                }

                            } else {
                                for (const auto &on_disconnect_callback: on_disconnect_callbacks) {
                                    on_disconnect_callback();
                                }
                            }
                        }
                );
            }

        public:
            std::vector<std::function<void()>> on_connect_callbacks;
            std::vector<std::function<void()>> on_disconnect_callbacks;
            std::vector<std::function<void(const std::string &)>> on_read_callbacks;

            explicit Connection(tcp::socket socket) : socket_(std::move(socket)) {
                auto address = socket_.remote_endpoint().address().to_string();
                auto port = socket_.remote_endpoint().port();
                std::cout << "Connect begin: " << address << ":" << port << std::endl;

                Read();
            }

            explicit Connection(asio::io_context &io, const std::string &address, unsigned short port) : socket_(io) {
                asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(address), port);
                socket_.async_connect(
                        endpoint,
                        [this](const asio::error_code &ec) {
                            if (ec) {

                                for (const auto &on_disconnect_callback: on_disconnect_callbacks) {
                                    on_disconnect_callback();
                                }

                            } else {

                                for (const auto &on_connect_callback: on_connect_callbacks) {
                                    on_connect_callback();
                                }

                                Read();
                            }
                        }
                );
            }

            void Send(const std::string &message) {
                socket_.async_write_some(
                        asio::buffer(message),
                        [this](const asio::error_code &ec, std::size_t bytes_transferred) {
                            if (ec) {
                                for (const auto &on_disconnect_callback: on_disconnect_callbacks) {
                                    on_disconnect_callback();
                                }
                            }
                        }
                );
            }

            std::string GetAddress() const { return socket_.remote_endpoint().address().to_string(); }

            unsigned short GetPort() const { return socket_.remote_endpoint().port(); }
        };
    }
}