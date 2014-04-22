template <typename Callback, typename... Args>
void database::execute_query(const std::string & query, Callback callback, Args... args)
{
	auto sqlite_callback = [](void * data, int argc, char** cols, char** rows) -> int
	{
		return (*reinterpret_cast<Callback *>(data))(argc, cols, rows);
	};
	char * q = ::sqlite3_mprintf(query.c_str(), args...);
	DBUG("Q: %s", q);
	char * err = nullptr;
	int result = ::sqlite3_exec(
		db_,
		q,
		sqlite_callback,
		&callback,
		&err
	);
	::sqlite3_free(q);
	if (err)
	{
		ERR("SQL error: %s", err);
		::sqlite3_free(err);
	}
}