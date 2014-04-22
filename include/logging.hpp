#if !defined(ESG_LOGGING_HPP_INCLUDED_)
#define ESG_LOGGING_HPP_INCLUDED_

#define LOG_MESSAGE_AUX(level, format, ...) \
	do \
	{ \
		std::fprintf(stdout, "[%d] %s: " format "\n", ::getpid(), level, ## __VA_ARGS__); \
	} while (0)

#define DBUG(format, ...) LOG_MESSAGE_AUX("debug", format, ## __VA_ARGS__)
#define INFO(format, ...) LOG_MESSAGE_AUX("info", format, ## __VA_ARGS__)
#define WARN(format, ...) LOG_MESSAGE_AUX("warn", format, ## __VA_ARGS__)
#define ERR(format, ...) LOG_MESSAGE_AUX("error", format, ## __VA_ARGS__)
#define INFO(format, ...) LOG_MESSAGE_AUX("info", format, ## __VA_ARGS__)

#endif