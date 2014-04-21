#include <iostream>
#include "gg_server.hpp"

int
main(int argc, char * argv[])
{
	boost::asio::io_service io_service;
	gg_server srv(io_service);
	io_service.run();
}
