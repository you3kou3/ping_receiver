#pragma once

#include "Pinger.hpp"


#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/attributes.hpp>

#include <boost/thread.hpp>
#include <boost/chrono/duration.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

namespace pt = boost::posix_time;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;


static unsigned short get_identifier()
{
#if defined(BOOST_WINDOWS)
    return static_cast<unsigned short>(::GetCurrentProcessId());
#else
    return static_cast<unsigned short>(::getpid());
#endif
}


/*******************************************************
 *    Constructor
 *******************************************************/
pinger::pinger(boost::asio::io_service& io_service, const char* destination)
    : resolver_(io_service), socket_(io_service, icmp::v4()),
    timer_(io_service), sequence_number_(0), num_replies_(0)
{

    init_log();

    icmp::resolver::query query(icmp::v4(), destination, "");
    destination_ = *resolver_.resolve(query);
    start_send();
    start_receive();

}


/*******************************************************
 *    prepare to send ping
 *******************************************************/
void pinger::start_send()
{
    std::string body("\"Hello!\" from Asio ping.");

    // Create an ICMP header for an echo request.
    icmp_header echo_request;
    echo_request.type(icmp_header::echo_request);
    echo_request.code(0);
    echo_request.identifier(get_identifier());
    echo_request.sequence_number(++sequence_number_);
    compute_checksum(echo_request, body.begin(), body.end());

    // Encode the request packet.
    boost::asio::streambuf request_buffer;
    std::ostream os(&request_buffer);
    os << echo_request << body;

    // Send the request.
    time_sent_ = posix_time::microsec_clock::universal_time();
    socket_.send_to(request_buffer.data(), destination_);

    // Wait up to five seconds for a reply.
    num_replies_ = 0;
    timer_.expires_at(time_sent_ + posix_time::seconds(5));
    timer_.async_wait(boost::bind(&pinger::handle_timeout, this));
}

/*******************************************************
 *    complete sending ping 
 *******************************************************/
void pinger::handle_timeout()
{
    if (num_replies_ == 0) {
        std::cout << "Request timed out" << std::endl;
        BOOST_LOG_TRIVIAL(error) << "Request timed out";
    }

    // Requests must be sent no less than one second apart.
    timer_.expires_at(time_sent_ + posix_time::seconds(1));
    timer_.async_wait(boost::bind(&pinger::start_send, this));
}


/*******************************************************
 *    prepare to receive ping
 *******************************************************/
void pinger::start_receive()
{
    // Discard any data already in the buffer.
    reply_buffer_.consume(reply_buffer_.size());

    // Wait for a reply. We prepare the buffer to receive up to 64KB.
    socket_.async_receive(reply_buffer_.prepare(65536),
        boost::bind(&pinger::handle_receive, this, _2));
}


/*******************************************************
 *    complete receiving ping
 *******************************************************/
void pinger::handle_receive(std::size_t length)
{
    // The actual number of bytes received is committed to the buffer so that we
    // can extract it using a std::istream object.
    reply_buffer_.commit(length);

    // Decode the reply packet.
    std::istream is(&reply_buffer_);
    ipv4_header ipv4_hdr;
    icmp_header icmp_hdr;
    is >> ipv4_hdr >> icmp_hdr;

    // We can receive all ICMP packets received by the host, so we need to
    // filter out only the echo replies that match the our identifier and
    // expected sequence number.
    if (is && icmp_hdr.type() == icmp_header::echo_reply
        && icmp_hdr.identifier() == get_identifier()
        && icmp_hdr.sequence_number() == sequence_number_)
    {
        // If this is the first reply, interrupt the five second timeout.
        if (num_replies_++ == 0)
            timer_.cancel();

        // Print out some information about the reply packet.
        posix_time::ptime now = posix_time::microsec_clock::universal_time();



        auto now_ms = get_nowtime();

        std::cout 
            << now_ms << ", " << length - ipv4_hdr.header_length()
            << " bytes from " << ipv4_hdr.source_address()
            << ": icmp_seq=" << icmp_hdr.sequence_number()
            << ", ttl=" << ipv4_hdr.time_to_live()
            << ", time1=" << (now - time_sent_).total_milliseconds() << " ms"
            << std::endl;

        BOOST_LOG_TRIVIAL(info) 
            << length - ipv4_hdr.header_length() << "bytes from " << ipv4_hdr.source_address()
            << ", icmp_seq=" << icmp_hdr.sequence_number()
            << ", ttl=" << ipv4_hdr.time_to_live()
            << ", time=" << (now - time_sent_).total_milliseconds() << " ms";
    }

    start_receive();
}

/*******************************************************
 *    get time in milliseconds 
 *******************************************************/
boost::posix_time::ptime pinger::get_nowtime()
{
    time_t nowtime;
    time(&nowtime);

    pt::ptime current_date_microseconds 
        = pt::microsec_clock::local_time();

    long milliseconds 
        = current_date_microseconds.time_of_day().total_milliseconds();

    pt::time_duration current_time_milliseconds 
        = pt::milliseconds(milliseconds);

    pt::ptime current_date_milliseconds(
        current_date_microseconds.date(),
        current_time_milliseconds
    );

    return current_date_milliseconds;
}

/*******************************************************
 *    initialize loggin format
 *******************************************************/
void pinger::init_log()
{
    boost::log::add_file_log(
        keywords::file_name = "logs/%Y_%m_%d.log",
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::format = "%TimeStamp%, %Severity%, %Message%",
        keywords::auto_flush = true,
        keywords::open_mode = (std::ios::out | std::ios::app)
    );
    boost::log::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
}



