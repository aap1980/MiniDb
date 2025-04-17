#include "QueryResult.h"
#include "../Constants.h"
#include "../Utils/StringUtils.h"
#include <iostream>
#include <iomanip>

namespace MiniDb::Table {

	void QueryResult::print() const {
		std::cout << "QueryResult\n";

		// Kolumny
		bool first = true;
		for (const auto& column : columns.getColumns()) {
			if (!first) std::cout << "  ";
			std::cout << std::setw(TEXT_DISPLAY_LIMIT)
				<< MiniDb::Utils::StringUtils::shortenText(column.name, TEXT_DISPLAY_LIMIT);
			first = false;
		}
		std::cout << "\n";

		// Wiersze
		for (const auto& row : rows.getRows()) {
			first = true;
			for (const auto& value : row.getValues()) {
				if (!first) std::cout << "  ";
				std::cout << std::setw(TEXT_DISPLAY_LIMIT)
					<< MiniDb::Utils::StringUtils::shortenText(value, TEXT_DISPLAY_LIMIT);
				first = false;
			}
			std::cout << "\n";
		}
	}

}
