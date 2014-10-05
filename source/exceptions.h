#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace atomic
{
	class error
	{
		const char *message;
		const char *func;
		const char *file;
		int line;

	public:
		error(const char *message, const char *func, const char *file, int line) : message(message), func(func), file(file), line(line) {}
		~error() {}

		const char *type() const { return "atomic::error"; }
		const char *what() const { return message; }

		const std::string loc() const
		{
			std::ostringstream s;
			s << func;
			s << " (in ";
			s << file;
			s << ":";
			s << line;
			s << ")";
			const std::string str(s.str());
			return str;
			return str.c_str();
		}
	};
};

#define throw_err(type, arg) throw type(arg, __FUNCTION__, __FILE__, __LINE__);