#pragma once
#include <Domain/QME/InfoQmePerson.h>

namespace QME {

//**********************************************************************************

class CPersonGetAccessor : public CInfoQmePerson {
  DBSTATUS _FirstName_dbs, _MiddleName_dbs;

 public:
  DEFINE_COMMAND_EX(CPersonGetAccessor, L"EXEC GQA_GetAllPersons")

  BEGIN_COLUMN_MAP(CPersonGetAccessor)

  COLUMN_ENTRY(1, _PersonId)
  COLUMN_ENTRY_STATUS(2, _FirstName, _FirstName_dbs)
  COLUMN_ENTRY_STATUS(3, _MiddleName, _MiddleName_dbs)
  COLUMN_ENTRY(4, _LastName)

  END_COLUMN_MAP()

  void MapNullParams_From_DB() {
    CLEAR_NULLABLE_STR_PARAM(_FirstName, _FirstName_dbs);
    CLEAR_NULLABLE_STR_PARAM(_MiddleName, _MiddleName_dbs);
  };
};

//**********************************************************************************

class CPersonEditAccessor : public CInfoQmePerson {
 public:
  INT ReturnValue, Action;
  DBSTATUS _FirstName_dbs, _MiddleName_dbs;

  CPersonEditAccessor() : ReturnValue(0), Action(-1) {}

  DEFINE_COMMAND_EX(CPersonEditAccessor,
                    L"{ ? = CALL dbo.GQA_EditPersons(?, ?, ?, ?, ?) }");

  BEGIN_PARAM_MAP(CPersonEditAccessor)

  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, ReturnValue)

  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, Action)
  COLUMN_ENTRY_STATUS(3, _FirstName, _FirstName_dbs)
  COLUMN_ENTRY_STATUS(4, _MiddleName, _MiddleName_dbs)
  COLUMN_ENTRY(5, _LastName)
  COLUMN_ENTRY(6, _PersonId)

  END_PARAM_MAP()

  void MapNullParams_To_DB() {
    SET_DB_STATUS_CSTR(_FirstName, _FirstName_dbs);
    SET_DB_STATUS_CSTR(_MiddleName, _MiddleName_dbs);
  };
};
}  // namespace QME