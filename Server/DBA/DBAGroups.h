
#ifndef __DBA___CORE_GROUPS__H__INCLUDED__
#define __DBA___CORE_GROUPS__H__INCLUDED__

#pragma once

class CGROUP_INSAccessor {
 public:
  CGROUP_INSAccessor() { memset(this, 0, sizeof(*this)); }

  LONG m_RETURN_VALUE;

  TCHAR m_GROUP_NAME[256];

  DEFINE_COMMAND_EX(CGROUP_INSAccessor,
                    L"{ ? = CALL dbo.GQA_NewCoreGroup(?) }");

  BEGIN_PARAM_MAP(CGROUP_INSAccessor)
  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, m_RETURN_VALUE)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, m_GROUP_NAME)
  END_PARAM_MAP()
};

class CGROUP_DELAccessor {
 public:
  CGROUP_DELAccessor() { memset(this, 0, sizeof(*this)); }

  LONG m_RETURN_VALUE;

  LONG m_GROUP_ID;

  DEFINE_COMMAND_EX(CGROUP_DELAccessor,
                    L"{ ? = CALL dbo.GQA_DelCoreGroup(?) }");

  BEGIN_PARAM_MAP(CGROUP_DELAccessor)
  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, m_RETURN_VALUE)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, m_GROUP_ID)
  END_PARAM_MAP()
};

#endif
