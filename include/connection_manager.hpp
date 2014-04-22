#if !defined(ESG_CONNECTION_MANAGER_HPP_INCLUDED_)
#define ESG_CONNECTION_MANAGER_HPP_INCLUDED_

#include <set>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

struct gg_connection;

class connection_manager
	: public boost::asio::io_service::service
{
public:
	static boost::asio::io_service::id id;
	connection_manager(boost::asio::io_service & io_service);
	void shutdown_service();
	/**
	 * Add new connection to the pool
	 */
	void start(const boost::shared_ptr<gg_connection> & conn);
	/**
	 * Stop connection in the pool
	 */
	void stop(const boost::shared_ptr<gg_connection> & conn);
	/**
	 * Find connection using its uin
	 */
	boost::shared_ptr<gg_connection> find_by_uin(int uin);

private:
	std::set<boost::shared_ptr<gg_connection> > connections_;
};

#endif