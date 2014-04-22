#include "database.hpp"

boost::asio::io_service::id database::id;

// Pass as callback parameter to execute_query with DML statement
static auto NOOP = [&](int argc, char**, char**) -> int
{
	assert(!"Called");
	return 0;
};

database::migration_function_type database::migrations_[] = {
	&database::migration_0001_create_schema,
	nullptr
};

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
	DBUG("open %s", dbfile.c_str());
	if (::sqlite3_open(dbfile.c_str(), &db_) != SQLITE_OK)
	{
		throw std::runtime_error("Unable to open database file at " + dbfile);
	}
	execute_query("CREATE TABLE IF NOT EXISTS history (value INTEGER NOT NULL)", NOOP);
	migrate();
}

bool database::is_open() const
{
	return db_ != nullptr;
}

void database::migrate()
{
	assert(db_ != nullptr);
	// Get current migration history
	int current_value = std::numeric_limits<int>::min();
	execute_query("SELECT MAX(value) FROM history", [&current_value](int argc, char ** rows, char ** cols) {
		if (!rows[0])
		{
			// Empty history
			current_value = -1;
			return 0;
		}
		current_value = boost::lexical_cast<int>(rows[0]);
		return 0;
	});
	assert(current_value != std::numeric_limits<int>::min());
	// Run migrations starting from the actual value
	int i = 0;
	for (migration_function_type * it = migrations_; *it != nullptr; ++it, ++i)
	{
		DBUG("i=%d current_value=%d", i, current_value);
		if (current_value < i)
		{
			DBUG("migrate: %04d", i);
			(this->*(*it))();
			execute_query("INSERT INTO history (value) VALUES (%d)", NOOP, i);
			continue;
		}
		DBUG("already migrated: %04d", i);
	}
}

void database::migration_0001_create_schema()
{
	execute_query("CREATE TABLE users (uin INTEGER NOT NULL PRIMARY KEY, password NOT NULL)", NOOP);
}