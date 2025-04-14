#ifndef SQLPARSER_IMPORT_STATEMENT_H
#define SQLPARSER_IMPORT_STATEMENT_H

#include "ImportExportOptions.h"
#include "SQLStatement.h"

namespace hsql {

// Represents SQL Import statements.
struct ImportStatement : SQLStatement {
  ImportStatement(ImportType type);
  ~ImportStatement() override;

  ImportType type;
  char* filePath;
  char* schema;
  char* _tableName;
  Expr* whereClause;
  char* encoding;
};

}  // namespace hsql

#endif
