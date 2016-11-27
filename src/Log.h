#ifndef LOG_H
#define LOG_H

#include <ostream>
#include <string>
#include <iostream>
#include <sstream>

namespace StaticMapping {

namespace Logging {

#ifndef NOINLINE_ATTRIBUTE
#ifdef __ICC
#define NOINLINE_ATTRIBUTE __attribute__((noinline))
#else
#define NOINLINE_ATTRIBUTE
#endif
#endif

struct None {};

template<typename List>
struct LogData {
	List list;
};

template<typename Begin, typename Value>
constexpr LogData<std::pair<Begin&&, Value&&>> operator<<(LogData<Begin>&& begin, 
                                                          Value&& value) noexcept
{
    return {{ std::forward<Begin>(begin.list), std::forward<Value>(value) }};
}

template<typename Begin, size_t n>
constexpr LogData<std::pair<Begin&&, const char*>> operator<<(LogData<Begin>&& begin, 
                                                              const char (&value)[n]) noexcept
{
    return {{ std::forward<Begin>(begin.list), value }};
}

typedef std::ostream& (*PfnManipulator)(std::ostream&);

template<typename Begin>
constexpr LogData<std::pair<Begin&&, PfnManipulator>> operator<<(LogData<Begin>&& begin, 
                                                                 PfnManipulator value) noexcept
{
	return {{ std::forward<Begin>(begin.list), value }};
}

template <typename Begin, typename Last>
void output(std::ostream& os, std::pair<Begin, Last>&& data)
{
    output(os, std::move(data.first));
    os << data.second;
}

inline void output(std::ostream& os, None)
{ }

template<typename List>
void log(const char* file, int line, LogData<List>&& data) NOINLINE_ATTRIBUTE
{
	std::cout << file << ":" << line << ": ";
	output(std::cout, std::move(data.list));
	std::cout << std::endl;
}

} // namespace Logging

#ifndef NOLOG
#define NOLOG 1
#endif

#if NOLOG
#define LOG(...) do{}while(false)
#else
#define LOG(msg) (Logging::log(__FILE__, __LINE__, Logging::LogData<Logging::None>() << msg))
#endif

} // namespace StaticMapping

#endif
