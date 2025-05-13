# ping_receiver

PING Receiver is a C++ application that sends ICMP Echo Requests (ping) to a specified IP address at 1-second intervals and receives and displays the responses. It is useful for checking network connectivity and performing basic communication diagnostics.

## Features

- Asynchronous ICMP communication using Boost.Asio

- Automatic ping transmission every 1 second

- Displays log output when ICMP Echo Replies are received

- Includes setup.bat script for automatic environment setup (downloads and builds Boost, compiles the project)




## Setup
Clone the repository and run setup.bat. It will automatically download Boost, build the libraries, and compile the executable.

``` bash
git clone https://github.com/you3kou3/ping_receiver.git
cd ping_receiver
setup.bat
```

## Usage
After building, run the program as follows:

```bash
ping_receiver.exe <IP_ADDRESS>
```

Example:
```bash
ping_receiver.exe 192.168.1.1
```


## Overview
- Sends a ping every second to the specified IP address

- Displays logs of ICMP Echo Replies, including sender address and response time

- Uses asynchronous I/O with Boost.Asio

