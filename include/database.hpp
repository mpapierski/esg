#if !defined(ESG_DATABASE_HPP_INCLUDED_)
#define ESG_DATABASE_HPP_INCLUDED_

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>
#include "sqlite3.h"
#include "libgadu.h"
#include "logging.hpp"

class database
	: public boost::asio::io_service::service
{
public:
	static boost::asio::io_service::id id;
	database(boost::asio::io_service & io_service);
	~database();
	void shutdown_service();
	/**
	 * Open database file at file
	 * @param dbfile Database file
	 */
	void open(const std::string & dbfile);
	bool is_open() const;
	
	struct noop_callback{};
	/**
	 * Execute query with the need for result
	 * Useful for: DML statements
	 * Callback arguments: int argc, char * rows, char * cols
	 */
	template <typename Callback, typename... Args>
	void execute_query(const std::string & query, Callback callback, Args... args);
private:
	struct sqlite3 * db_;
	// Install custom database functions
	void create_functions();
	void func_gg_login_hash_sha1(sqlite3_context* ctx, int argc, sqlite3_value** argv);
	void func_gg_login_hash(sqlite3_context* ctx, int argc, sqlite3_value** argv);
	/**
	 * SQLite wrapper for "gg_login_hash_sha1" function.
	 */
	//void func_gg_login_hash_sha1(sqlite_func* fn, int, const char**);
	/// Migrations
	typedef void (database::*migration_function_type)();
	// Execute all migrations stored in `migrations_` array.
	void migrate();
	// List of database schema migrations
	static migration_function_type migrations_[];
	void migration_0001_create_schema();
};

#include "database-inl.hpp"

#endif