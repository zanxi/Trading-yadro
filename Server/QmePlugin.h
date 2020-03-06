#pragma once

#include <memory>
#include <string>

#include <GateSrvPlugin.h>
#include <PluginSrvDefs.h>

#include <Domain\QME\GQA_FunctionsDef.h>

class QmePlugin : public GateSrvPlugin {
  typedef std::map<int, std::string> ClassDescPathMap;
  ClassDescPathMap m_mSDPPath;
  ClassDescPathMap m_mTransPath;

  BOOL CheckIniError();

  BOOL GetClassesBySection(const std::string& strClassName,
                           std::string strSection);
  BOOL SetClassesBySection(bool bSet,
                           int nContractType,
                           const std::string& strClassName,
                           std::string strSection,
                           ClassDescPathMap& mPathes);

  void CreateDbError(CAlert& aErr,
                     const std::string& strFunc,
                     const std::string& strTable,
                     const std::string& strError,
                     int err);

 protected:
  virtual BOOL CheckDataProviders(unsigned short) override;
  void InitClassesDefaults(Library::Ini::CIniFile& iniLocal) override;

  BOOL GetClasses(CAlert& a, BOOL bForced, const std::string& strDbUser);
  BOOL GetAccounts(CAlert& a, BOOL bForced, const std::string& strDbUser);
  BOOL GetCalendars(CAlert& a, BOOL bForced, const std::string& strDbUser);
  BOOL GetCalendarDates(CAlert& a, BOOL bForced, const std::string& strDbUser);
  BOOL GetClients(CAlert& a, BOOL bForced, const std::string& strDbUser);
  BOOL GetFirms(CAlert& a, BOOL bForced, const std::string& strDbUser);
  BOOL GetSecuririties(CAlert& a, BOOL bForced, const std::string& strDbUser);
  BOOL GetTags(CAlert& a, BOOL bForced, const std::string& strDbUser);
  BOOL GetGroups(CAlert& a, BOOL bForced, const std::string& strDbUser);
  BOOL GetSettleCodes(CAlert& a, BOOL bForced, const std::string& strDbUser);
  BOOL GetSchedule(CAlert& a, BOOL bForced, const std::string& strDbUser);
  BOOL GetTradeParam(CAlert& a, BOOL bForced, const std::string& strDbUser);
  BOOL GetAuctions(CAlert& a, BOOL bForced, const std::string& strDbUser);
  BOOL GetConnections(CAlert& a);
  BOOL GetProperties(CAlert& a);

  BOOL GetIniSDPClass(const std::string& strClassName);
  BOOL GetIniTransClass(const std::string& strClassName);
  BOOL SetIniSDPClass(bool bSet,
                      int nContractType,
                      const std::string& strClassName);
  BOOL SetIniTransClass(bool bSet,
                        int nContractType,
                        const std::string& strClassName);

  BOOL InsertAndUpdateTable(CAlert& a,
                            const std::string& strTableName,
                            QME::CrudActions act);
  BOOL IAUClasses(CAlert& aRow, const std::string& strDbUser);
  INT GetFlags(void* info);
  BOOL IAUFirms(CAlert& aRow, const std::string& strDbUser);
  BOOL IAUSecurities(CAlert& aRow, const std::string& strDbUser);
  BOOL IAUAccounts(CAlert& aRow, const std::string& strDbUser);
  BOOL IAUTags(CAlert& aRow, const std::string& strDbUser);
  BOOL IAUGroups(CAlert& aRow, const std::string& strDbUser);
  BOOL IAUSettleCodes(CAlert& aRow, const std::string& strDbUser);
  BOOL IAUSchedule(CAlert& aRow, const std::string& strDbUser);
  BOOL UpdateSchedule(CAlert& keyRow,
                      CAlert& newRow,
                      const std::string& strDbUser);
  BOOL IAUCalendars(CAlert& aRow,
                    const std::string& strDbUser,
                    QME::CrudActions act);
  void IAUCalendarDates(CAlert aCDates,
                        int ReturnValue,
                        CAlert& aRow,
                        const std::string& strDbUser);

  BOOL IAUClients(CAlert& aRow, const std::string& strDbUser);
  BOOL IAUTradeParams(CAlert& aRow, const std::string& strDbUser);
  BOOL IAUAuctions(CAlert& aRow, const std::string& strDbUser);
  BOOL IAUAuctions_del(CAlert& aRow, const std::string& strDbUser);
  BOOL IAUConnection(CAlert& aRow);
  BOOL IAUProperty(CAlert& aRow);

  BOOL DeleteClass(CAlert& aRow, const std::string& strDbUser);
  BOOL DeleteFirm(CAlert& aRow, const std::string& strDbUser);
  BOOL DeleteSecurity(CAlert& aRow, const std::string& strDbUser);
  BOOL DeleteAccount(CAlert& aRow, const std::string& strDbUser);
  BOOL DeleteTag(CAlert& aRow, const std::string& strDbUser);
  BOOL DeleteGroup(CAlert& aRow, const std::string& strDbUser);
  BOOL DeleteSettleCode(CAlert& aRow, const std::string& strDbUser);
  BOOL DeleteSchedule(CAlert& aRow, const std::string& strDbUser);
  BOOL DeleteCalendar(CAlert& aRow, const std::string& strDbUser);
  BOOL DeleteClient(CAlert& aRow, const std::string& strDbUser);
  BOOL DeleteTradeParam(CAlert& aRow, const std::string& strDbUser);
  BOOL DeleteAuction(CAlert& aRow, const std::string& strDbUser);
  BOOL DeleteConnection(CAlert& aRow);
  BOOL DeleteProperty(CAlert& aRow);

  template <typename T_GET_ACCESSOR>
  BOOL GetAllObjects(CAlert& a, BOOL bForced, const std::string& strDbUser);
  template <typename T_GET_ACCESSOR>
  BOOL EditObject(CAlert& aRow,
                  const std::string& strDbUser,
                  QME::CrudActions act);

 public:
  QmePlugin();

  virtual BOOL GetTableImpl(CAlert& aGetTable,
                            const std::string& strTableName,
                            BOOL bForced,
                            const std::string& strFilter,
                            CAlert& alFilter) override;
  virtual BOOL InsertIntoTableImpl(CAlert& aKeyRow,
                                   CAlert& aNewRow,
                                   const std::string& strTableName) override;
  virtual BOOL UpdateTableImpl(CAlert& aKeyRow,
                               CAlert& aNewRow,
                               const std::string& strTableName) override;
  virtual BOOL DeleteFromTableImpl(CAlert& aDummyRow,
                                   CAlert& aKeyRow,
                                   const std::string& strTableName) override;
};

class ModuleRouter {
  std::map<std::pair<int, std::string>, std::unique_ptr<GateSrvPlugin>>
      m_mapModules;

 public:
  GateSrvPlugin* AddModule(int nPort, std::string strBase) {
    auto ins = m_mapModules.insert(
        std::make_pair(std::make_pair(nPort, strBase), new QmePlugin));
    if (!ins.second)
      return nullptr;

    return ins.first->second.get();
  }

  GateSrvPlugin* GetModule(int nPort, const std::string& strBaseName) {
    auto itm = m_mapModules.find(std::make_pair(nPort, strBaseName));
    if (itm == m_mapModules.end())
      return nullptr;

    return itm->second.get();
  }
};
