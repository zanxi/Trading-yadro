
#ifndef __DBA___CLIENTS__H__INCLUDED__
#define __DBA___CLIENTS__H__INCLUDED__

#pragma once

class CCLIENT_INSAccessor {
 public:
  CCLIENT_INSAccessor() { memset(this, 0, sizeof(*this)); }

  LONG RetuenValue;

  LONG ClientId;
  LONG FirmId;
  TCHAR ClientCode[13];
  LONG ClientType;
  LONG PersonId;

  DBSTATUS PersonId_dbs;

  DEFINE_COMMAND_EX(CCLIENT_INSAccessor,
                    L"{ ? = CALL dbo.GQA_NewClient(?, ?, ?, ?, ?) }");

  BEGIN_PARAM_MAP(CCLIENT_INSAccessor)

  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, RetuenValue)

  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, ClientId)
  COLUMN_ENTRY(3, FirmId)
  COLUMN_ENTRY(4, ClientCode)
  COLUMN_ENTRY(5, ClientType)
  COLUMN_ENTRY_STATUS(6, PersonId, PersonId_dbs)

  END_PARAM_MAP()
};

class CCLIENT_DELAccessor {
 public:
  CCLIENT_DELAccessor() { memset(this, 0, sizeof(*this)); }

  LONG ReturnValue;

  LONG ClientId;

  DEFINE_COMMAND_EX(CCLIENT_DELAccessor, L"{ ? = CALL dbo.GQA_DelClient(?) }");

  BEGIN_PARAM_MAP(CCLIENT_DELAccessor)
  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, ReturnValue)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, ClientId)
  END_PARAM_MAP()
};

#endif
