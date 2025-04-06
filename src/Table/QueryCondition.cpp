#include <unordered_map>
#include <string>
#include "QueryCondition.h"

namespace MiniDb::Table {

	void QueryCondition::addCondition(const std::string& column, const std::string& value) {
		conditions[column] = value;
	}

	const std::unordered_map<std::string, std::string>& QueryCondition::getConditions() const {
		return conditions;
	}

}