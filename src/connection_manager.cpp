#include "connection_manager.hpp"
#include "gg_connection.hpp"

boost::asio::io_service::id connection_manager::id;

connection_manager::connection_manager(boost::asio::io_service & io_service)
	: boost::asio::io_service::service(io_service)
{

}

void connection_manager::shutdown_service()
{

}

void connection_manager::start(const boost::shared_ptr<gg_connection> & conn)
{
	connections_.insert(conn);
	conn->start();
}

void connection_manager::stop(const boost::shared_ptr<gg_connection> & conn)
{
	connections_.erase(conn);
}

boost::shared_ptr<gg_connection> connection_manager::find_by_uin(int uin)
{
	return boost::shared_ptr<gg_connection>();
}
