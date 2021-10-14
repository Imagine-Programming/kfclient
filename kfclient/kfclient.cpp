#include "kfclient.hpp"

#include <boost/bind.hpp>

#include <stdexcept>
#include <vector>
#include <iostream>

kfc::kfclient::kfclient(io_context& context, const udp::resolver::results_type& endpoints, std::size_t receive_buffer_size) 
    : io_context_(context), socket_(context), recvbuf_(receive_buffer_size), challenge_(0) {
        do_connect(endpoints);
}

void kfc::kfclient::do_connect(const udp::resolver::results_type& endpoints) {
    boost::system::error_code error;
    socket_.connect(*endpoints.begin(), error);
    if (error) 
        throw std::runtime_error(error.message());
}

const kfc::kfdetails& kfc::kfclient::request_details() {
    details_ = nullptr;
    do_request(PACKET_DETAILS, REQUEST_DETAILS);
    if (details_ == nullptr)
        throw std::runtime_error("request_details failed, received response could not be processed");
    return *details_;
}

const kfc::kfrules& kfc::kfclient::request_rules() {
    rules_ = nullptr;
    do_request(PACKET_RULES, REQUEST_RULES);
    if (rules_ == nullptr)
        throw std::runtime_error("request_rules failed, received response could not be processed");
    return *rules_;
}

const kfc::kfplayers& kfc::kfclient::request_players() {
    players_ = nullptr;
    do_request(PACKET_PLAYERS, REQUEST_PLAYERS);
    if (players_ == nullptr)
        throw std::runtime_error("request_players failed, received repsonse could not be processed");
    return *players_;
}

void kfc::kfclient::do_challenge() {
    boost::system::error_code error;
    socket_.send(boost::asio::buffer(REQUEST_CHALLENGE, sizeof(REQUEST_CHALLENGE)), 0, error);

    if (error) 
        throw std::runtime_error(error.message());
    
    process_response(PACKET_CHALLENGE);
}

void kfc::kfclient::process_response(std::int8_t expected_packet) {
    boost::system::error_code error;
    socket_.receive(boost::asio::buffer(recvbuf_.data(), recvbuf_.size()), 0, error);

    if (error)
        throw std::runtime_error(error.message());

    recvbuf_.rewind();

    kfheader header;
    recvbuf_.consume(header.magic);
    recvbuf_.consume(header.type);

    if (header.magic != -1)
        throw std::runtime_error("unexpected header magic received");
    
    if (expected_packet != -1 && header.type != expected_packet)
        throw std::runtime_error("unexpected packet received");

    switch (header.type) {
    case PACKET_CHALLENGE: {
        challenge_ = recvbuf_.consume<std::uint32_t>();
    } break;
    case PACKET_DETAILS: {
        details_ = std::make_unique<kfdetails>(recvbuf_);
    } break;
    case PACKET_RULES: {
        rules_ = std::make_unique<kfrules>(recvbuf_);
    } break;
    case PACKET_PLAYERS: {
        players_ = std::make_unique<kfplayers>(recvbuf_);
    } break;
    default:
        throw std::runtime_error("unexpected result type received");
    }
}