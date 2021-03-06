#if !defined(ESG_GG_SERVER_HPP_INCLUDED_)
#define ESG_GG_SERVER_HPP_INCLUDED_

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include "gg_connection.hpp"
#include "connection_manager.hpp"

class gg_server
	: private boost::noncopyable
{
public:
	gg_server(boost::asio::io_service & io_service);
	void start_accept();
	void handle_accept(const boost::system::error_code & ec,
		gg_connection::pointer new_connection);
public:
	boost::asio::io_service & io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;
	connection_manager & manager_;
};

#endif /* ESG_GG_SERVER_HPP_INCLUDED_ */