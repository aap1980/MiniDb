#pragma once

#include <string>
#include <sstream>

namespace MiniDb::Console {

	class Console {
	private:
		void parseCommand(const std::string& command);
	public:
		void run();
	};

}