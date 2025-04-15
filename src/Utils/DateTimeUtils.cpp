#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "DateTimeUtils.h"

namespace MiniDb::Utils {

	std::string DateTimeUtils::getCurrentDatetimeAsString() {
		auto now = std::chrono::system_clock::now();
		std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

		std::tm tm;
		localtime_s(&tm, &currentTime);

		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
		return oss.str();
	}

}
