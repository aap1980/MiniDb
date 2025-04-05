#pragma once
#include <string>
#include <vector>

namespace MiniDb::Metadata {

	class ColumnDef {
	public:
		std::string name;
		std::string type;

		ColumnDef(const std::string& name, const std::string& type);
	};

	class TableMetadataWriter {
	private:
		std::string tableName;
		std::vector<ColumnDef> columns;
		constexpr static char SEP = 0x1F;

	public:
		TableMetadataWriter(const std::string& name);
		void addColumn(const std::string& name, const std::string& type);
		bool saveToFile(const std::string& filename) const;
	};

}
