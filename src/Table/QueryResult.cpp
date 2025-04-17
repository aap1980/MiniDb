#include "QueryResult.h"
#include "../Constants.h"
#include "../Utils/StringUtils.h"
#include <iostream>
#include <iomanip>

namespace MiniDb::Table {

	void QueryResult::print() const {
		std::cout << "QueryResult" << "\n";

		for (const auto& column : columns.getColumns()) {
			std::cout << std::setw(TEXT_DISPLAY_LIMIT) << MiniDb::Utils::StringUtils::shortenText(column.name, TEXT_DISPLAY_LIMIT) << "  ";
		}
		std::cout << "\n";

		for (const auto& row : rows.getRows()) {
			for (const auto& value : row.getValues()) {
				std::cout << std::setw(TEXT_DISPLAY_LIMIT) << MiniDb::Utils::StringUtils::shortenText(value, TEXT_DISPLAY_LIMIT) << "  ";
			}
			std::cout << "\n";
		}
	}

}
