#pragma once
#include <Domain/QME/InfoQmeExtraClassParam.h>

namespace QME {

//**********************************************************************************

class CExtraClassParamParams : public CInfoQmeExtraClassParam {
 public:
  CExtraClassParamParams() {}

  DBSTATUS _IsLimited_dbs, _ShortNameRus_dbs, _ShortNameEng_dbs,
      _FullNameRus_dbs, _FullNameEng_dbs;
};

//**********************************************************************************

class CExtraClassParamGetAccessor : public CExtraClassParamParams {
 public:
  DEFINE_COMMAND_EX(CExtraClassParamGetAccessor,
                    L"EXEC GQA_GetAllExtraClassParams")

  BEGIN_COLUMN_MAP(CExtraClassParamGetAccessor)

  COLUMN_ENTRY(1, _ParamId)
  COLUMN_ENTRY(2, _DbName)
  COLUMN_ENTRY(3, _Type)
  COLUMN_ENTRY_STATUS(4, _IsLimited, _IsLimited_dbs)
  COLUMN_ENTRY_STATUS(5, _ShortNameRus, _ShortNameRus_dbs)
  COLUMN_ENTRY_STATUS(6, _ShortNameEng, _ShortNameEng_dbs)
  COLUMN_ENTRY_STATUS(7, _FullNameRus, _FullNameRus_dbs)
  COLUMN_ENTRY_STATUS(8, _FullNameEng, _FullNameEng_dbs)

  END_COLUMN_MAP()

  void MapNullParams_From_DB() {
    CLEAR_NULLABLE_INT_PARAM(_IsLimited, _IsLimited_dbs);
    CLEAR_NULLABLE_STR_PARAM(_ShortNameRus, _ShortNameRus_dbs);
    CLEAR_NULLABLE_STR_PARAM(_ShortNameEng, _ShortNameEng_dbs);
    CLEAR_NULLABLE_STR_PARAM(_FullNameRus, _FullNameRus_dbs);
    CLEAR_NULLABLE_STR_PARAM(_FullNameEng, _FullNameEng_dbs);
  };
};

//**********************************************************************************

class CExtraClassParamEditAccessor : public CExtraClassParamParams {
 public:
  INT ReturnValue, Action;

  CExtraClassParamEditAccessor() : ReturnValue(0), Action(-1) {}

  DEFINE_COMMAND_EX(
      CExtraClassParamEditAccessor,
      L"{ ? = CALL dbo.GQA_EditExtraClassParams(?, ?, ?, ?, ?, ?, ?, ?, ?) }");

  BEGIN_PARAM_MAP(CExtraClassParamEditAccessor)

  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, ReturnValue)

  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, Action)
  COLUMN_ENTRY(3, _DbName)
  COLUMN_ENTRY(4, _Type)
  COLUMN_ENTRY_STATUS(5, _IsLimited, _IsLimited_dbs)
  COLUMN_ENTRY_STATUS(6, _ShortNameRus, _ShortNameRus_dbs)
  COLUMN_ENTRY_STATUS(7, _ShortNameEng, _ShortNameEng_dbs)
  COLUMN_ENTRY_STATUS(8, _FullNameRus, _FullNameRus_dbs)
  COLUMN_ENTRY_STATUS(9, _FullNameEng, _FullNameEng_dbs)
  COLUMN_ENTRY(10, _ParamId)
  END_PARAM_MAP()

  void MapNullParams_To_DB() {
    SET_DB_STATUS(_IsLimited, _IsLimited_dbs);
    SET_DB_STATUS_CSTR(_ShortNameRus, _ShortNameRus_dbs);
    SET_DB_STATUS_CSTR(_ShortNameEng, _ShortNameEng_dbs);
    SET_DB_STATUS_CSTR(_FullNameRus, _FullNameRus_dbs);
    SET_DB_STATUS_CSTR(_FullNameEng, _FullNameEng_dbs);
  };
};
}  // namespace QME