#pragma once
#include <vector>
#include <string>
#include "../Table/Table.h"

namespace MiniDb::Metadata {

	class TableMetadataReader {
	public:
		explicit TableMetadataReader(const std::string& tableName);

		bool loadFromFile(const std::string& filename, MiniDb::Table::Table& table);
	private:
		std::string tableName;
		constexpr static char SEP = 0x1F;
	};

}
