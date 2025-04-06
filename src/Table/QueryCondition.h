#pragma once
#include <unordered_map>
#include <string>

namespace MiniDb::Table {

	class QueryCondition {
	private:
		std::unordered_map<std::string, std::string> conditions;
	public:
		void addCondition(const std::string& column, const std::string& value);
		const std::unordered_map<std::string, std::string>& getConditions() const;
	};

}