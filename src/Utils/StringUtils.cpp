#include <string>
#include "StringUtils.h"       

namespace MiniDb::Utils {

	std::string StringUtils::shortenText(const std::string& text, const int& textDisplayLimit) {
		if (text.length() > textDisplayLimit) {
			return text.substr(0, textDisplayLimit) + "..";
		}
		return text;
	}

}
