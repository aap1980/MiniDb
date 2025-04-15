#pragma once

#include <string>

namespace MiniDb::Utils {

	class StringUtils {
	public:
		static std::string shortenText(const std::string& text, const int& textDisplayLimit);
	};

}
