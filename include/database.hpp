#if !defined(ESG_DATABASE_HPP_INCLUDED_)
#define ESG_DATABASE_HPP_INCLUDED_

#include <iostream>
#include <boost/asio.hpp>
#include <sqlite3.h>
#include "logging.hpp"

class database
	: public boost::asio::io_service::service
{
public:
	static boost::asio::io_service::id id;
	database(boost::asio::io_service & io_service);
	void shutdown_service();
	/**
	 * Open database file at file
	 * @param dbfile Database file
	 */
	void open(const std::string & dbfile);
	bool is_open() const;
private:
	struct sqlite3 * db_;
};

#endif