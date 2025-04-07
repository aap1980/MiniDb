#pragma once
#include <string>
#include <sstream>

namespace MiniDb::Console {

	class Console {
	private:
		void parseCommand(const std::string& command);
		void handleSelect(std::istringstream& iss);
		void handleInsert(std::istringstream& iss);
		void handleUpdate(std::istringstream& iss);
		void handleDelete(std::istringstream& iss);
	public:
		void run();
	};

}