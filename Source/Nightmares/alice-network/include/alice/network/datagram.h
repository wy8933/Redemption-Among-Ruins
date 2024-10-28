#pragma once

#include <iostream>

#include "asio.hpp"

using asio::ip::udp;

namespace alice {
    namespace network {
        class Datagram {
            udp::socket socket_;
            char read_buffer[1024]{};

            void Receive() {
                udp::endpoint endpoint;
                socket_.async_receive_from(
                        asio::buffer(read_buffer, 1024),
                        endpoint,
                        [this](const asio::error_code &ec, std::size_t bytes_transferred) {
                            Receive();

                            if (bytes_transferred > 0) {
                                std::string message(read_buffer, bytes_transferred);
                                std::cout << "[UDP]: " << message << std::endl;
                            }
                        }
                );
            }

        public:
            explicit Datagram(asio::io_context &io, unsigned short port = 0)
                    : socket_(io, udp::endpoint(udp::v4(), port)) {
                Receive();
            }
        };
    }
}