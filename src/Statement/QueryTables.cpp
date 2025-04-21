#include "QueryTables.h"

namespace MiniDb::Statement {

	std::size_t QueryTables::size() const {
		return tables.size();
	}

	void QueryTables::addTable(const MiniDb::Statement::QueryTable& table) {
		for (const auto& existing : tables) {
			if (existing.tableAlias == table.tableAlias) {
				throw std::logic_error("Alias already exists: " + table.tableAlias);
			}
		}
		tables.push_back(table);
	}

	bool QueryTables::hasAlias(const std::string& tableAlias) const {
		for (const auto& qtInfo : tables) {
			if (qtInfo.tableAlias == tableAlias) {
				return true;
			}
		}
		return false;
	}

	MiniDb::Statement::QueryTable& QueryTables::getByIndex(std::size_t index) {
		if (index >= tables.size()) {
			throw std::out_of_range("Index out of range: " + index);
		}
		return tables[index];
	}

	MiniDb::Statement::QueryTable& QueryTables::getByAlias(const std::string& tableAlias) {
		for (auto& info : tables) {
			if (info.tableAlias == tableAlias) {
				return info;
			}
		}
		throw std::out_of_range("Alias not found: " + tableAlias);
	}

	MiniDb::Statement::QueryTable& QueryTables::last() {
		return tables.back();
	}

}
