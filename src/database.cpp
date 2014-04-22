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
	create_functions();
	migrate();
}

bool database::is_open() const
{
	return db_ != nullptr;
}

void database::create_functions()
{
	assert(db_ != nullptr);
	int result = ::sqlite3_create_function(
		db_,
		"gg_login_hash_sha1",
		2,
		SQLITE_UTF8 | SQLITE_DETERMINISTIC,
		this,
		[](sqlite3_context* ctx, int argc, sqlite3_value** argv)
		{
			assert(ctx);
			database * self = reinterpret_cast<database *>(::sqlite3_user_data(ctx));
			assert(self);
			self->func_gg_login_hash_sha1(ctx, argc, argv);
		},
		nullptr,
		nullptr);
	assert(result == 0);
	result = ::sqlite3_create_function(
		db_,
		"gg_login_hash",
		2,
		SQLITE_UTF8 | SQLITE_DETERMINISTIC,
		this,
		[](sqlite3_context* ctx, int argc, sqlite3_value** argv)
		{
			assert(ctx);
			database * self = reinterpret_cast<database *>(::sqlite3_user_data(ctx));
			assert(self);
			self->func_gg_login_hash(ctx, argc, argv);
		},
		nullptr,
		nullptr);
	assert(result == 0);
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

void database::func_gg_login_hash_sha1(sqlite3_context* ctx, int argc, sqlite3_value** argv)
{
	assert(argc == 2);
	const char * password =
		reinterpret_cast<const char *>(::sqlite3_value_text(argv[0]));
	assert(password);
	int seed = ::sqlite3_value_int(argv[1]);
	DBUG("password: %s", password);
	DBUG("seed: %d", seed);
	void * result = ::sqlite3_malloc(20);
	::gg_login_hash_sha1(password, seed, static_cast<uint8_t *>(result));
	::sqlite3_result_blob(ctx, result, 20, &sqlite3_free);
}

void database::func_gg_login_hash(sqlite3_context* ctx, int argc, sqlite3_value** argv)
{
	assert(argc == 2);
	const unsigned char * password = ::sqlite3_value_text(argv[0]);
	assert(password);
	int seed = ::sqlite3_value_int(argv[1]);
	unsigned int result = ::gg_login_hash(password, seed);
	DBUG("gg_login_hash=%d", result);
	::sqlite3_result_int(ctx, result);
}