#include <stdexcept>
#include <string>

namespace MiniDb {

	class FileWriteException : public std::runtime_error {
	public:
		explicit FileWriteException(const std::string& message)
			: std::runtime_error(message) {}
	};

	class DatabaseException : public std::runtime_error {
	public:
		explicit DatabaseException(const std::string& message)
			: std::runtime_error(message) {}
	};

}