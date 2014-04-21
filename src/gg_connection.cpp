#include "gg_connection.hpp"

gg_connection::gg_connection(boost::asio::io_service & io_service)
	: io_service_(io_service)
	, socket_(io_service_)
{

}

void gg_connection::start()
{
	// Create gg_header and read data from client directly into it
	boost::shared_ptr<struct gg_header> header = boost::make_shared<struct gg_header>();
	auto bufs = boost::asio::buffer(
		reinterpret_cast<char *>(header.get()),
		sizeof(struct gg_header));
	// Begin asynchronous operation
	boost::asio::async_read(socket_, bufs,
		boost::bind(&gg_connection::handle_read_gg_header, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred,
			header));
}

void gg_connection::handle_read_gg_header(const boost::system::error_code & error,
	std::size_t bytes_transferred, boost::shared_ptr<struct gg_header> gg_header)
{
	if (!error)
	{
	}
}