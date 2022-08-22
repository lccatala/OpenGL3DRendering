#pragma once

#include <string>
#include <memory>

namespace PGUPV {
	class LogConsole {
	public:
		LogConsole();
		~LogConsole();
		void clear();
		void add(const std::string &msg);
		void render();
	private:
		struct LogImpl;
		std::unique_ptr<LogImpl> impl;
	};
};