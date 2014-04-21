#include "gg_connection.hpp"

gg_connection::gg_connection(boost::asio::io_service & io_service)
	: io_service_(io_service)
	, socket_(io_service_)
{

}

void gg_connection::start()
{
	
}