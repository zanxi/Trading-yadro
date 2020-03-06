// The following ifdef block is the standard way of creating macros which make
// exporting from a DLL simpler. All files within this DLL are compiled with the
// BOQME_EXPORTS symbol defined on the command line. This symbol should not be
// defined on any project that uses this DLL. This way any other project whose
// source files include this file see BOQME_API functions as being imported from
// a DLL, whereas this DLL sees symbols defined with this macro as being
// exported.

#include "QmePlugin.h"

#ifdef BOQME_EXPORTS
#define BOQME_API extern "C" __declspec(dllexport)
#else
#define BOQME_API extern "C" __declspec(dllimport)
#endif

ModuleRouter* g_pMODULES = nullptr;

BOQME_API void ModuleType(std::string& strModuleType);

BOQME_API BOOL ModuleInit(std::string& strModuleType);

BOQME_API BOOL ModuleUnInit();

BOQME_API BOOL InitDB(const std::string& strModuleDBName,
                      unsigned short port,
                      const std::string& strIniFile);

BOQME_API BOOL Execute(const std::string& strBaseName,
                       unsigned short nPort,
                       const std::string& func,
                       CAlert& alert,
                       int user_id);

BOQME_API BOOL Reset(int user_id);

BOQME_API void GetUnique(const std::string& strBaseName,
                         unsigned short nPort,
                         std::string& strDbLocalIp,
                         std::string& strDbBrokerGuid,
                         int& nDbId);

BOQME_API bool CheckUnique(const std::string& strBaseName,
                           unsigned short nPort,
                           const std::string& strDbLocalIp,
                           const std::string& strDbBrokerGuid,
                           int nDbId);
