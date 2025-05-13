


#include "Pinger.hpp"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;


static int end_flag = 0;


void on_cancel_signal(const boost::system::error_code& error, int signal_number)
{
    if (!error && signal_number == SIGINT) {
        std::cout << "on cancel signal" << std::endl;
        end_flag = 1;
    }
}


int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: ping <host>" << std::endl;
#if !defined(BOOST_WINDOWS)
            std::cerr << "(You may need to run this program as root.)" << std::endl;
#endif
            return 1;
        }


 
        boost::asio::io_service io_service;
        pinger p(io_service, argv[1]);
        io_service.run();


        boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
        signals.async_wait(on_cancel_signal);


        while (!end_flag)
        {
            io_service.run_one();
            std::this_thread::sleep_for(1000ms);
        }
        io_service.stop();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}


