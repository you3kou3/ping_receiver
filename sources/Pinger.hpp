#pragma once


#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <istream>
#include <iostream>
#include <ostream>

#include "icmp_header.hpp"
#include "ipv4_header.hpp"


using boost::asio::ip::icmp;
using boost::asio::deadline_timer;
namespace posix_time = boost::posix_time;


class pinger
{
public:
    pinger(boost::asio::io_service& io_service, const char* destination);
    ~pinger() {};

private:
    void start_send();
    void handle_timeout();
    void start_receive();
    void handle_receive(std::size_t length);
    boost::posix_time::ptime get_nowtime();
    void init_log();

private:
    icmp::resolver resolver_;
    icmp::endpoint destination_;
    icmp::socket socket_;
    deadline_timer timer_;
    unsigned short sequence_number_;
    posix_time::ptime time_sent_;
    boost::asio::streambuf reply_buffer_;
    std::size_t num_replies_;
};
