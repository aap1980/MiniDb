#pragma once

#include "Statement.h"
#include "../Database/Database.h"
#include "../SqlParser/SQLParser.h"

namespace MiniDb::Statement {

	class SelectStatement : public Statement {
	public:
		SelectStatement(const hsql::SelectStatement* statement);
		void execute(MiniDb::Database::Database& database) const override;
	private:
		std::string _tableName;
		bool _selectAll;
	};

}
