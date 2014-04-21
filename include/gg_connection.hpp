#if !defined(ESG_GG_CONNECTION_HPP_INCLUDED_)
#define ESG_GG_CONNECTION_HPP_INCLUDED_

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include "libgadu.h"

class gg_connection
	: public boost::enable_shared_from_this<gg_connection>
{
public:
	typedef boost::shared_ptr<gg_connection> pointer;
	static pointer create(boost::asio::io_service & io_service)
	{
		return pointer(new gg_connection(io_service));
	}
	boost::asio::ip::tcp::socket & get_socket()
	{
		return socket_;
	}
	void start();
	void handle_read_gg_header(const boost::system::error_code & errorr,
		std::size_t bytes_transferred, boost::shared_ptr<struct gg_header> gg_header);
private:
	gg_connection(boost::asio::io_service & io_service);
	boost::asio::io_service & io_service_;
	boost::asio::ip::tcp::socket socket_;
};

#endif /* ESG_GG_CONNECTION_HPP_INCLUDED_ */