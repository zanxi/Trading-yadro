#include "stdafx.h"
#include "QmePlugin.h"
#include <Utils\StringUtils.h>

#include <DomainSerializer.h>

#include "DBA/DBAClasses.h"
#include "DBA/DBAFirms.h"
#include "DBA/DBAAccounts.h"
#include "DBA/DBATags.h"
#include "DBA/DBAGroups.h"
#include "DBA/DBASettleCodes.h"
#include "DBA/DBASchedule.h"
#include "DBA/DBACalendars.h"
#include "DBA/DBADates.h"
#include "DBA/DBAClients.h"
#include "DBA/DBASecurities.h"
#include "DBA/DBATradeParams.h"
#include "DBA/DBACoupon.h"
#include "DBA/DBACurrency.h"
#include "DBA/DBAExtraClassParam.h"
#include "DBA/DBAPerson.h"
#include "DBA/DBAMarkets.h"
#include "DBA/DBAAuctions.h"

#include "QmePluginTemplate.h"

using namespace QME;

std::string ConvertBSTRoString(const BSTR bstr, size_t wlen)
{
	int len = ::WideCharToMultiByte(CP_ACP, 0, bstr, wlen, nullptr, 0, nullptr, nullptr);

	std::string strres(len, 0);
	len = ::WideCharToMultiByte(CP_ACP, 0, bstr, wlen, &strres[0], len, nullptr, nullptr);

	return strres;
}

static const std::string astrContractTypes[] = { PSRV::INI_PARAM_COMPLEX_TYPE_SPOT, PSRV::INI_PARAM_COMPLEX_TYPE_FUT
	, PSRV::INI_PARAM_COMPLEX_TYPE_FX		// Валюта
	, PSRV::INI_PARAM_COMPLEX_TYPE_BONDS 	// Облигации
	, PSRV::INI_PARAM_COMPLEX_TYPE_OPT };	// Опционы

// Дефолтные значения параметров:

static const std::string astrContractTypes_DefaultValues_Trans[] =
{
	"Desc\\Transactions.dsc", "Desc\\Transactions.dsc", "Desc\\Transactions.dsc", "Desc\\Transactions.dsc"
	, "Desc\\Transactions.dsc"
};

static const std::string astrContractTypes_DefaultValues_SDP[] =
{
	"Desc\\Parameters.dsc", "Desc\\FutParameters.dsc", "Desc\\FxParameters.dsc", "Desc\\BondParameters.dsc"
	, "Desc\\OptParameters.dsc"
};

QmePlugin::QmePlugin()
	: GateSrvPlugin(PCORE::PMODULE_TYPE_QME)
{}

void QmePlugin::CreateDbError(CAlert& aErr, const std::string& strFunc, const std::string& strTable
	, const std::string& strError, int err)
{
	aErr.clear();
	aErr << strFunc;
	aErr << strTable;
	aErr << err;
	aErr << strError;
}

void QmePlugin::InitClassesDefaults(Library::Ini::CIniFile& iniLocal)
{
	for (int i = 0; i < COUNT_OF(astrContractTypes); i++)
	{
		std::string strSection = PSRV::INI_PARAM_COMPLEX_SDP + astrContractTypes[i];
		std::string strVal = iniLocal.GetValue(PSRV::INI_SECTION_CLASSES_DEFAULTS, strSection, astrContractTypes_DefaultValues_SDP[i]);
		if (!strVal.empty())
			m_mSDPPath.insert(std::make_pair(i, strVal));

		strSection = PSRV::INI_PARAM_COMPLEX_TRANS + astrContractTypes[i];
		strVal = iniLocal.GetValue(PSRV::INI_SECTION_CLASSES_DEFAULTS, strSection, astrContractTypes_DefaultValues_Trans[i]);
		if (!strVal.empty())
			m_mTransPath.insert(std::make_pair(i, strVal));
	}

	std::string emptyString;
	iniLocal.SetStrings(PSRV::INI_SECTION_CLASSES_DEFAULTS, emptyString.c_str());
	iniLocal.RemoveSection(PSRV::INI_SECTION_CLASSES_DEFAULTS);
}

BOOL QmePlugin::CheckDataProviders(unsigned short port)
{
	GATECONFIG dbINITIALIZE;
	dbINITIALIZE.ini_file_name = m_strModuleIniFile;

	return Initialize(dbINITIALIZE, port);
}

BOOL QmePlugin::GetTableImpl(CAlert& aGetTable, const std::string& strTableName, BOOL bForced, const std::string& strFilter, CAlert& alFilter)
{
	std::string db_User;
	if (!CheckConnection(db_User))
	{
		aGetTable.clear();
		int err = 3; // no db connectivity
		aGetTable << std::string("GetTable ") << std::string(strTableName) << err; // << LoadResourseString(DONT_CONNECT_DB, UserLocale(user_id));
		return FALSE;
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CLASSES))
	{
		return GetClasses(aGetTable, bForced, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_ACCOUNTS))
	{
		return GetAccounts(aGetTable, bForced, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CALENDARS))
	{
		return GetCalendars(aGetTable, bForced, db_User);
	}	

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CALENDARDATES))
	{
		return GetCalendarDates(aGetTable, bForced, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CLIENTS))
	{
		return GetClients(aGetTable, bForced, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_FIRMS))
	{
		return GetFirms(aGetTable, bForced, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_SECURITIES))
	{
		return GetSecuririties(aGetTable, bForced, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_TAGS))
	{
		return GetTags(aGetTable, bForced, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CORE_GROUPS))
	{
		return GetGroups(aGetTable, bForced, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_SETTLE_CODES))
	{
		return GetSettleCodes(aGetTable, bForced, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_SCHEDULE))
	{
		return GetSchedule(aGetTable, bForced, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_TRADE_PARAMS))
	{
		return GetTradeParam(aGetTable, bForced, db_User);
	}

// 	if (Library::Utils::StringNoCaseEq(strTableName, GQA_TABLENAME_CALENDAR_DATES))
// 	{
// 		return GetDates(aGetTable, bForced, db_User);
// 	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CONNECTIONS))
	{
		return GetConnections(aGetTable);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_PROPS))
	{
		return GetProperties(aGetTable);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, CInfoQmeCoupon::TABLE_NAME))
		return GetAllObjects<CCouponGetAccessor>(aGetTable, bForced, db_User);

	if (Library::Utils::StringNoCaseEq(strTableName, CInfoQmeCurrency::TABLE_NAME))
		return GetAllObjects<CCurrencyGetAccessor>(aGetTable, bForced, db_User);

	if (Library::Utils::StringNoCaseEq(strTableName, CInfoQmeExtraClassParam::TABLE_NAME))
		return GetAllObjects<CExtraClassParamGetAccessor>(aGetTable, bForced, db_User);

	if (Library::Utils::StringNoCaseEq(strTableName, CInfoQmePerson::TABLE_NAME))
		return GetAllObjects<CPersonGetAccessor>(aGetTable, bForced, db_User);

	if (Library::Utils::StringNoCaseEq(strTableName, CInfoQmeMarkets::TABLE_NAME))
		return GetAllObjects<CMarketsGetAccessor>(aGetTable, bForced, db_User);

	if (Library::Utils::StringNoCaseEq(strTableName, CInfoQmeAuction::TABLE_NAME))
	{
		return GetAuctions(aGetTable, bForced, db_User);
	}

	CreateCommonError(aGetTable, strTableName, "Unknown Table");
	return FALSE;
}

BOOL QmePlugin::GetClasses(CAlert& a, BOOL bForced, const std::string& strDbUser)
{
	BOOL bRes(FALSE);
	std::string strErrDB;
	devent(LL_HIGH, "GetTable Classes");

	CAlert table_records;
	CAlert field_names;
	int record_count(0);
	HRESULT hr = S_OK;

	CCommand<CDynamicAccessor> cmd;
	hr = cmd.Open(m_OLE_DB_Provider[strDbUser], L"EXEC GQA_GetAllClasses");
	if (!ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		while (cmd.MoveNext() == S_OK)
		{
			CInfoQmeClass info;
			DBSTATUS dbStatus;

			info.ClassId = *(int*)cmd.GetValue("ClassId");
			info.ClassCode = (const char*)cmd.GetValue("ClassCode");
			info.ContractsType = *(int*)cmd.GetValue("ContractsType");

			if (cmd.GetStatus("CalendarName", &dbStatus) && dbStatus != DBSTATUS_S_ISNULL)
				info.CalendarId = *(int*)cmd.GetValue("CalendarName");

			if (cmd.GetStatus("SettleCode", &dbStatus) && dbStatus != DBSTATUS_S_ISNULL)
				info.SettleCodeId = *(int*)cmd.GetValue("SettleCode");

			if (cmd.GetStatus("CrossTradeCheckType", &dbStatus) && dbStatus != DBSTATUS_S_ISNULL)
				info.CrossTradeCheckType = *(int*)cmd.GetValue("CrossTradeCheckType");

			STRING tags = (const char*)cmd.GetValue("ClassTags");
			info.SetTags(tags);

			STRING extraClassParams = (const char*)cmd.GetValue("ExtraClassParams");
			info.SetExtraParams(extraClassParams);

			if (cmd.GetStatus("MarketId", &dbStatus) && dbStatus != DBSTATUS_S_ISNULL)
				info.MarketId = *(int*)cmd.GetValue("MarketId");
			else
				info.MarketId = -1;

			info.MatchingMode = *(int*)cmd.GetValue("MatchingMode");

			info.QuoteBasis = *(int*)cmd.GetValue("QuoteBasis");

			long flags = *(long*)cmd.GetValue("Flags");
			info.Ccy1QtyScaleEnabled = !!(flags & CInfoQmeClass::FlagsBitValues::Bit0);
			info.Use4DecimalPlacesForYield = !!(flags & CInfoQmeClass::FlagsBitValues::Bit1);
			info.AddAccruedInterestToTheValueOrder = !!(flags & CInfoQmeClass::FlagsBitValues::Bit2);

			CAlert record(CDomainSerializer::ToAlert(&info));
			record_count++;
			table_records << record;
			cmd.ClearRecordMemory();
		}

		a.clear();
		BOOL use_filter = FALSE;
		a << std::string(QME::GQA_TABLENAME_CLASSES) << bForced << record_count << use_filter << table_records << field_names;

		bRes = TRUE;
	}
	else
	{
		CreateCommonError(a, QME::GQA_TABLENAME_CLASSES, strErrDB);
		bRes = FALSE;
	}

	cmd.Close();
	cmd.ReleaseCommand();

	return bRes;
}

BOOL QmePlugin::GetConnections(CAlert& a)
{
	Library::Ini::CIniFile iniGate(m_dbINITIALIZE.ini_file_name_global);

	Library::Ini::CIniFile::TSectionsVec vecSec = iniGate.GetSections();

	// find the [pt_connect_<ConnName>] or [disabled_pt_connect_<>] sections
	static const char sz_pt_connect[] = "pt_connect_";
	static const char sz_dis_pt_connect[] = "disabled_pt_connect_";

	CAlert aTableRecords;
	int nRecordCount(0);

	for (auto itSec = vecSec.begin(); itSec != vecSec.end(); itSec++)
	{
		if (_strnicmp(sz_pt_connect, itSec->c_str(), sizeof(sz_pt_connect) - 1) != 0
			&& _strnicmp(sz_dis_pt_connect, itSec->c_str(), sizeof(sz_dis_pt_connect) - 1) != 0)
			continue; // skip other sections

		CAlert aRecord;
		int nEnabled = strncmp(sz_pt_connect, itSec->c_str(), sizeof(sz_pt_connect) - 1) == 0 ? 1 : 0;
		std::string strName = itSec->substr((nEnabled ? sizeof(sz_pt_connect) : sizeof(sz_dis_pt_connect)) - 1, -1);
		std::string strClasses = iniGate.GetValue(*itSec, "Classes", "");
		int nFix2Qme = iniGate.GetValue(*itSec, GQA_TABLE_CONNECTIONS_COLUMN_FIX2QME, 0);
		int nTransCount = iniGate.GetValue(*itSec, "trans_connection_count", 1);
		int nTransTimeout = iniGate.GetValue(*itSec, "trans_connection_timeout", 15);
		std::string strSever = iniGate.GetValue(*itSec, "server_ip_address", "127.0.0.1");
		unsigned short nPort = static_cast<unsigned short>(iniGate.GetValue(*itSec, "port", 0));

		aRecord << strName << strClasses << nFix2Qme << nTransCount << nTransTimeout << strSever << nPort << nEnabled;
		aTableRecords << aRecord;
		nRecordCount++;
	}

	CAlert aFieldNames;
	aFieldNames << std::string(QME::GQA_TABLE_CONNECTIONS_COLUMN_NAME) << std::string(QME::GQA_TABLE_CONNECTIONS_COLUMN_CLASSES)
		<< std::string(QME::GQA_TABLE_CONNECTIONS_COLUMN_FIX2QME) << std::string(QME::GQA_TABLE_CONNECTIONS_COLUMN_COUNT)
		<< std::string(QME::GQA_TABLE_CONNECTIONS_COLUMN_TIMEOUT) << std::string(QME::GQA_TABLE_CONNECTIONS_COLUMN_SERVER)
		<< std::string(QME::GQA_TABLE_CONNECTIONS_COLUMN_PORT) << std::string(QME::GQA_TABLE_CONNECTIONS_COLUMN_ENABLED);

	a.clear();
	BOOL use_filter = FALSE;
	BOOL bForced(FALSE);
	a << std::string(QME::GQA_TABLENAME_CONNECTIONS) << bForced << nRecordCount << use_filter << aTableRecords << aFieldNames;

	return TRUE;
}

BOOL QmePlugin::GetAccounts(CAlert& a, BOOL bForced, const std::string& strDbUser)
{
	BOOL bRes(FALSE);

	std::string strErrDB;
	devent(LL_HIGH, "GetTable Accounts");

	CAlert table_records;
	CAlert field_names;
	int record_count(0);
	HRESULT hr = S_OK;

	CCommand<CDynamicAccessor> cmd;
	hr = cmd.Open(m_OLE_DB_Provider[strDbUser], L"EXEC GQA_GetAllAccounts");
	if (!ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		while (cmd.MoveNext() == S_OK)
		{
			CAlert record;
			record << *(int*)cmd.GetValue(1);				// Account ID
			record << std::string((char*)cmd.GetValue(2));	    // CLASS NAME
			record << *(int*)cmd.GetValue(3);				// Firm ID
			record << std::string((char*)cmd.GetValue(4));	    // CLASSES STRING
			record << *(int*)cmd.GetValue(5);				// Enabled

			record_count++;
			table_records << record;
			cmd.ClearRecordMemory();
		}

		field_names << std::string(QME::GQA_TABLE_ACCOUNTS_COLUMN_ACCOUNT_ID) << std::string(QME::GQA_TABLE_ACCOUNTS_COLUMN_ACCOUNT)
			<< std::string(QME::GQA_TABLE_ACCOUNTS_COLUMN_FIRM_ID) << std::string(QME::GQA_TABLE_ACCOUNTS_COLUMN_CLASSES)
			<< std::string(QME::GQA_TABLE_ACCOUNTS_COLUMN_ENABLED);

		a.clear();
		BOOL use_filter = FALSE;
		a << std::string(QME::GQA_TABLENAME_ACCOUNTS) << bForced << record_count << use_filter << table_records << field_names;

		bRes = TRUE;
	}
	else
	{
		CreateCommonError(a, QME::GQA_TABLENAME_ACCOUNTS, strErrDB);
		bRes = FALSE;
	}

	cmd.Close();
	cmd.ReleaseCommand();

	return bRes;
}

BOOL QmePlugin::GetCalendars(CAlert& a, BOOL bForced, const std::string& strDbUser)
{
	BOOL bRes(FALSE);

	ParamTableQMECalendar m_fields;

	std::string strErrDB;
	devent(LL_HIGH, "GetTable Calendars");

	CAlert table_records;

	CAlert field_names;
	int record_count(0);

	HRESULT hr = S_OK;

	CCommand<CDynamicAccessor> cmd;
	hr = cmd.Open(m_OLE_DB_Provider[strDbUser], L"EXEC GQA_GetAllCalendars");
	if (!ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		while (cmd.MoveNext() == S_OK)
		{
			int Enabled = 0;
			CAlert record;
						
			m_fields.CalendarId= *(int*)cmd.GetValue(1);
			m_fields.CalendarName=std::string((char*)cmd.GetValue(2));
			m_fields.Enabled = *(int*)cmd.GetValue(3);
			m_fields.Enabled = (m_fields.Enabled&CInfoQmeClass::FlagsBitValues::Bit0);
// 			//strDates = std::string((char*) cmd.GetValue(3)); // Calendar Dates
// 			// Parse Calendar Dates
// 			Library::Utils::TStrVector vDates;
// 			Library::Utils::ParseStrToVector('|', strDates, vDates);
// 			for (auto itd = vDates.begin(); itd != vDates.end(); itd++)
// 			{
// 				Library::Utils::TStrVector vStruct;
// 				Library::Utils::ParseStrToVector(',', *itd, vStruct);
// 				if (vStruct.size() < 2)
// 					continue;
// 
// 				CAlert aCD;
// 				int x;
// 				aCD << vStruct[0];
// 				aCD << (x = atoi(vStruct[1].c_str()));
// 
// 				aDates << aCD;
// 			}
// 
// 			record << aDates;

			record << m_fields.CalendarId;	// Calendar ID
			record << m_fields.CalendarName;// Calendar Name
			record << m_fields.Enabled;		// Enabled

			record_count++;
			table_records << record;
			cmd.ClearRecordMemory();
		}

		field_names << std::string(QME::GQA_TABLE_CALENDARS_COLUMN_CALENDAR_ID)
			<< std::string(QME::GQA_TABLE_CALENDARS_COLUMN_CALENDAR_NAME)
			<< std::string(QME::GQA_TABLE_CALENDARS_COLUMN_ENABLED);

		a.clear();
		BOOL use_filter = FALSE;
		a << std::string(QME::GQA_TABLENAME_CALENDARS) << bForced << record_count << use_filter << table_records << field_names;

		bRes = TRUE;
	}
	else
	{
		CreateCommonError(a, QME::GQA_TABLENAME_CALENDARS, strErrDB);
		bRes = FALSE;
	}

	cmd.Close();
	cmd.ReleaseCommand();

	return bRes;
}


BOOL QmePlugin::GetCalendarDates(CAlert& a, BOOL bForced, const std::string& strDbUser)
{
	BOOL bRes(FALSE);

	ParamTableQMECalendarDate m_fields;

	std::string strErrDB;
	devent(LL_HIGH, "GetTable Calendar Dates");

	CAlert table_records;
	CAlert field_names;
	int record_count(0);
	HRESULT hr = S_OK;

	CCommand<CDynamicAccessor> cmd;
	hr = cmd.Open(m_OLE_DB_Provider[strDbUser], L"EXEC GQA_GetAllCalendarDates");
	if (!ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		int k = 0;
		while (cmd.MoveNext() == S_OK)
		{
			int Enabled = 0;
			CAlert record;

			m_fields.id_unique = k;
			m_fields.CalendarId = *(int*)cmd.GetValue(1);
			m_fields.Date = *(int*)cmd.GetValue(2);
			m_fields.TradeIndicator = *(int*)cmd.GetValue(3);
			k++;
			//m_fields.TradeIndicator = (m_fields.TradeIndicator&CInfoQmeClass::FlagsBitValues::Bit0);

			record << m_fields.id_unique;
			record << m_fields.CalendarId;	// Calendar ID
			record << m_fields.Date;// Date
			record << m_fields.TradeIndicator;	// TradeIndicator

			record_count++;
			table_records << record;
			cmd.ClearRecordMemory();
		}

		field_names << std::string(QME::GQA_TABLE_CALENDARDATES_COLUMN_CALENDAR_ID)
			<< std::string(QME::GQA_TABLE_CALENDARDATES_COLUMN_CALENDAR_DATE)
			<< std::string(QME::GQA_TABLE_CALENDARDATES_COLUMN_TRADEINDICATOR);

		a.clear();
		BOOL use_filter = FALSE;
		a << std::string(QME::GQA_TABLENAME_CALENDARS) << bForced << record_count << use_filter << table_records << field_names;

		bRes = TRUE;
	}
	else
	{
		CreateCommonError(a, QME::GQA_TABLENAME_CALENDARS, strErrDB);
		bRes = FALSE;
	}

	cmd.Close();
	cmd.ReleaseCommand();

	return bRes;
}


BOOL QmePlugin::GetClients(CAlert& a, BOOL bForced, const std::string& strDbUser)
{
	BOOL bRes(FALSE);

	std::string strErrDB;
	devent(LL_HIGH, "GetTable Clients");

	CAlert table_records;
	CAlert field_names;
	int record_count(0);

	CCommand<CDynamicAccessor> cmd;
	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser], L"EXEC GQA_GetAllClients");
	if (!ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		while (cmd.MoveNext() == S_OK)
		{
			CInfoQmeClient info;

			info.ClientId = *(int*) cmd.GetValue(1);
			info.FirmId = *(int*) cmd.GetValue(2);
			info.ClientCode = std::string((char*) cmd.GetValue(3));
			info.ClientType = *(int*) cmd.GetValue(4);
			info.PersonId = *(int*)cmd.GetValue(5);

			CAlert record(CDomainSerializer::ToAlert(&info));
			record_count++;
			table_records << record;
			cmd.ClearRecordMemory();
		}

		a.clear();
		BOOL use_filter = FALSE;
		a << std::string(QME::GQA_TABLENAME_CLIENTS) << bForced << record_count << use_filter << table_records << field_names;

		bRes = TRUE;
	}
	else
	{
		CreateCommonError(a, QME::GQA_TABLENAME_CLIENTS, strErrDB);
		bRes = FALSE;
	}

	cmd.Close();
	cmd.ReleaseCommand();

	return bRes;
}

BOOL QmePlugin::GetFirms(CAlert& a, BOOL bForced, const std::string& strDbUser)
{
	BOOL bRes;

	std::string strErrDB;
	devent(LL_HIGH, "GetTable Firms");

	CAlert table_records;
	CAlert field_names;
	int record_count(0);
	HRESULT hr = S_OK;

	CCommand<CDynamicAccessor> cmd;
	//	hr = cmd.Open(m_OLE_DB_Provider[db_User], strtows(sql, CP_ACP).c_str());
	hr = cmd.Open(m_OLE_DB_Provider[strDbUser], L"EXEC GQA_GetAllFirms");
	if (!ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		while (cmd.MoveNext() == S_OK)
		{
			CAlert record;
			record << *(int*) cmd.GetValue(1);				// Firm ID
			record << std::string((char*) cmd.GetValue(2));	    // Firm Code
			record << std::string((char*) cmd.GetValue(3));	    // Firm Name
			record << *(int*) cmd.GetValue(4);				// Permissions
			record << std::string((char*) cmd.GetValue(5));	    // Exchange
			record << std::string((char*) cmd.GetValue(6));	    // Classes

			record_count++;
			table_records << record;
			cmd.ClearRecordMemory();
		}

		field_names << std::string(QME::GQA_TABLE_FIRMS_COLUMN_FIRM_ID) << std::string(QME::GQA_TABLE_FIRMS_COLUMN_FIRM_CODE)
			<< std::string(QME::GQA_TABLE_FIRMS_COLUMN_FIRM_NAME) << std::string(QME::GQA_TABLE_FIRMS_COLUMN_PERMISSIONS)
			<< std::string(QME::GQA_TABLE_FIRMS_COLUMN_EXCHANGE) << std::string(QME::GQA_TABLE_FIRMS_COLUMN_CLASSES);

		a.clear();
		BOOL use_filter = FALSE;
		a << std::string(QME::GQA_TABLENAME_FIRMS) << bForced << record_count << use_filter << table_records << field_names;

		bRes = TRUE;
	}
	else
	{
		CreateCommonError(a, QME::GQA_TABLENAME_FIRMS, strErrDB);
		bRes = FALSE;
	}

	cmd.Close();
	cmd.ReleaseCommand();

	return bRes;
}

BOOL QmePlugin::GetSecuririties(CAlert& a, BOOL bForced, const std::string& strDbUser)
{
	BOOL bRes;
	std::string strErrDB;
	devent(LL_HIGH, "GetTable Securities");

	CAlert table_records;
	CAlert field_names;
	int record_count(0);
	HRESULT hr = S_OK;

	CCommand<CAccessor<CSecurityGetAccessor>> cmd;
	hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (!ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		while (cmd.MoveNext() == S_OK)
		{
			CInfoQmeSecurity info;
			cmd.ConvertTo(info);
			CAlert record(CDomainSerializer::ToAlert(&info));
			record_count++;
			table_records << record;
			cmd.ClearRecordMemory();
		}

		a.clear();
		BOOL use_filter = FALSE;
		a << std::string(QME::GQA_TABLENAME_SECURITIES) << bForced << record_count << use_filter << table_records << field_names;

		bRes = TRUE;
	}
	else
	{
		CreateCommonError(a, QME::GQA_TABLENAME_SECURITIES, strErrDB);
		bRes = FALSE;
	}

	cmd.Close();
	cmd.ReleaseCommand();

	return bRes;
}

BOOL QmePlugin::GetTags(CAlert& a, BOOL bForced, const std::string& strDbUser)
{
	BOOL bRes;
	std::string strErrDB;
	devent(LL_HIGH, "GetTable Tags");

	CAlert table_records;
	CAlert field_names;
	int record_count(0);
	HRESULT hr = S_OK;

	CCommand<CDynamicAccessor> cmd;
	//	hr = cmd.Open(m_OLE_DB_Provider[db_User], strtows(sql, CP_ACP).c_str());
	hr = cmd.Open(m_OLE_DB_Provider[strDbUser], L"EXEC GQA_GetAllTags");
	if (!ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		while (cmd.MoveNext() == S_OK)
		{
			CAlert record;
			record << *(int*) cmd.GetValue(1);				// Tag ID
			record << std::string((char*) cmd.GetValue(2));	    // TAG
			record << std::string((char*) cmd.GetValue(3));	    // Classes

			record_count++;
			table_records << record;
			cmd.ClearRecordMemory();
		}

		field_names << std::string(QME::GQA_TABLE_TAGS_COLUMN_TAG_ID) << std::string(QME::GQA_TABLE_TAGS_COLUMN_TAG)
			<< std::string(QME::GQA_TABLE_TAGS_COLUMN_CLASSES);

		a.clear();
		BOOL use_filter = FALSE;
		a << std::string(QME::GQA_TABLENAME_TAGS) << bForced << record_count << use_filter << table_records << field_names;

		bRes = TRUE;
	}
	else
	{
		CreateCommonError(a, QME::GQA_TABLENAME_TAGS, strErrDB);
		bRes = FALSE;
	}

	return bRes;
}

BOOL QmePlugin::GetGroups(CAlert& a, BOOL bForced, const std::string& strDbUser)
{
	BOOL bRes;
	std::string strErrDB;
	devent(LL_HIGH, "GetTable Core Groups");

	CAlert table_records;
	CAlert field_names;
	int record_count(0);
	HRESULT hr = S_OK;

	CCommand<CDynamicAccessor> cmd;
	//	hr = cmd.Open(m_OLE_DB_Provider[db_User], strtows(sql, CP_ACP).c_str());
	hr = cmd.Open(m_OLE_DB_Provider[strDbUser], L"EXEC GQA_GetAllCoreGroups");
	if (!ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		while (cmd.MoveNext() == S_OK)
		{
			CAlert record;
			record << *(int*) cmd.GetValue(1);				// Group ID
			record << std::string((char*) cmd.GetValue(2));	    // Group Name

			record_count++;
			table_records << record;
			cmd.ClearRecordMemory();
		}

		field_names << std::string(QME::GQA_TABLE_CG_COLUMN_GROUP_ID) << std::string(QME::GQA_TABLE_CG_COLUMN_GROUP_NAME);

		a.clear();
		BOOL use_filter = FALSE;
		a << std::string(QME::GQA_TABLENAME_CORE_GROUPS) << bForced << record_count << use_filter << table_records << field_names;

		bRes = TRUE;
	}
	else
	{
		CreateCommonError(a, QME::GQA_TABLENAME_CORE_GROUPS, strErrDB);
		bRes = FALSE;
	}

	return bRes;

}

BOOL QmePlugin::GetSettleCodes(CAlert& a, BOOL bForced, const std::string& strDbUser)
{
	BOOL bRes(FALSE);

	std::string strErrDB;
	devent(LL_HIGH, "GetTable SettlCodes");

	CAlert table_records;
	CAlert field_names;
	int record_count(0);
	HRESULT hr = S_OK;

	CCommand<CDynamicAccessor> cmd;
	//	hr = cmd.Open(m_OLE_DB_Provider[db_User], strtows(sql, CP_ACP).c_str());
	hr = cmd.Open(m_OLE_DB_Provider[strDbUser], L"EXEC GQA_GetAllSettleCodes");
	if (!ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		while (cmd.MoveNext() == S_OK)
		{
			CAlert record;
			record << *(int*) cmd.GetValue(1);				// Settle Code ID
			record << std::string((char*) cmd.GetValue(2));	    // Settle Code
			record << *(int*) cmd.GetValue(3);	    // Settle Days

			record_count++;
			table_records << record;
			cmd.ClearRecordMemory();
		}

		field_names << std::string(QME::GQA_TABLE_SC_COLUMN_SETTLE_CODE_ID) << std::string(QME::GQA_TABLE_SC_COLUMN_SETTLE_CODE)
			<< std::string(QME::GQA_TABLE_SC_COLUMN_SETTLE_DAYS);

		a.clear();
		BOOL use_filter = FALSE;
		a << std::string(QME::GQA_TABLENAME_SETTLE_CODES) << bForced << record_count << use_filter << table_records << field_names;

		bRes = TRUE;
	}
	else
	{
		CreateCommonError(a, QME::GQA_TABLENAME_SETTLE_CODES, strErrDB);
		bRes = FALSE;
	}

	cmd.Close();
	cmd.ReleaseCommand();
	return bRes;
}

BOOL QmePlugin::GetSchedule(CAlert& a, BOOL bForced, const std::string& strDbUser)
{
	BOOL bRes;
	std::string strErrDB;
	devent(LL_HIGH, "GetTable Schedule");

	CAlert table_records;
	CAlert field_names;
	int record_count(0);
	HRESULT hr = S_OK;

	CCommand<CDynamicAccessor> cmd;
	//	hr = cmd.Open(m_OLE_DB_Provider[db_User], strtows(sql, CP_ACP).c_str());
	hr = cmd.Open(m_OLE_DB_Provider[strDbUser], L"EXEC GQA_GetAllSchedule");
	if (!ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		while (cmd.MoveNext() == S_OK)
		{
			CAlert record;
			record << *(int*) cmd.GetValue(1); // Class ID

			// SecurityID
			{
				int secID(-1);
				DBSTATUS dbStatus;
				if (cmd.GetStatus(2, &dbStatus) && dbStatus != DBSTATUS_S_ISNULL)
					secID = *(int*)cmd.GetValue(2);
				record << secID;
			}

			// SecCode
			{
				DBSTATUS dbStatus;
				STRING secCode;
				if (cmd.GetStatus(3, &dbStatus) && dbStatus != DBSTATUS_S_ISNULL)
					secCode = std::string((char*)cmd.GetValue(3));
				record << secCode;
			}

			record << std::string((char*) cmd.GetValue(4));	// EventCode
			record << std::string((char*) cmd.GetValue(5));	// EventTime
			record << *(int*) cmd.GetValue(6);				// CancelOrders

			record_count++;
			table_records << record;
			cmd.ClearRecordMemory();
		}

		field_names << std::string(QME::GQA_TABLE_SCHEDULE_COLUMN_CLASS_ID)
			<< std::string(QME::GQA_TABLE_SCHEDULE_COLUMN_SEC_ID)
			<< std::string(QME::GQA_TABLE_SCHEDULE_COLUMN_SEC_CODE)
			<< std::string(QME::GQA_TABLE_SCHEDULE_COLUMN_EVENT_CODE)
			<< std::string(QME::GQA_TABLE_SCHEDULE_COLUMN_EVENT_TIME)
			<< std::string(QME::GQA_TABLE_SCHEDULE_COLUMN_CANCEL_ORDERS);

		a.clear();
		BOOL use_filter = FALSE;
		a << std::string(QME::GQA_TABLENAME_SCHEDULE) << bForced << record_count << use_filter << table_records << field_names;

		bRes = TRUE;
	}
	else
	{
		CreateCommonError(a, QME::GQA_TABLENAME_SCHEDULE, strErrDB);
		bRes = FALSE;
	}

	cmd.Close();
	cmd.ReleaseCommand();
	return bRes;
}

BOOL QmePlugin::GetProperties(CAlert& aRow)
{
	static std::multimap<std::string, std::string> mapAvailableProps;
	if (mapAvailableProps.empty())
	{
		mapAvailableProps.insert(std::make_pair("System", "Timeshift"));
		mapAvailableProps.insert(std::make_pair("System", "ClearingTime"));
	}

	Library::Ini::CIniFile iniGate(m_dbINITIALIZE.ini_file_name_global);

	CAlert aTableRecords;
	int nRecordCount(0);

	for (auto itProps = mapAvailableProps.begin(); itProps != mapAvailableProps.end(); itProps++)
	{
		CAlert aRecord;
		std::string strValue;
		std::string strName;

		strValue = iniGate.GetValue(itProps->first, itProps->second, "");
		strName = "[" + itProps->first + "]." + itProps->second;

		aRecord << strName << strValue;
		aTableRecords << aRecord;
		nRecordCount++;
	}

	CAlert aFieldNames;
	aFieldNames << std::string(QME::GQA_TABLE_PROPS_COLUMN_NAME) << std::string(QME::GQA_TABLE_PROPS_COLUMN_VALUE);

	aRow.clear();
	BOOL use_filter = FALSE;
	BOOL bForced(FALSE);
	aRow << std::string(QME::GQA_TABLENAME_PROPS) << bForced << nRecordCount << use_filter << aTableRecords << aFieldNames;

	return TRUE;
}

BOOL QmePlugin::GetTradeParam(CAlert& a, BOOL bForced, const std::string& strDbUser)
{
	BOOL bRes;
	std::string strErrDB;
	devent(LL_HIGH, "GetTable TradeCreationParams");

	CAlert table_records;
	CAlert field_names;
	int record_count(0);
	HRESULT hr = S_OK;

	CCommand<CDynamicAccessor> cmd;
	//	hr = cmd.Open(m_OLE_DB_Provider[db_User], strtows(sql, CP_ACP).c_str());
	hr = cmd.Open(m_OLE_DB_Provider[strDbUser], L"EXEC GQA_GetAllTradeCreationParams");
	if (!ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		while (cmd.MoveNext() == S_OK)
		{
			CAlert record;
			record << *(int*)cmd.GetValue(1);				// Param ID
			record << *(int*)cmd.GetValue(2);	    // Firm Id
			record << *(int*)cmd.GetValue(3);	    // Class Id
			record << *(int*)cmd.GetValue(4);	    // Trade Mode

			long nNull(-1);
			DBSTATUS dbStatus;
			if (cmd.GetStatus(5, &dbStatus)) // AccountId
			{
				if (dbStatus != DBSTATUS_S_ISNULL)
					record << *(int*)cmd.GetValue(5);
				else
					record << nNull;
			}
			else
				record << nNull;

			if (cmd.GetStatus(6, &dbStatus)) // Client
			{
				if (dbStatus != DBSTATUS_S_ISNULL)
					record << *(int*)cmd.GetValue(6);
				else
					record << nNull;
			}
			else
				record << nNull;

			record_count++;
			table_records << record;
			cmd.ClearRecordMemory();
		}

		field_names << std::string(QME::GQA_TABLE_TCP_COLUMN_ID) << std::string(QME::GQA_TABLE_TCP_COLUMN_FIRM_ID)
			<< std::string(QME::GQA_TABLE_TCP_COLUMN_CLASS_ID) << std::string(QME::GQA_TABLE_TCP_COLUMN_MODE)
			<< std::string(QME::GQA_TABLE_TCP_COLUMN_ACCOUNT_ID) << std::string(QME::GQA_TABLE_TCP_COLUMN_CLIENT_ID);

		a.clear();
		BOOL use_filter = FALSE;
		a << std::string(QME::GQA_TABLENAME_TRADE_PARAMS) << bForced << record_count << use_filter << table_records << field_names;

		bRes = TRUE;
	}
	else
	{
		CreateCommonError(a, QME::GQA_TABLENAME_TRADE_PARAMS, strErrDB);
		bRes = FALSE;
	}

	return bRes;
}

BOOL QmePlugin::GetIniSDPClass(const std::string& strClassName)
{
	return GetClassesBySection(strClassName, "Classes");
}

BOOL QmePlugin::GetIniTransClass(const std::string& strClassName)
{
	return GetClassesBySection(strClassName, "TransClasses");
}

BOOL QmePlugin::GetClassesBySection(const std::string& strClassName, std::string strSection)
{
	Library::Ini::CIniFile iniGate(m_dbINITIALIZE.ini_file_name_global); // 25 october 2019

	Library::Ini::CIniFile::TRecordsVec vecClasses(iniGate.GetRecords(strSection));

	return std::find_if(vecClasses.begin(), vecClasses.end(),
		[&strClassName](Library::Ini::CIniFile::CRecord& rr) {
			return strClassName == rr.m_strName;
		}) != vecClasses.end();
}

BOOL QmePlugin::GetAuctions(CAlert& a, BOOL bForced, const std::string& strDbUser)
{
	BOOL bRes;
	std::string strErrDB;
	devent(LL_HIGH, "GetTable Auctions");

	CAlert table_records;
	CAlert field_names;
	int record_count(0);
	HRESULT hr = S_OK;

	CCommand<CAccessor<CAuctionGetAccessor>> cmd;
	hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (!ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		while (cmd.MoveNext() == S_OK)
		{
			CInfoQmeAuction info;
			cmd.ConvertTo(info);
			CAlert record(CDomainSerializer::ToAlert(&info));
			record_count++;
			table_records << record;
			cmd.ClearRecordMemory();
		}

		a.clear();
		BOOL use_filter = FALSE;
		a << std::string(QME::GQA_TABLENAME_SECURITIES) << bForced << record_count << use_filter << table_records << field_names;

		bRes = TRUE;
	}
	else
	{
		CreateCommonError(a, QME::GQA_TABLENAME_SECURITIES, strErrDB);
		bRes = FALSE;
	}

	cmd.Close();
	cmd.ReleaseCommand();

	return bRes;
}

BOOL QmePlugin::InsertAndUpdateTable(CAlert& aRow, const std::string& strTableName, QME::CrudActions act)
{
	std::string db_User;
	if (!CheckConnection(db_User))
	{
		aRow.clear();
		int err = 3; // no db connectivity
		aRow << std::string(PCORE::GQA_FUNCTION_UPDATE_TABLE) << std::string(strTableName) << err; // << LoadResourseString(DONT_CONNECT_DB, UserLocale(user_id));
		return FALSE;
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CLASSES))
	{
		return IAUClasses(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_ACCOUNTS))
	{
		return IAUAccounts(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CALENDARS))
	{
		return IAUCalendars(aRow, db_User, act);
	}
	
	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CLIENTS))
	{
		return IAUClients(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_FIRMS))
	{
		return IAUFirms(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_SECURITIES))
	{
		return IAUSecurities(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_TAGS))
	{
		return IAUTags(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CORE_GROUPS))
	{
		return IAUGroups(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_SETTLE_CODES))
	{
		return IAUSettleCodes(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_SCHEDULE))
	{
		return IAUSchedule(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_TRADE_PARAMS))
	{
		return IAUTradeParams(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CONNECTIONS))
	{
		return IAUConnection(aRow);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_PROPS))
	{
		return IAUProperty(aRow);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, CInfoQmeCoupon::TABLE_NAME))
		return EditObject<CCouponEditAccessor>(aRow, db_User, act);

	if (Library::Utils::StringNoCaseEq(strTableName, CInfoQmeCurrency::TABLE_NAME))
		return EditObject<CCurrencyEditAccessor>(aRow, db_User, act);

	if (Library::Utils::StringNoCaseEq(strTableName, CInfoQmeExtraClassParam::TABLE_NAME))
		return EditObject<CExtraClassParamEditAccessor>(aRow, db_User, act);

	if (Library::Utils::StringNoCaseEq(strTableName, CInfoQmePerson::TABLE_NAME))
		return EditObject<CPersonEditAccessor>(aRow, db_User, act);

	if (Library::Utils::StringNoCaseEq(strTableName, CInfoQmeMarkets::TABLE_NAME))
		return EditObject<CMarketsEditAccessor>(aRow, db_User, act);
	
	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_AUCTIONS))
	{
		return IAUAuctions(aRow, db_User);
	}

	CreateCommonError(aRow, strTableName, "Unknown Table");
	return FALSE;
}

BOOL QmePlugin::IAUClasses(CAlert& aRow, const std::string& strDbUser)
{	

	BOOL bRes(FALSE);
	std::string strErrDB;

	CInfoQmeClass info;
	CDomainSerializer::FromAlert(&info, aRow);

	devent(LL_LOW, "Updating %s... Parameters: CLASS_ID[%d], CLASS_CODE[%s], CLASS_TAGS[%s], CLASS_CALENDAR[%d], \
		CLASS_SETTLE[%d], CLASS_CONTRACTS[%d], CLASS_CROSSTRADECHECKTYPE[%d], CLASS_CCY1QTYSCALEENABLED[%d], MARKET_ID[%d]"
		, CInfoQmeClass::TABLE_NAME.c_str()
		, info.ClassId, info.ClassCode.c_str(), info.GetTags().c_str(), info.CalendarId, info.SettleCodeId
		, info.ContractsType, info.CrossTradeCheckType, info.Ccy1QtyScaleEnabled, info.MarketId);

	CCommand< CAccessor<CCLASS_INSAccessor> > cmd;

	COPY_INTO_FIELD_STDSTR(cmd.ClassCode, info.ClassCode);
	COPY_INTO_FIELD_STDSTR(cmd.ClassTags, info.GetTags());
	SET_NULLABLE_INT_PARAM(cmd.CalendarId, cmd.CalendarId_status, info.CalendarId);
	SET_NULLABLE_INT_PARAM(cmd.SettleCodeId, cmd.SettleCodeId_status, info.SettleCodeId);
	SET_NULLABLE_INT_PARAM(cmd.CrossTradeCheckType, cmd.CrossTradeCheckType_status, info.CrossTradeCheckType);
	cmd.ContractsType = info.ContractsType;
	COPY_INTO_FIELD_STDSTR(cmd.ExtraClassParams, info.GetExtraParams());
	cmd.ClassId = info.ClassId;
	cmd.Flags = cmd.Flags & ~CInfoQmeClass::FlagsBitValues::AllRelevantBits
		| GetFlags(&info);
	SET_NULLABLE_INT_PARAM(cmd.MarketId, cmd.MarketId_status, info.MarketId);
	cmd.MatchingMode = info.MatchingMode;
	cmd.QuoteBasis = info.QuoteBasis;
	
	if (!CheckIniError())
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_CLASSES,
			"Error occurred when writing ini file. See server log for more details.", 0);
		return FALSE;
	}
	
	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_CLASSES, strErrDB, 0);
		bRes = FALSE;
	}
	else
	{
		if (m_dbINITIALIZE.ini_file_name_global.empty() == FALSE) {
			SetIniSDPClass(true, info.ContractsType, info.ClassCode); // m_dbINITIALIZE.ini_file_name_global // 25 october 2019		    
		SetIniTransClass(true, info.ContractsType, info.ClassCode);
		}
		else {
			aRow << std::string(QME::CALENDAR_CURRENCY_MESSAGE);
		}

		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_CLASSES);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = 1);
		aRow << (nResult = cmd.ReturnValue);

		bRes = TRUE;
	}

	cmd.Close();
	cmd.ReleaseCommand();

	return bRes;
}

INT QmePlugin::GetFlags(void* info)
{
	INT bitResult = 0;
	auto pInfo = reinterpret_cast<CInfoQmeClass*>(info);

	if (pInfo->Ccy1QtyScaleEnabled)
		bitResult |= CInfoQmeClass::FlagsBitValues::Bit0;

	if (pInfo->Use4DecimalPlacesForYield)
		bitResult |= CInfoQmeClass::FlagsBitValues::Bit1;

	if (pInfo->AddAccruedInterestToTheValueOrder)
		bitResult |= CInfoQmeClass::FlagsBitValues::Bit2;
	return bitResult;
}

BOOL QmePlugin::IAUFirms(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	int nPermission, nFirmId;
	std::string strFirmCode, strFirmName, strExchange, strClasses;

	std::vector< std::pair<long, std::string> > vAccClasses;

#ifdef _DEBUG
	DebugAlert(aRow);
#endif // _DEBUG

	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_FIRMS_COLUMN_FIRM_CODE))
		{
			aField >> strFirmCode;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_FIRMS_COLUMN_FIRM_NAME))
		{
			aField >> strFirmName;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_FIRMS_COLUMN_PERMISSIONS))
		{
			aField >> nPermission;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_FIRMS_COLUMN_EXCHANGE))
		{
			aField >> strExchange;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_FIRMS_COLUMN_CLASSES))
		{
			aField >> strClasses;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_FIRMS_COLUMN_FIRM_ID))
		{
			aField >> nFirmId;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_FIRMS_COLUMN_ACC_MAP))
		{
			long id;
			std::string strAccClasses;
			CAlert aMapComposite;
			aField >> aMapComposite;
			while (aMapComposite.get_fields_count())
			{
				CAlert aMapFld;
				aMapComposite >> aMapFld;
				aMapFld >> id >> strAccClasses;
				vAccClasses.push_back(std::make_pair(id, strAccClasses));
			}
		}
	}

	devent(LL_LOW, "Parameters:FIRM_ID[%d],FIRM_NAME[%s],FIRM_CODE[%s],FIRM_CLASSES[%s],FIRM_PERMISSIONS[%d],FIRM_EXCHANGE[%s]",
		nFirmId, strFirmName.c_str(), strFirmCode.c_str(), strClasses.c_str(), nPermission, strExchange.c_str());

	CCommand< CAccessor<CFIRM_INSAccessor> > cmd;
	COPY_INTO_FIELD(cmd.m_FIRM_CODE, strFirmCode.c_str());
	COPY_INTO_FIELD(cmd.m_FIRM_NAME, strFirmName.c_str());
	COPY_INTO_FIELD(cmd.m_FIRM_CLASSES, strClasses.c_str());
	COPY_INTO_FIELD(cmd.m_FIRM_EXCHANGE, strExchange.c_str());
	cmd.m_PERMISSIONS = nPermission;
	cmd.m_FIRM_ID = nFirmId;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_FIRMS, strErrDB, 0);
		return FALSE;
	}

	for each (const auto& a in vAccClasses)
	{
		CCommand< CAccessor<CAccToClass_Accessor> > cmd;
		COPY_INTO_FIELD(cmd.m_ACC_CLASSES, a.second.c_str());
		cmd.m_ACC_ID = a.first;

		HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
		if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
		{
			CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_FIRMS, strErrDB, 0);
			return FALSE;
		}

		devent(LL_LOW, "SaveAccountClasses:ACC_ID[%d],ACC_CLASSES[%s]",
			a.first, a.second.c_str());
	}

	aRow.clear();
	aRow << std::string(QME::GQA_TABLENAME_FIRMS);
	int nResult = 1;
	aRow << nResult;
	aRow << std::string("");
	aRow << std::string("");
	aRow << (nResult = -1);
	aRow << (nResult = cmd.m_RETURN_VALUE);

	return TRUE;
}

BOOL QmePlugin::IAUSecurities(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	CInfoQmeSecurity info;
	CDomainSerializer::FromAlert(&info, aRow);

	devent(LL_LOW,
"Updating %s... Parameters: SecurityId[%d], ClassId[%d], GroupId[%d], CalendarId[%d], SettleCodeId[%d], SecCode[%s], \
ShortNameRus[%s], FullNameRus[%s], ShortNameEng[%s], FullNameEng[%s], MinStep[%d], FaceUnitId[%d], FaceValue[%f], \
Scale[%d], MatDate[%d], LotSize[%d], ISIN[%s], CFI[%s], RegNumber[%s], BaseClassCode[%s], \
BaseSecCode[%s], TradeCurrencyId[%d], BaseCurrencyId[%d], TermCurrencyId[%d], BasisType[%d], PrevPrice[%I64d], ClPrice[%I64d], \
SellDepo[%f], BuyDepo[%f], StepPrice[%f], ListLevel[%d], Enabled[%d], Subtype[%d], StockCode[%s], \
StockName[%s], SedolCode[%s], RicCode[%s], CusipCode[%s], FigiCode[%s], QtyScale[%d], QtyMultiplier[%I64d], Ccy1QtyScaleEnabled[%d], MarketId[%d], \
StrikePrice[%I64d], OptionKind[%c], OptionType[%c], OptionSettleType[%c]"
		, CInfoQmeSecurity::TABLE_NAME.c_str()
		, info.SecurityId
		, info.ClassId
		, info.GroupId
		, info.CalendarId
		, info.SettleCodeId
		, info.SecCode.c_str()
		, info.ShortNameRus.c_str()
		, info.FullNameRus.c_str()
		, info.ShortNameEng.c_str()
		, info.FullNameEng.c_str()
		, info.MinStep
		, info.FaceUnitId
		, info.FaceValue
		, info.Scale
		, info.MatDate
		, info.LotSize
		, info.ISIN.c_str()
		, info.CFI.c_str()
		, info.RegNumber.c_str()
		, info.BaseClassCode.c_str()
		, info.BaseSecCode.c_str()
		, info.TradeCurrencyId
		, info.BaseCurrencyId
		, info.TermCurrencyId
		, info.BasisType
		, info.PrevPrice
		, info.ClPrice
		, info.SellDepo
		, info.BuyDepo
		, info.StepPrice
		, info.ListLevel
		, (int)info.Enabled
		, info.SubType
		, info.StockCode.c_str()
		, info.StockName.c_str()
		, info.SedolCode.c_str()
		, info.RicCode.c_str()
		, info.CusipCode.c_str()
		, info.FigiCode.c_str()
		, info.QtyScale
		, info.QtyMultiplier
		, info.Ccy1QtyScaleEnabled
		, info.MarketId
		, info.StrikePrice
		, info.OptionKind
		, info.OptionType
		, info.OptionSettleType
		);

	CCommand< CAccessor<CSecurityInsAccessor> > cmd;
	cmd.ConvertFrom(info);

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_SECURITIES, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_SECURITIES);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::IAUAccounts(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	int nFirmId, nEnabled, nAccId;
	std::string strAccount;

#ifdef _DEBUG
	DebugAlert(aRow);
#endif // _DEBUG

	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_ACCOUNTS_COLUMN_ACCOUNT))
		{
			aField >> strAccount;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_ACCOUNTS_COLUMN_FIRM_ID))
		{
			aField >> nFirmId;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_ACCOUNTS_COLUMN_ENABLED))
		{
			aField >> nEnabled;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_ACCOUNTS_COLUMN_ACCOUNT_ID))
		{
			aField >> nAccId;
		}
	}

	devent(LL_LOW, "Parameters:ACC_ID[%d],ACCOUNT_NAME[%s],FIRM_ID[%d],ACCOUNT_ENABLED[%d]",
		nAccId, strAccount.c_str(), nFirmId, nEnabled);

	CCommand< CAccessor<CACCCOUNT_INSAccessor> > cmd;
	COPY_INTO_FIELD(cmd.m_ACCOUNT_NAME, strAccount.c_str());
	cmd.m_FIRM_ID = nFirmId;
	cmd.m_ENABLED = nEnabled;
	cmd.m_ACC_ID = nAccId;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_ACCOUNTS, strErrDB, 0);
		return FALSE;
	}

	aRow.clear();
	aRow << std::string(QME::GQA_TABLENAME_ACCOUNTS);
	int nResult = 1;
	aRow << nResult;
	aRow << std::string("");
	aRow << std::string("");
	aRow << (nResult = -1);
	aRow << (nResult = cmd.m_RETURN_VALUE);

	return TRUE;
}

BOOL QmePlugin::IAUTags(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	std::string strTag, strClasses;

#ifdef _DEBUG
	DebugAlert(aRow);
#endif // _DEBUG

	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_TAGS_COLUMN_TAG))
		{
			aField >> strTag;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_FIRMS_COLUMN_CLASSES))
		{
			aField >> strClasses;
		}
	}

	devent(LL_LOW, "Parameters:TAG[%s],TAG_CLASSES[%s]", strTag.c_str(), strClasses.c_str());

	CCommand< CAccessor<CTAG_INSAccessor> > cmd;
	COPY_INTO_FIELD(cmd.m_TAG_NAME, strTag.c_str());
	COPY_INTO_FIELD(cmd.m_TAG_CLASSES, strClasses.c_str());

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_TAGS, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_TAGS);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::IAUGroups(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	std::string strGroupName;

#ifdef _DEBUG
	DebugAlert(aRow);
#endif // _DEBUG

	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CG_COLUMN_GROUP_NAME))
		{
			aField >> strGroupName;
		}
	}

	devent(LL_LOW, "Parameters:GROUP_NAME[%s]", strGroupName.c_str());

	CCommand< CAccessor<CGROUP_INSAccessor> > cmd;
	COPY_INTO_FIELD(cmd.m_GROUP_NAME, strGroupName.c_str());

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_CORE_GROUPS, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_CORE_GROUPS);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::IAUSettleCodes(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	std::string strName;
	int nDays, nId;

#ifdef _DEBUG
	DebugAlert(aRow);
#endif // _DEBUG

	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_SC_COLUMN_SETTLE_CODE))
		{
			aField >> strName;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_SC_COLUMN_SETTLE_DAYS))
		{
			aField >> nDays;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_SC_COLUMN_SETTLE_CODE_ID))
		{
			aField >> nId;
		}
	}

	devent(LL_LOW, "Parameters:ID[%d],SETTLE_CODE[%s],SETTLE_DAYS[%d]", nId, strName.c_str(), nDays);

	CCommand< CAccessor<CSETTLEDAY_INSAccessor> > cmd;
	COPY_INTO_FIELD(cmd.m_SETTLE_NAME, strName.c_str());
	cmd.m_DAYS = nDays;
	cmd.m_ID = nId;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_SETTLE_CODES, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_SETTLE_CODES);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}
	return TRUE;
}

BOOL QmePlugin::IAUSchedule(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	std::string strSecCode, strEventCode, strEventTime;
	int nClassId, nCancelOrders, nSecId;

	DebugAlert(aRow);

	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_SCHEDULE_COLUMN_CLASS_ID))
		{
			aField >> nClassId;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_SCHEDULE_COLUMN_SEC_CODE))
		{
			aField >> strSecCode;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_SCHEDULE_COLUMN_EVENT_CODE))
		{
			aField >> strEventCode;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_SCHEDULE_COLUMN_EVENT_TIME))
		{
			aField >> strEventTime;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_SCHEDULE_COLUMN_CANCEL_ORDERS))
		{
			aField >> nCancelOrders;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_SCHEDULE_COLUMN_SEC_ID))
		{
			aField >> nSecId;
		}
	}

	devent(LL_LOW, "Parameters:CLASS_ID[%d],SEC_ID[%d],SEC_CODE[%s],EVENT_CODE[%s],EVENT_TIME[%s],CANCEL_ORDERS[%d]",
		nClassId, nSecId, strSecCode.c_str(), strEventCode.c_str(), strEventTime.c_str(), nCancelOrders);

	CCommand< CAccessor<CSCHEDULE_INSAccessor> > cmd;

	if (nSecId == -1)
		cmd.m_SEC_ID_DBS = DBSTATUS_S_ISNULL;
	else
		cmd.m_SEC_ID = nSecId;

	COPY_INTO_FIELD(cmd.m_EVENT_CODE, strEventCode.c_str());
	COPY_INTO_FIELD(cmd.m_EVENT_TIME, strEventTime.c_str());
	cmd.m_CLASS_ID = nClassId;
	cmd.m_CANCEL_ORDERS = nCancelOrders;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_SCHEDULE, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_SCHEDULE);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::UpdateSchedule(CAlert& keyRow, CAlert& newRow, const std::string& strDbUser)
{
	return DeleteSchedule(keyRow, strDbUser) && IAUSchedule(newRow, strDbUser);
}

BOOL QmePlugin::IAUCalendars(CAlert& aRow, const std::string& strDbUser, QME::CrudActions act)
{
	std::string strErrDB;
	ParamDbQMECalendar m_fields = {};	
	CAlert aCDates;

#ifdef _DEBUG
	DebugAlert(aRow);
#endif // _DEBUG

	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CALENDARS_COLUMN_CALENDAR_NAME))
		{
			aField >> m_fields.CalendarName_;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CALENDARS_COLUMN_ENABLED))
		{
			aField >> m_fields.Enabled;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CALENDARS_COLUMN_CALENDAR_DATES))
		{
			aField >> aCDates;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CALENDARS_COLUMN_CALENDAR_ID))
		{
			aField >> m_fields.CalendarId;
		}		
		}

	devent(LL_LOW, "Parameters:CALENDARID[%d],CALENDARNAME[%s],ENABLED[%d]",
		m_fields.CalendarId, m_fields.CalendarName_.c_str(), m_fields.Enabled);

	CCommand<CAccessor<CCALENDAR_INSAccessor2>> cmd;
	cmd.Action = act; // insert new values	 
	//cmd.Action = 0; // insert new values	 
	std::copy(m_fields.CalendarName_.begin(), m_fields.CalendarName_.end(),
		m_fields.CalendarName);
	COPY_INTO_FIELD(cmd.m_fields.CalendarName, m_fields.CalendarName);	
	cmd.m_fields.Enabled = m_fields.Enabled;
	cmd.m_fields.CalendarId = m_fields.CalendarId;


	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_CALENDARS, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_CALENDARS);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_fields.CalendarId);
	}
	
	cmd.Close();
	cmd.ReleaseCommand();
		
	// save record in dates calendar with calendarId=cmd.ReturnValue
	if(act!= QME::CRUD_CREATE) IAUCalendarDates(aCDates, cmd.m_fields.CalendarId,aRow,strDbUser);
	else IAUCalendarDates(aCDates, cmd.ReturnValue, aRow, strDbUser);
	
	return TRUE;
}


void QmePlugin::IAUCalendarDates(CAlert aCDates, int ReturnValue, CAlert& aRow, const std::string& strDbUser)
{	


#ifdef _DEBUG
	DebugAlert(aRow);
#endif // _DEBUG

	std::string strErrDB;
	std::ostringstream oss;
	ParamTableQMECalendarDate tb;
	
	//TQMECalendarDate
	VECTOR_QME_CALENDARDATES vCD;
	CInfoQmeCalendarDate infoCD;
		
	long nDate;
	long nTrade;
	long action;
	while (aCDates.get_fields_count())
	{

		CAlert aDate;
		aCDates >> aDate;
		aDate >> nDate >> nTrade >> action;
		oss << nDate << "," << nTrade << "," << action << "|";

		infoCD.fields.CalendarId = ReturnValue;
		infoCD.fields.Date = nDate;
		infoCD.fields.TradeIndicator = nTrade;
		infoCD.fields.action = action;

		vCD.push_back(infoCD);		
	}	

	for(int i=0;i<vCD.size();i++)
	{
		if (vCD[i].fields.action == 2) {
			CCommand<CAccessor<CCALENDARDATE_INSAccessor2>> cmd_date;
			cmd_date.Action = vCD[i].fields.action; // insert new values	 	
			cmd_date.m_fields.Date = vCD[i].fields.Date;
			cmd_date.m_fields.TradeIndicator = vCD[i].fields.TradeIndicator;
			cmd_date.m_fields.CalendarId = ReturnValue;


			HRESULT hr_date = cmd_date.Open(m_OLE_DB_Provider[strDbUser]);
			if (ISFAILED(hr_date, strErrDB, m_OLE_DB_Provider))
			{
				CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_CALENDARDATES, strErrDB, 0);
				return;
			}
			else
			{
				aRow.clear();
				aRow << std::string(QME::GQA_TABLENAME_CALENDARDATES);

				int nResult = 1;
				aRow << nResult;
				aRow << std::string("");
				aRow << std::string("");
				aRow << (nResult = -1);
				aRow << (nResult = cmd_date.m_fields.CalendarId);
			}
			cmd_date.Close();
			cmd_date.ReleaseCommand();
		}
	}

	for (int i = 0; i<vCD.size(); i++)
	{
		if (vCD[i].fields.action != 2) {
			CCommand<CAccessor<CCALENDARDATE_INSAccessor2>> cmd_date;
			cmd_date.Action = vCD[i].fields.action; // insert new values	 	
			cmd_date.m_fields.Date = vCD[i].fields.Date;
			cmd_date.m_fields.TradeIndicator = vCD[i].fields.TradeIndicator;
			cmd_date.m_fields.CalendarId = ReturnValue;


			HRESULT hr_date = cmd_date.Open(m_OLE_DB_Provider[strDbUser]);
			if (ISFAILED(hr_date, strErrDB, m_OLE_DB_Provider))
			{
				CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_CALENDARDATES, strErrDB, 0);
				return;
			}
			else
			{
				aRow.clear();
				aRow << std::string(QME::GQA_TABLENAME_CALENDARDATES);

				int nResult = 1;
				aRow << nResult;
				aRow << std::string("");
				aRow << std::string("");
				aRow << (nResult = -1);
				aRow << (nResult = cmd_date.m_fields.CalendarId);
			}
			cmd_date.Close();
			cmd_date.ReleaseCommand();
		}
	}
}

BOOL QmePlugin::IAUClients(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	CInfoQmeClient info;
	CDomainSerializer::FromAlert(&info, aRow);

	devent(LL_LOW, "Parameters:CLIENT_ID[%d],CLIENT_CODE[%s],FIRM_ID[%d],CLIENT_TYPE[%d],PERSON_ID[%d]",
		info.ClientId, info.ClientCode.c_str(), info.FirmId, info.ClientType, info.PersonId);

	CCommand< CAccessor<CCLIENT_INSAccessor> > cmd;
	COPY_INTO_FIELD_STDSTR(cmd.ClientCode, info.ClientCode);
	cmd.FirmId = info.FirmId;
	cmd.ClientType = info.ClientType;
	cmd.ClientId = info.ClientId;
	SET_NULLABLE_INT_PARAM(cmd.PersonId, cmd.PersonId_dbs, info.PersonId);

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_CLIENTS, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_CLIENTS);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.RetuenValue);
	}

	return TRUE;
}

BOOL QmePlugin::IAUTradeParams(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	int nFirmId, nClassId, nMode, nAccountId, nClientId, nParamId;

#ifdef _DEBUG
	DebugAlert(aRow);
#endif // _DEBUG

	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_TCP_COLUMN_FIRM_ID))
		{
			aField >> nFirmId;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_TCP_COLUMN_CLASS_ID))
		{
			aField >> nClassId;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_TCP_COLUMN_ACCOUNT_ID))
		{
			aField >> nAccountId;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_TCP_COLUMN_CLIENT_ID))
		{
			aField >> nClientId;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_TCP_COLUMN_MODE))
		{
			aField >> nMode;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_TCP_COLUMN_ID))
		{
			aField >> nParamId;
		}
	}

	devent(LL_LOW, "Parameters:PARAM_ID[%d],FIRM_ID[%d],CLASS_ID[%d],ACCOUNT_ID[%d],CLIENT_ID[%d],MODE[%d]"
		, nParamId, nFirmId, nClassId, nAccountId, nClientId, nMode);

	CCommand< CAccessor<CTCP_INSAccessor> > cmd;
	cmd.m_FIRM_ID = nFirmId;
	cmd.m_CLASS_ID = nClassId;
	cmd.m_MODE = nMode;
	cmd.m_CLIENT_ID = nClientId;
	cmd.m_PARAM_ID = nParamId;
	if (nClientId < 0)
	{
		cmd.m_dbsClientID = DBSTATUS_S_ISNULL;
	}
	cmd.m_ACCOUNT_ID = nAccountId;
	if (nAccountId < 0)
	{
		cmd.m_dbsAccountID = DBSTATUS_S_ISNULL;
	}

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_TRADE_PARAMS, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_TRADE_PARAMS);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::IAUAuctions(CAlert& aRow, const std::string& strDbUser)
{
		std::string strErrDB;

		CInfoQmeAuction info;
		CDomainSerializer::FromAlert(&info, aRow);

		devent(LL_LOW,
			"Updating %s... Parameters: SecurityId[%d], \
			IssuerId[%d], \
			BuySell[%d], \
			AuctionDate[%d], \
			OrderEntryPhaseStartTime[%d], \
			OrderEntryPhaseDuration[%d], \
			IssuerPhaseDuration[%d], \
			NumIssuerPhaseAutoProlongations[%d], \
			AuctionQty[%d], \
			MaxActiveOrdersPerFirm[%d], \
			OrderValueMultiplier[%d], \
			ParentAuctionId[%d], \
			AdditionalAuctionQtyPercent[%d]"
	, CInfoQmeAuction::TABLE_NAME.c_str()
	, info.SecurityId
	, info.IssuerId
	, info.BuySell
	, info.AuctionDate
	, info.OrderEntryPhaseStartTime
	, info.OrderEntryPhaseDuration
	, info.IssuerPhaseDuration
	, info.NumIssuerPhaseAutoProlongations
	, info.AuctionQty
	, info.MaxActiveOrdersPerFirm
	, info.OrderValueMultiplier
	, info.ParentAuctionId
	, info.AdditionalAuctionQtyPercent	
	);

		CCommand< CAccessor<CAuctionInsAccessor> > cmd;
		cmd.ConvertFrom(info);

		HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
		if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
		{
			CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_SECURITIES, strErrDB, 0);
			return FALSE;
		}
		else
		{
			aRow.clear();
			aRow << std::string(QME::GQA_TABLENAME_SECURITIES);

			int nResult = 1;
			aRow << nResult;
			aRow << std::string("");
			aRow << std::string("");
			aRow << (nResult = -1);
			aRow << (nResult = cmd.RETURN_VALUE);
		}

		return TRUE;
}


BOOL QmePlugin::IAUAuctions_del(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	CInfoQmeAuction info;
	CDomainSerializer::FromAlert(&info, aRow);

	devent(LL_LOW,
		"Updating %s... Parameters: SecurityId[%d], \
			IssuerId[%d], \
			BuySell[%d], \
			AuctionDate[%d], \
			OrderEntryPhaseStartTime[%d], \
			OrderEntryPhaseDuration[%d], \
			IssuerPhaseDuration[%d], \
			NumIssuerPhaseAutoProlongations[%d], \
			AuctionQty[%d], \
			MaxActiveOrdersPerFirm[%d], \
			OrderValueMultiplier[%d], \
			ParentAuctionId[%d], \
			AdditionalAuctionQtyPercent[%d]"
		, CInfoQmeAuction::TABLE_NAME.c_str()
		, info.SecurityId
		, info.IssuerId
		, info.BuySell
		, info.AuctionDate
		, info.OrderEntryPhaseStartTime
		, info.OrderEntryPhaseDuration
		, info.IssuerPhaseDuration
		, info.NumIssuerPhaseAutoProlongations
		, info.AuctionQty
		, info.MaxActiveOrdersPerFirm
		, info.OrderValueMultiplier
		, info.ParentAuctionId
		, info.AdditionalAuctionQtyPercent
	);

	CCommand< CAccessor<CAuctionInsAccessor> > cmd;
	cmd.ConvertFrom(info);
	cmd.Action = 2;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_SECURITIES,
			strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_SECURITIES);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::IAUConnection(CAlert& aRow)
{
	if (!CheckIniError())
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_CLASSES,
			"Error occurred when writing ini file. See server log for more details.", 0);
		return FALSE;
	}

	Library::Ini::CIniFile iniGate(m_dbINITIALIZE.ini_file_name_global); // 25 october 2019

	static const char sz_pt_connect[] = "pt_connect_";
	static const char sz_dis_pt_connect[] = "disabled_pt_connect_";

	int nEnabled(0);
	std::string strName;
	std::string strClasses;
	int nFix2Qme(0);
	int nTransCount(1);
	int nTransTimeout(15);
	std::string strSever("127.0.0.1");
	unsigned short nPort(0);

	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CONNECTIONS_COLUMN_NAME))
		{
			aField >> strName;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CONNECTIONS_COLUMN_CLASSES))
		{
			aField >> strClasses;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CONNECTIONS_COLUMN_FIX2QME))
		{
			aField >> nFix2Qme;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CONNECTIONS_COLUMN_TIMEOUT))
		{
			aField >> nTransTimeout;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CONNECTIONS_COLUMN_COUNT))
		{
			aField >> nTransCount;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CONNECTIONS_COLUMN_SERVER))
		{
			aField >> strSever;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CONNECTIONS_COLUMN_PORT))
		{
			aField >> nPort;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CONNECTIONS_COLUMN_ENABLED))
		{
			aField >> nEnabled;
		}
	}

	devent(LL_LOW, "Parameters:NAME[%s],CLASSES[%s],FIX2QME[%d],TIMEOUT[%d],COUNT[%d],SERVER[%s],PORT[%d],ENABLED[%d]"
		, strName.c_str(), strClasses.c_str(), nFix2Qme, nTransTimeout, nTransCount, strSever.c_str(), nPort, nEnabled);

	// remove all possible sections
	iniGate.RemoveSection(sz_pt_connect + strName);
	iniGate.RemoveSection(sz_dis_pt_connect + strName);

	std::string strSection = (nEnabled == 1 ? sz_pt_connect : sz_dis_pt_connect) + strName;
	iniGate.SetValue(strSection, "Classes", strClasses);
	iniGate.SetValue(strSection, GQA_TABLE_CONNECTIONS_COLUMN_FIX2QME, nFix2Qme);
	iniGate.SetValue(strSection, "trans_connection_count", nTransCount);
	iniGate.SetValue(strSection, "trans_connection_timeout", nTransTimeout);
	iniGate.SetValue(strSection, "server_ip_address", strSever);
	iniGate.SetValue(strSection, "port", nPort);
	iniGate.SetValue(strSection, "connection_transport", std::string("tcp"));

	aRow.clear();
	aRow << std::string(QME::GQA_TABLENAME_CONNECTIONS);
	int nResult = 1;
	aRow << nResult;
	aRow << std::string("");
	aRow << std::string("");
	aRow << (nResult = -1);
	aRow << (nResult = 0);

	return TRUE;
}

BOOL QmePlugin::IAUProperty(CAlert& aRow)
{
	if (!CheckIniError())
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_CLASSES,
			"Error occurred when writing ini file. See server log for more details.", 0);
		return FALSE;
	}

	Library::Ini::CIniFile iniGate(m_dbINITIALIZE.ini_file_name_global); // 25 october 2019

	std::string strName;
	std::string strValue;

	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_PROPS_COLUMN_NAME))
		{
			aField >> strName;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_PROPS_COLUMN_VALUE))
		{
			aField >> strValue;
		}
	}

	devent(LL_LOW, "Parameters:NAME[%s],VALUE[%s]", strName.c_str(), strValue.c_str());

	std::string strSection, strParam;
	// Parse value name for sections and parameter
	strSection = strName.substr(1, strName.find_first_of(']') - 1);
	strParam = strName.substr(strName.find_first_of('.') + 1);

	// set value
	iniGate.SetValue(strSection, strParam, strValue);

	aRow.clear();
	aRow << std::string(QME::GQA_TABLENAME_PROPS);
	int nResult = 1;
	aRow << nResult;
	aRow << std::string("");
	aRow << std::string("");
	aRow << (nResult = -1);
	aRow << (nResult = 0);

	return TRUE;
}

BOOL QmePlugin::CheckIniError()
{  
	Library::Ini::CIniFile iniGate(m_dbINITIALIZE.ini_file_name_global); // 25 october 2019

	bool bRes = iniGate.SetValue("GQA", "LastWrite", ::GetTickCount());
	if (!bRes)
	{
		DWORD dwError = GetLastError();
		DWORD dwFileAttr = ::GetFileAttributes(m_dbINITIALIZE.ini_file_name_global.c_str());
		if (INVALID_FILE_ATTRIBUTES == dwFileAttr)
		{
			devent(LL_HIGH, "Error occurred when writing [%s]. Can't find file.",
				m_dbINITIALIZE.ini_file_name_global.c_str());
		}
		else if (dwFileAttr & FILE_ATTRIBUTE_DIRECTORY)
		{
			devent(LL_HIGH, "Error occurred when writing [%s]. Not a file.",
				m_dbINITIALIZE.ini_file_name_global.c_str());
		}
		else if (dwFileAttr & FILE_ATTRIBUTE_READONLY)
		{
			devent(LL_HIGH, "Error occurred when writing [%s]. File is read only.",
				m_dbINITIALIZE.ini_file_name_global.c_str());
		}
		else
		{
			devent(LL_HIGH, "Error occurred when writing [%s]. Error code: [%d]. File attributes [%d].",
				m_dbINITIALIZE.ini_file_name_global.c_str(), dwError, dwFileAttr);
		}

		return FALSE;
	}

	return TRUE;
}

BOOL QmePlugin::SetIniSDPClass(bool bSet, int nContractType, const std::string& strClassName)
{
	return SetClassesBySection(bSet, nContractType, strClassName, "Classes", m_mSDPPath);
}

BOOL QmePlugin::SetIniTransClass(bool bSet, int nContractType, const std::string& strClassName)
{
	return SetClassesBySection(bSet, nContractType, strClassName, "TransClasses", m_mTransPath);
}

BOOL QmePlugin::SetClassesBySection(bool bSet, int nContractType, const std::string& strClassName
	, std::string strSection, ClassDescPathMap& mPathes)
{
	Library::Ini::CIniFile iniGate(m_dbINITIALIZE.ini_file_name_global); // 25 october 2019

	if (bSet)
	{
		bool bRes(false);
		std::string strPath;
		bool isClassesSection = strSection.compare("Classes") == 0;
		bool isTransClassesSection = strSection.compare("TransClasses") == 0;

		if (!isClassesSection && !isTransClassesSection)
		{
		if (mPathes.find(nContractType) != mPathes.end())
		{
			strPath = mPathes[nContractType];
		}
		else
		{
			// error - path for this instrument type not set
			devent(LL_HIGH, "Path for contracts type [%s] not set in config file [%s].",
				astrContractTypes[nContractType].c_str(), m_dbINITIALIZE.ini_file_name_global.c_str());
		}
		}

		if (!strPath.empty() || (!isClassesSection || !isTransClassesSection))
			bRes = iniGate.SetValue(strSection, strClassName, strPath);

		if (!bRes && (!isClassesSection && !isTransClassesSection))
		{
			DWORD dwError = GetLastError();
			devent(LL_HIGH, "Error occurred when writing [%s]. Error code: [%d]",
				m_dbINITIALIZE.ini_file_name_global.c_str(), dwError);
			return FALSE;
		}

		return TRUE;
	}
	else
	{
		bool bRes = iniGate.RemoveValue(strSection, strClassName);
		if (!bRes)
		{
			DWORD dwError = GetLastError();
			devent(LL_HIGH, "Error occurred when writing [%s]. Error code: [%d]",
				m_dbINITIALIZE.ini_file_name_global.c_str(), dwError);
			return FALSE;
		}

		return TRUE;
	}
}

BOOL QmePlugin::UpdateTableImpl(CAlert& aKeyRow, CAlert& aNewRow, const std::string& strTableName)
{
	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_SCHEDULE))
	{
		std::string db_User;
		if (!CheckConnection(db_User))
		{
			aNewRow.clear();
			int err = 3; // no db connectivity
			aNewRow << std::string("UpdateTable ") << std::string(strTableName) << err;
			return FALSE;
		}

		return UpdateSchedule(aKeyRow, aNewRow, db_User);
	}	
	return InsertAndUpdateTable(aNewRow, strTableName, CrudActions::CRUD_UPDATE);
}

BOOL QmePlugin::InsertIntoTableImpl(CAlert& aKeyRow, CAlert& aNewRow, const std::string& strTableName)
{
	return InsertAndUpdateTable(aNewRow, strTableName, CrudActions::CRUD_CREATE);
}

BOOL QmePlugin::DeleteFromTableImpl(CAlert& aDummyRow, CAlert& aRow, const std::string& strTableName)
{
	std::string db_User, strErrDB;
	if (!CheckConnection(db_User))
	{
		aRow.clear();
		int err = 3; // no db connectivity
		aRow << std::string("DeleteTable ") << std::string(strTableName) << err; // << LoadResourseString(DONT_CONNECT_DB, UserLocale(user_id));
		return FALSE;
	}

	devent(LL_HIGH, "Delete from table %s", strTableName.c_str());


	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CLASSES))
	{
		return DeleteClass(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_ACCOUNTS))
	{
		return DeleteAccount(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CALENDARS))
	{
		return DeleteCalendar(aRow, db_User);
	}


	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CLIENTS))
	{
		return DeleteClient(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_FIRMS))
	{
		return DeleteFirm(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CALENDARS))
	{
		return DeleteFirm(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_SECURITIES))
	{
		return DeleteSecurity(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_TAGS))
	{
		return DeleteTag(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CORE_GROUPS))
	{
		return DeleteGroup(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_SETTLE_CODES))
	{
		return DeleteSettleCode(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_SCHEDULE))
	{
		return DeleteSchedule(aRow, db_User);
	}
	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_AUCTIONS))
	{
		return IAUAuctions_del(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_TRADE_PARAMS))
	{
		return DeleteTradeParam(aRow, db_User);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_CONNECTIONS))
	{
		return DeleteConnection(aRow);
	}

	if (Library::Utils::StringNoCaseEq(strTableName, QME::GQA_TABLENAME_PROPS))
	{
		return DeleteProperty(aRow);
	}
	

	if (Library::Utils::StringNoCaseEq(strTableName, CInfoQmeCurrency::TABLE_NAME)
		|| Library::Utils::StringNoCaseEq(strTableName, CInfoQmePerson::TABLE_NAME)
		|| Library::Utils::StringNoCaseEq(strTableName, CInfoQmeCoupon::TABLE_NAME)
		|| Library::Utils::StringNoCaseEq(strTableName, CInfoQmeExtraClassParam::TABLE_NAME)
		|| Library::Utils::StringNoCaseEq(strTableName, CInfoQmeMarkets::TABLE_NAME)
		)
	{
		return InsertAndUpdateTable(aRow, strTableName, CrudActions::CRUD_DELETE);
	}

	CreateCommonError(aRow, strTableName, "Unknown Table");
	return FALSE;
}

BOOL QmePlugin::DeleteFirm(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	int nFirmID;
	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_FIRMS_COLUMN_FIRM_ID))
		{
			aField >> nFirmID;
		}
	}

	devent(LL_LOW, "Parameters:FIRM_ID[%d]", nFirmID);

	CCommand< CAccessor<CFIRM_DELAccessor> > cmd;
	cmd.m_FIRM_ID = nFirmID;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_DELETE_FROM_TABLE, QME::GQA_TABLENAME_FIRMS, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_FIRMS);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::DeleteSecurity(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;
	CInfoQmeSecurity deletingObject;
	CDomainSerializer::FromAlert(&deletingObject, aRow);
	devent(LL_LOW, "Deleting %s... Parameters: SecurityId[%d], SecCode[%s]", deletingObject.TABLE_NAME.c_str()
		, deletingObject.SecurityId, deletingObject.SecCode.c_str());

	CCommand< CAccessor<CSecurityDelAccessor> > cmd;
	cmd.m_SECURITY_ID = deletingObject.SecurityId;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_DELETE_FROM_TABLE, QME::GQA_TABLENAME_SECURITIES, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << deletingObject.TABLE_NAME;

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::DeleteAccount(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	int nID;
	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_ACCOUNTS_COLUMN_ACCOUNT_ID))
		{
			aField >> nID;
		}
	}

	devent(LL_LOW, "Parameters:ACCOUNT_ID[%d]", nID);

	CCommand< CAccessor<CACCOUNT_DELAccessor> > cmd;
	cmd.m_ACCOUNT_ID = nID;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_DELETE_FROM_TABLE, QME::GQA_TABLENAME_ACCOUNTS, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_ACCOUNTS);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::DeleteTag(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	int nID;
	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_TAGS_COLUMN_TAG_ID))
		{
			aField >> nID;
		}
	}

	devent(LL_LOW, "Parameters:TAG_ID[%d]", nID);

	CCommand< CAccessor<CTAG_DELAccessor> > cmd;
	cmd.m_TAG_ID = nID;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_DELETE_FROM_TABLE, QME::GQA_TABLENAME_TAGS, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_TAGS);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::DeleteGroup(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	int nID;
	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CG_COLUMN_GROUP_ID))
		{
			aField >> nID;
		}
	}

	devent(LL_LOW, "Parameters:GROUP_ID[%d]", nID);

	CCommand< CAccessor<CGROUP_DELAccessor> > cmd;
	cmd.m_GROUP_ID = nID;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_DELETE_FROM_TABLE, QME::GQA_TABLENAME_CORE_GROUPS, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_CORE_GROUPS);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::DeleteSettleCode(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	int nID;
	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_SC_COLUMN_SETTLE_CODE_ID))
		{
			aField >> nID;
		}
	}

	devent(LL_LOW, "Parameters:SETTLE_CODE_ID[%d]", nID);

	CCommand< CAccessor<CSETTLEDAY_DELAccessor> > cmd;
	cmd.m_SETTLE_ID = nID;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_DELETE_FROM_TABLE, QME::GQA_TABLENAME_SETTLE_CODES, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_SETTLE_CODES);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::DeleteSchedule(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	int nID;
	std::string strEventTime;

	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_SCHEDULE_COLUMN_CLASS_ID))
		{
			aField >> nID;
		}
		else if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_SCHEDULE_COLUMN_EVENT_TIME))
		{
			aField >> strEventTime;
		}
	}

	devent(LL_LOW, "Parameters:CLASS_ID[%d],EVENT_TIME[%s]", nID, strEventTime.c_str());

	CCommand< CAccessor<CSCHEDULE_DELAccessor> > cmd;
	cmd.m_CLASS_ID = nID;
	COPY_INTO_FIELD(cmd.m_EVENT_TIME, strEventTime.c_str());

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_DELETE_FROM_TABLE, QME::GQA_TABLENAME_SCHEDULE, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_SCHEDULE);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::DeleteCalendar(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	int nID;
	CAlert aCDates;
	ParamDbQMECalendar m_fields = {};
	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CALENDARS_COLUMN_CALENDAR_ID))
		{
			aField >> m_fields.CalendarId;
		}		
	}



	devent(LL_LOW, "Parameters:CALENDAR_ID[%d]", m_fields.CalendarId);

	CCommand< CAccessor<CCALENDAR_INSAccessor2> > cmd;
	cmd.Action = 2; // delete record from Calendars
	cmd.m_fields.CalendarId = m_fields.CalendarId;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_DELETE_FROM_TABLE, QME::GQA_TABLENAME_CALENDARS, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_CALENDARS);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_fields.CalendarId);
	}


	

	return TRUE;
}




BOOL QmePlugin::DeleteClient(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	int nID;
	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CLIENTS_COLUMN_CLIENT_ID))
		{
			aField >> nID;
		}
	}

	devent(LL_LOW, "Parameters:CLIENT_ID[%d]", nID);

	CCommand< CAccessor<CCLIENT_DELAccessor> > cmd;
	cmd.ClientId = nID;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_DELETE_FROM_TABLE, QME::GQA_TABLENAME_CLIENTS, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_CLIENTS);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.ReturnValue);
	}

	return TRUE;
}

BOOL QmePlugin::DeleteClass(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	CInfoQmeClass theClass;
	CDomainSerializer::FromAlert(&theClass, aRow);

	devent(LL_LOW, "Deleting %s... Parameters: CLASS_ID[%d], CLASS_CODE[%s]", theClass.TABLE_NAME.c_str(), theClass.ClassId, theClass.ClassCode.c_str());

	CCommand< CAccessor<CCLASS_DELAccessor> > cmd;
	cmd.m_CLASS_ID = theClass.ClassId;

	if (!CheckIniError())
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_CLASSES,
			"Error occurred when writing ini file. See server log for more details.", 0);
		return FALSE;
	}

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_DELETE_FROM_TABLE, QME::GQA_TABLENAME_CLASSES, strErrDB, 0);
		return FALSE;
	}
	else
	{
		SetIniSDPClass(false, 0, theClass.ClassCode); // m_dbINITIALIZE.ini_file_name_global // 25 october 2019
		SetIniTransClass(false, 0, theClass.ClassCode);

		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_CLASSES);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::DeleteConnection(CAlert& aRow)
{
	if (!CheckIniError())
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE, QME::GQA_TABLENAME_CLASSES,
			"Error occurred when writing ini file. See server log for more details.", 0);
		return FALSE;
	}

	Library::Ini::CIniFile iniGate(m_dbINITIALIZE.ini_file_name_global); // 25 october 2019

	// find the [pt_connect_<ConnName>] or [disabled_pt_connect_<>] sections
	static const char sz_pt_connect[] = "pt_connect_";
	static const char sz_dis_pt_connect[] = "disabled_pt_connect_";

	std::string strName;

	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_CONNECTIONS_COLUMN_NAME))
		{
			aField >> strName;
		}
	}

	// remove all possible sections
	iniGate.RemoveSection(sz_pt_connect + strName);
	iniGate.RemoveSection(sz_dis_pt_connect + strName);

	aRow.clear();
	aRow << std::string(QME::GQA_TABLENAME_CONNECTIONS);
	int nResult = 1;
	aRow << nResult;
	aRow << std::string("");
	aRow << std::string("");
	aRow << (nResult = -1);
	aRow << (nResult = 0);

	return TRUE;
}

BOOL QmePlugin::DeleteProperty(CAlert& aRow)
{
//	Library::Ini::CIniFile iniGate(m_dbINITIALIZE.ini_file_name_global);

	aRow.clear();
	aRow << std::string(QME::GQA_TABLENAME_PROPS);
	int nResult = 0;
	aRow << nResult;
	aRow << std::string("Can't delete any property");
	aRow << std::string("");
	aRow << (nResult = -1);
	aRow << (nResult = -1);

	return FALSE;
}

BOOL QmePlugin::DeleteTradeParam(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;

	int nID;
	while (aRow.get_fields_count())
	{
		CAlert aField;
		aRow >> aField;

		std::string strFieldName;
		aField >> strFieldName;
		if (Library::Utils::StringNoCaseEq(strFieldName, QME::GQA_TABLE_TCP_COLUMN_ID))
		{
			aField >> nID;
		}
	}

	devent(LL_LOW, "Parameters:PARAM_ID[%d]", nID);

	CCommand< CAccessor<CTCP_DELAccessor> > cmd;
	cmd.m_PARAM_ID = nID;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_DELETE_FROM_TABLE, QME::GQA_TABLENAME_TRADE_PARAMS, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << std::string(QME::GQA_TABLENAME_TRADE_PARAMS);

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}

	return TRUE;
}

BOOL QmePlugin::DeleteAuction(CAlert& aRow, const std::string& strDbUser)
{
	std::string strErrDB;
	CInfoQmeAuction deletingObject;
	CDomainSerializer::FromAlert(&deletingObject, aRow);
	devent(LL_LOW, "Deleting %s... Parameters: AuctionId[%d]", deletingObject.TABLE_NAME.c_str()
		, deletingObject.AuctionId);

	CCommand< CAccessor<CAuctionDelAccessor> > cmd;
	cmd.m_AUCTION_ID = deletingObject.AuctionId;

	HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
	if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider))
	{
		CreateDbError(aRow, PCORE::GQA_FUNCTION_DELETE_FROM_TABLE, QME::GQA_TABLENAME_SECURITIES, strErrDB, 0);
		return FALSE;
	}
	else
	{
		aRow.clear();
		aRow << deletingObject.TABLE_NAME;

		int nResult = 1;
		aRow << nResult;
		aRow << std::string("");
		aRow << std::string("");
		aRow << (nResult = -1);
		aRow << (nResult = cmd.m_RETURN_VALUE);
	}

	return TRUE;
}