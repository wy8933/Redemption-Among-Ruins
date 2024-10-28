#pragma once

#include <iostream>
#include <vector>

#include "connection.h"

namespace alice {
    namespace network {
        class TcpServer {
            asio::io_context &io_;
            tcp::acceptor acceptor_;
            std::vector<std::shared_ptr<Connection>> connections_;

            void Accept() {
                acceptor_.async_accept(
                        [this](const asio::error_code &error, tcp::socket socket) {
                            std::shared_ptr<Connection> connection = std::make_shared<Connection>(std::move(socket));

                            connection->on_disconnect_callbacks.emplace_back(
                                    [this, connection]() {
                                        auto it = connections_.begin();
                                        while (it != connections_.end()) {
                                            if ((*it).get() == connection.get()) {

                                                std::cerr << "Connection lost: "
                                                          << (*it)->GetAddress()
                                                          << ":"
                                                          << (*it)->GetPort()
                                                          << std::endl;

                                                it = connections_.erase(it);
                                            } else {
                                                ++it;
                                            }
                                        }
                                    }
                            );

                            connections_.push_back(connection);
                            Accept();
                        }
                );
            }

        public:
            explicit TcpServer(asio::io_context &io, unsigned short port = 0)
                    : io_(io), acceptor_(io_, tcp::endpoint(tcp::v4(), port)) {
                Accept();
            }
        };
    }
}