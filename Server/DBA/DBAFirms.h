#ifndef __DBA___FIRMS__H__INCLUDED__
#define __DBA___FIRMS__H__INCLUDED__

#pragma once

class CFIRM_INSAccessor {
 public:
  CFIRM_INSAccessor() { memset(this, 0, sizeof(*this)); }

  LONG m_RETURN_VALUE;

  TCHAR m_FIRM_CODE[13];
  TCHAR m_FIRM_NAME[31];
  TCHAR m_FIRM_CLASSES[900];
  TCHAR m_FIRM_EXCHANGE[13];
  LONG m_PERMISSIONS;
  LONG m_FIRM_ID;

  DEFINE_COMMAND_EX(CFIRM_INSAccessor,
                    L"{ ? = CALL dbo.GQA_NewFirm(?, ?, ?, ?, ?, ?) }");

  BEGIN_PARAM_MAP(CFIRM_INSAccessor)
  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, m_RETURN_VALUE)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, m_FIRM_CODE)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(3, m_FIRM_NAME)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(4, m_PERMISSIONS)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(5, m_FIRM_EXCHANGE)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(6, m_FIRM_CLASSES)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(7, m_FIRM_ID)
  END_PARAM_MAP()
};

class CFIRM_DELAccessor {
 public:
  CFIRM_DELAccessor() { memset(this, 0, sizeof(*this)); }

  LONG m_RETURN_VALUE;

  LONG m_FIRM_ID;

  DEFINE_COMMAND_EX(CFIRM_DELAccessor, L"{ ? = CALL dbo.GQA_DelFirm(?) }");

  BEGIN_PARAM_MAP(CFIRM_DELAccessor)
  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, m_RETURN_VALUE)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, m_FIRM_ID)
  END_PARAM_MAP()
};

class CAccToClass_Accessor {
 public:
  CAccToClass_Accessor() { memset(this, 0, sizeof(*this)); }

  LONG m_RETURN_VALUE;

  LONG m_ACC_ID;
  TCHAR m_ACC_CLASSES[900];

  DEFINE_COMMAND_EX(CAccToClass_Accessor,
                    L"{ ? = CALL dbo.GQA_SetClassesToAccount(?, ?) }");

  BEGIN_PARAM_MAP(CAccToClass_Accessor)

  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, m_RETURN_VALUE)

  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, m_ACC_ID)
  COLUMN_ENTRY(3, m_ACC_CLASSES)

  END_PARAM_MAP()
};

#endif
