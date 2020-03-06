#pragma once
#include "./Domain/QME/InfoQmeMap.h"
#include <Domain/QME/InfoQmeCurrency.h>

const LPCWSTR dbCommandGetCurrencies = L"EXEC GQA_GetAllCurrencies";
const LPCWSTR dbCommandEditCurrencies =
    L"{? = CALL dbo.GQA_EditCurrencies(?, ?, ?, ?, ?, ?) }";

namespace QME {
class CCurrencyGetAccessor : public CInfoQmeCurrency {
 public:
  DEFINE_COMMAND_EX(CCurrencyGetAccessor, dbCommandGetCurrencies)
  BEGIN_COLUMN_MAP(CCurrencyGetAccessor)
  COLUMN_ENTRY(1, _CurrencyId)
  COLUMN_ENTRY(2, _CurrencyCode)
  COLUMN_ENTRY(3, _FullName)
  COLUMN_ENTRY(4, _Scale)
  COLUMN_ENTRY_STATUS(5, _CalendarId, CalendarId_dbs)
  END_COLUMN_MAP()
  void MapNullParams_From_DB(){};
};

class CCurrencyEditAccessor : public CInfoQmeCurrency {
 public:
  INT ReturnValue, Action;
  CCurrencyEditAccessor() {}
  DEFINE_COMMAND_EX(CCurrencyEditAccessor, dbCommandEditCurrencies);
  BEGIN_PARAM_MAP(CCurrencyEditAccessor)
  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, ReturnValue)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, Action)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(3, _CurrencyCode)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(4, _FullName)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(5, _Scale)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(6, _CurrencyId)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY_STATUS(7, _CalendarId, CalendarId_dbs)
  END_PARAM_MAP()

  void MapNullParams_To_DB(){};
};
}  // namespace QME