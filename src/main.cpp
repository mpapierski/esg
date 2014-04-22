#include <iostream>
#include "gg_server.hpp"
#include "database.hpp"
#include "logging.hpp"

int
main(int argc, char * argv[])
{
	INFO("Eksperymentaly Serwer Gadu-Gadu");
	boost::asio::io_service io_service;
	// Create database service
	database * db_svc = new database(io_service);
	boost::asio::add_service(io_service, db_svc); // Transfer ownership 
	try
	{
		db_svc->open("db.sqlite");
	} catch (std::exception & e)
	{
		ERR("%s", e.what());
		return 1;
	}
	// GG protocol listener
	gg_server srv(io_service);
	io_service.run();
}
