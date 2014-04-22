#include "gg_connection.hpp"

gg_connection::gg_connection(boost::asio::io_service & io_service)
	: io_service_(io_service)
	, database_(boost::asio::use_service<database>(io_service_))
	, socket_(io_service_)
{
}

void gg_connection::start()
{
	INFO("New connection");
	// Send GG_WELCOME to client and wait for packets AFTER seed was delivered.
	send_gg_welcome();
}

void gg_connection::begin_read_gg_header()
{
	// Begin asynchronous operation
	boost::asio::async_read(socket_,
		boost::asio::buffer(&gg_header_, sizeof(struct gg_header)),
		boost::bind(&gg_connection::handle_read_gg_header, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void gg_connection::handle_read_gg_header(const boost::system::error_code & error,
	std::size_t bytes_transferred)
{
	if (!error)
	{
		gg_event_.resize(gg_header_.length);
		boost::asio::async_read(socket_, boost::asio::buffer(gg_event_),
			boost::bind(&gg_connection::handle_read_gg_event, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
}

void gg_connection::handle_read_gg_event(const boost::system::error_code & error,
	std::size_t bytes_transferred)
{
	if (!error)
	{
		DBUG("Received gg event: %d", gg_header_.type);
		switch (gg_header_.type)
		{
		case GG_LOGIN80:
			struct gg_login80 * event =
				reinterpret_cast<struct gg_login80 *>(gg_event_.data());
			handle_gg_login80(event);
			break;
		}
		begin_read_gg_header();
	}
}

void gg_connection::send_gg_welcome()
{
	// Chosen by fair dice roll. Guarenteed to be random.
	seed_ = 69;

	boost::asio::streambuf::mutable_buffers_type bufs =
		write_buffer_.prepare(sizeof(struct gg_header) + sizeof(struct gg_welcome));
	
	struct gg_header * header = boost::asio::buffer_cast<struct gg_header *>(bufs);
	header->type = GG_WELCOME;
	header->length = sizeof(struct gg_welcome);

	struct gg_welcome * welcome = boost::asio::buffer_cast<struct gg_welcome *>(bufs + sizeof(struct gg_header));
	welcome->key = seed_;
		
	boost::asio::async_write(socket_, bufs,
		boost::bind(&gg_connection::handle_write_gg_welcome, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void gg_connection::handle_write_gg_welcome(const boost::system::error_code & error,
	std::size_t bytes_transferred)
{
	if (!error)
	{
		write_buffer_.consume(bytes_transferred);
		begin_read_gg_header();
	}
}

void gg_connection::handle_gg_login80(struct gg_login80 * event)
{
	bool found;
	std::string function_name;
	switch (event->hash_type)
	{
	case GG_LOGIN_HASH_GG32:
	{
		// XXX: This is my guess how to handle old hashing method
		unsigned int * hash = reinterpret_cast<unsigned int *>(event->hash);
		database_.execute_query(
			"SELECT EXISTS("
				"SELECT 1 "
				"FROM users "
				"WHERE uin = %d "
				"AND gg_login_hash(password, %d) = %d)",
			[&](int argc, char ** rows, char ** cols) -> int
			{
				assert(argc == 1);
				found = boost::lexical_cast<bool>(rows[0]);
				return 0;
			}, event->uin, seed_, *hash);
		break;
	}
	case GG_LOGIN_HASH_SHA1:
		database_.execute_query(
			"SELECT uin, gg_login_hash_sha1(password, %d) FROM users WHERE uin = %d",
			[&](int argc, char ** rows, char ** cols) -> int
			{
				assert(boost::lexical_cast<int>(rows[0]) == event->uin);
				found = std::memcmp(rows[1], event->hash, 20) == 0;
				return 0;
			}, seed_, event->uin);
		break;
	default:
		ERR("Unknown login hash method: 0x%04X", event->hash_type);
		return;
	}
	if (!found)
	{
		ERR("Unable to find user for uin: %d (Seed: %d)", event->uin, seed_);
		return;
	}
	INFO("User logged in: %d (Seed: %d)", event->uin, seed_);
}