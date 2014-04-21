#include "gg_connection.hpp"

gg_connection::gg_connection(boost::asio::io_service & io_service)
	: io_service_(io_service)
	, socket_(io_service_)
{

}

void gg_connection::start()
{
	// Send GG_WELCOME to client and wait for packets AFTER seed was delivered.
	send_gg_welcome();
}

void gg_connection::begin_read_gg_header()
{
	// Begin asynchronous operation
	boost::asio::async_read(socket_,
		boost::asio::buffer(&gg_header_, sizeof(struct gg_header)),
		boost::bind(&gg_connection::handle_read_gg_header, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void gg_connection::handle_read_gg_header(const boost::system::error_code & error,
	std::size_t bytes_transferred)
{
	if (!error)
	{
		gg_event_.resize(gg_header_.length);
		boost::asio::async_read(socket_, boost::asio::buffer(gg_event_),
			boost::bind(&gg_connection::handle_read_gg_event, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
}

void gg_connection::handle_read_gg_event(const boost::system::error_code & error,
	std::size_t bytes_transferred)
{
	if (!error)
	{
		std::cout << "Received gg event" << std::endl;
		begin_read_gg_header();
	}
}

void gg_connection::send_gg_welcome()
{
	// Chosen by fair dice roll. Guarenteed to be random.
	seed_ = 69;

	boost::asio::streambuf::mutable_buffers_type bufs =
		write_buffer_.prepare(sizeof(struct gg_header) + sizeof(struct gg_welcome));
	
	struct gg_header * header = boost::asio::buffer_cast<struct gg_header *>(bufs);
	header->type = GG_WELCOME;
	header->length = sizeof(struct gg_welcome);

	struct gg_welcome * welcome = boost::asio::buffer_cast<struct gg_welcome *>(bufs + sizeof(struct gg_header));
	welcome->key = seed_;
		
	boost::asio::async_write(socket_, bufs,
		boost::bind(&gg_connection::handle_write_gg_welcome, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void gg_connection::handle_write_gg_welcome(const boost::system::error_code & error,
	std::size_t bytes_transferred)
{
	if (!error)
	{
		write_buffer_.consume(bytes_transferred);
		std::cout << "Success " << bytes_transferred << std::endl;
		begin_read_gg_header();
	}
}