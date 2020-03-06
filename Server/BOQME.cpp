// BOQME.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "BOQME.h"

BOQME_API void ModuleType(std::string& strModuleType) { strModuleType = "QME"; }

BOQME_API BOOL ModuleInit(std::string& strModuleType) {
  try {
    g_pMODULES = new ModuleRouter;
    ModuleType(strModuleType);
    return TRUE;
  } catch (std::exception& e) {
    devent(LL_CRITICAL, "Can't initialize module for QME. Exception [%s]",
           e.what());
    return FALSE;
  }
}

BOQME_API BOOL ModuleUnInit() {
  if (g_pMODULES == nullptr)
    return TRUE;

  delete g_pMODULES;
  g_pMODULES = nullptr;

  return TRUE;
}

BOQME_API BOOL InitDB(const std::string& strModuleDBName,
                      unsigned short port,
                      const std::string& strIniFile) {
  if (g_pMODULES == nullptr) {
    devent(LL_CRITICAL, "ERROR: QME Server Plugin not instantiated.");
    return FALSE;
  }

  try {
    QmePlugin* pQME =
        static_cast<QmePlugin*>(g_pMODULES->AddModule(port, strModuleDBName));
    if (pQME)
      return pQME->Init(strModuleDBName, strIniFile, port);
    else
      return FALSE;
  } catch (std::exception& e) {
    devent(LL_CRITICAL, "Can't create module for QME. Exception [%s]",
           e.what());
    return FALSE;
  }
}

GateSrvPlugin* GetModule(int nPort, const std::string& strBaseName) {
  if (g_pMODULES == nullptr)
    return nullptr;

  return g_pMODULES->GetModule(nPort, strBaseName);
}

BOQME_API BOOL Execute(const std::string& strBaseName,
                       unsigned short nPort,
                       const std::string& func,
                       CAlert& alert,
                       int user_id) {
  QmePlugin* pQME = static_cast<QmePlugin*>(GetModule(nPort, strBaseName));
  if (pQME == nullptr)
    return FALSE;

  if (Library::Utils::StringNoCaseEq(func, PCORE::GQA_FUNCTION_INIT)) {
    return pQME->InitModule(alert, user_id);
  }

  if (Library::Utils::StringNoCaseEq(func, PCORE::GQA_FUNCTION_CHECK_BASE)) {
    alert.clear();
    int res = 1;
    alert << res;
    alert << std::string("1");
    alert << res;
    alert << std::string("");  // error
    return TRUE;
  }

  if (Library::Utils::StringNoCaseEq(func, PCORE::GQA_FUNCTION_GET_TABLE)) {
    // find module by name and type
    // send command
    return pQME->GetTable(alert);
  }

  if (Library::Utils::StringNoCaseEq(func, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE)
		|| Library::Utils::StringNoCaseEq(func, PCORE::GQA_FUNCTION_UPDATE_TABLE)
		/*|| Library::Utils::StringNoCaseEq(func, GQA_FUNCTION_REPLACE_TABLE) */
		|| Library::Utils::StringNoCaseEq(func, PCORE::GQA_FUNCTION_DELETE_FROM_TABLE)
		/*|| Library::Utils::StringNoCaseEq(func, GQA_FUNCTION_COMAND_ON_TABLE)*/ )
	{
    return pQME->UpdateTable(alert, func);
  }

  return FALSE;
}

BOQME_API BOOL Reset(int user_id) { return FALSE; }

BOQME_API void GetUnique(const std::string& strBaseName,
                         unsigned short nPort,
                         std::string& strDbLocalIp,
                         std::string& strDbBrokerGuid,
                         int& nDbId) {
  QmePlugin* pQME = static_cast<QmePlugin*>(GetModule(nPort, strBaseName));
  if (pQME == nullptr)
    return;

  pQME->GetUnique(strDbLocalIp, strDbBrokerGuid, nDbId);
}

BOQME_API bool CheckUnique(const std::string& strBaseName,
                           unsigned short nPort,
                           const std::string& strDbLocalIp,
                           const std::string& strDbBrokerGuid,
                           int nDbId) {
  QmePlugin* pQME = static_cast<QmePlugin*>(GetModule(nPort, strBaseName));
  if (pQME == nullptr)
    return false;

  return pQME->CheckUnique(strDbLocalIp, strDbBrokerGuid, nDbId);
}
