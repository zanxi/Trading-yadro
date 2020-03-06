
#ifndef __DBA___CLASSES__H__INCLUDED__
#define __DBA___CLASSES__H__INCLUDED__

#pragma once

class CCLASS_INSAccessor {
 public:
  CCLASS_INSAccessor() { memset(this, 0, sizeof(*this)); }

  LONG ReturnValue;

  LONG ClassId;
  TCHAR ClassCode[13];
  LONG ContractsType;
  LONG CalendarId;
  LONG SettleCodeId;
  LONG CrossTradeCheckType;
  TCHAR ClassTags[901];
  TCHAR ExtraClassParams[901];
  LONG MarketId;
  LONG MatchingMode;
  LONG QuoteBasis;
  LONG Flags;

  DBSTATUS CalendarId_status, SettleCodeId_status, CrossTradeCheckType_status,
      MarketId_status;
  DBSTATUS MatchingMode_status, QuoteBasis_status;

  DEFINE_COMMAND_EX(
      CCLASS_INSAccessor,
      L"{ ? = CALL dbo.GQA_NewClass(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? ) }");

  BEGIN_PARAM_MAP(CCLASS_INSAccessor)

  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, ReturnValue)

  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, ClassCode)
  COLUMN_ENTRY(3, ClassTags)
  COLUMN_ENTRY_STATUS(4, CalendarId, CalendarId_status)
  COLUMN_ENTRY_STATUS(5, SettleCodeId, SettleCodeId_status)
  COLUMN_ENTRY(6, ContractsType)
  COLUMN_ENTRY_STATUS(7, CrossTradeCheckType, CrossTradeCheckType_status)
  COLUMN_ENTRY(8, ExtraClassParams)
  COLUMN_ENTRY(9, ClassId)
  COLUMN_ENTRY(10, Flags)
  COLUMN_ENTRY_STATUS(11, MarketId, MarketId_status)
  COLUMN_ENTRY_STATUS(12, MatchingMode, MatchingMode_status)
  COLUMN_ENTRY_STATUS(13, QuoteBasis, QuoteBasis_status)
  END_PARAM_MAP()
};

class CCLASS_DELAccessor {
 public:
  CCLASS_DELAccessor() { memset(this, 0, sizeof(*this)); }

  LONG m_RETURN_VALUE;

  LONG m_CLASS_ID;

  DEFINE_COMMAND_EX(CCLASS_DELAccessor, L"{ ? = CALL dbo.GQA_DelClass(?) }");

  BEGIN_PARAM_MAP(CCLASS_DELAccessor)
  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, m_RETURN_VALUE)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, m_CLASS_ID)
  END_PARAM_MAP()
};

#endif
