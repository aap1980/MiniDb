#include "QueryTablesOrder.h"

namespace MiniDb::Statement {

	std::size_t QueryTablesOrder::size() const {
		return queryTableInfos.size();
	}

	void QueryTablesOrder::addQueryTableInfo(const MiniDb::Statement::QueryTableInfo& queryTableInfo) {
		for (const auto& existing : queryTableInfos) {
			if (existing.alias == queryTableInfo.alias) {
				throw std::logic_error("Alias already exists: " + queryTableInfo.alias);
			}
		}
		queryTableInfos.push_back(queryTableInfo);
	}

	bool MiniDb::Statement::QueryTablesOrder::hasAlias(const std::string& alias) const {
		for (const auto& qtInfo : queryTableInfos) {
			if (qtInfo.alias == alias) {
				return true;
			}
		}
		return false;
	}

	MiniDb::Statement::QueryTableInfo& MiniDb::Statement::QueryTablesOrder::getByIndex(std::size_t index) {
		if (index >= queryTableInfos.size()) {
			throw std::out_of_range("Index out of range: " + index);
		}
		return queryTableInfos[index];
	}

	MiniDb::Statement::QueryTableInfo& MiniDb::Statement::QueryTablesOrder::getByAlias(const std::string& alias) {
		for (auto& info : queryTableInfos) {
			if (info.alias == alias) {
				return info;
			}
		}
		throw std::out_of_range("Alias not found: " + alias);
	}

	MiniDb::Statement::QueryTableInfo& QueryTablesOrder::last() {
		return queryTableInfos.back();
	}

}
