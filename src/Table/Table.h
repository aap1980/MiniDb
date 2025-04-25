#pragma once

#include <vector>
#include <string>
#include "Rows.h"
#include "Columns.h"

namespace MiniDb::Table {

	class Table {
	private:
		std::string _tableName;
		std::string _dataFile;
		std::string _metadataFile;

	public:
		Columns columns;
		Rows rows;
		Table(const std::string& tableName);
		void loadMetadataFromFile();
		void saveMetadataToFile();
		void loadDataFromFile();
		void saveDataToFile();
	};

}
