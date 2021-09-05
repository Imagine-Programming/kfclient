#ifndef kfclient_client_hpp
#define kfclient_client_hpp

#include "libdef.hpp"
#include "kfbuffer.hpp"
#include "kfdetails.hpp"
#include "kfrules.hpp"
#include "kfplayers.hpp"

#include <boost/asio.hpp>

#include <cstdint>
#include <cstdlib>
#include <memory>

namespace kfc {
    class KFCLIENT_API kfclient {
        using io_context = boost::asio::io_context;
        using udp = boost::asio::ip::udp;

        static constexpr const char PACKET_CHALLENGE = 'A';
        static constexpr const char PACKET_PLAYERS = 'D';
        static constexpr const char PACKET_DETAILS = 'I';
        static constexpr const char PACKET_RULES = 'E';

        static constexpr const std::uint8_t REQUEST_CHALLENGE[] = {
            0xFF, 0xFF, 0xFF, 0xFF, 0x55, 0xFF, 0xFF, 0xFF, 0xFF
        };

        static constexpr const std::uint8_t REQUEST_DETAILS[] = {
			0xFF, 0xFF, 0xFF, 0xFF, 0x54, 0x53, 0x6F, 0x75, 0x72, 0x63, 0x65, 0x20, 0x45,
			0x6E, 0x67, 0x69, 0x6E, 0x65, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79, 0x00
        };

        static constexpr const std::uint8_t REQUEST_PLAYERS[] = {
            0xFF, 0xFF, 0xFF, 0xFF, 0x55
        };

        static constexpr const std::uint8_t REQUEST_RULES[] = {
            0xFF, 0xFF, 0xFF, 0xFF, 0x56
        };
    public:
        kfclient(io_context& context, const udp::resolver::results_type& endpoints, std::size_t receive_buffer_size = 2048);

        const kfdetails& request_details();
        const kfrules& request_rules();
        const kfplayers& request_players();

        void do_challenge();
        void do_request(const char packet, const std::uint8_t* request, std::size_t size);
        void process_response(char expected_packet);

    private:
        void do_connect(const udp::resolver::results_type& endpoints);

        io_context& io_context_;
        udp::socket socket_;
        kfbuffer recvbuf_;
        std::int32_t challenge_;

        std::unique_ptr<kfdetails> details_;
        std::unique_ptr<kfrules> rules_;
        std::unique_ptr<kfplayers> players_;
    };
}

#endif 