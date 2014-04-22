#include "gg_server.hpp"

gg_server::gg_server(boost::asio::io_service & io_service)
	: io_service_(io_service)
	, acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8074))
	, manager_(boost::asio::use_service<connection_manager>(io_service_))
{
	start_accept();
}

void gg_server::start_accept()
{
	gg_connection::pointer new_connection =
		gg_connection::create(acceptor_.get_io_service());
	acceptor_.async_accept(new_connection->get_socket(),
		boost::bind(&gg_server::handle_accept, this,
			boost::asio::placeholders::error,
			new_connection));
}

void gg_server::handle_accept(const boost::system::error_code & error,
	gg_connection::pointer new_connection)
{
	if (!error)
	{
		manager_.start(new_connection);
		start_accept();
	}
}
