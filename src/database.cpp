#include "database.hpp"

boost::asio::io_service::id database::id;

database::database(boost::asio::io_service & io_service)
	: boost::asio::io_service::service(io_service)
	, db_(nullptr)
{
}

void database::shutdown_service()
{
}

void database::open(const std::string & dbfile)
{
	if (::sqlite3_open(dbfile.c_str(), &db_) != SQLITE_OK)
	{
		throw std::runtime_error("Unable to open database file at " + dbfile);
	}
}

bool database::is_open() const
{
	return db_ != nullptr;
}