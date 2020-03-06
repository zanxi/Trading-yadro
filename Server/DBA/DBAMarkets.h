#pragma once
#include <Domain/QME/InfoQmeMarkets.h>

namespace QME {

class CBasisMarketsParams : public CInfoQmeMarkets {
 public:
  CBasisMarketsParams() { memset(this, 0, sizeof(*this)); }

  INT MarketId;
  TCHAR MarketCode[5];
  TCHAR Description[129];
};

class CMarketsGetAccessor : public CInfoQmeMarkets {
 public:
  DEFINE_COMMAND_EX(CMarketsGetAccessor, L"EXEC dbo.GQA_GetAllMarkets")

  BEGIN_COLUMN_MAP(CMarketsGetAccessor)
  COLUMN_ENTRY(1, MarketId)
  COLUMN_ENTRY(2, MarketCode)
  COLUMN_ENTRY(3, Description)
  END_COLUMN_MAP()

  void MapNullParams_From_DB(){};
};

class CMarketsEditAccessor : public CInfoQmeMarkets {
 public:
  INT Action, ReturnValue;
  ;

  DEFINE_COMMAND_EX(CMarketsEditAccessor,
                    L"{ ? = CALL dbo.GQA_EditMarkets(?, ?, ?, ?) }")

  BEGIN_PARAM_MAP(CMarketsEditAccessor)
  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, ReturnValue)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, Action)
  COLUMN_ENTRY(3, MarketCode)
  COLUMN_ENTRY(4, Description)
  COLUMN_ENTRY(5, MarketId)
  END_PARAM_MAP()

  void MapNullParams_To_DB(){};
};
}  // namespace QME