
#ifndef __DBA___SECURITIES__H__INCLUDED__
#define __DBA___SECURITIES__H__INCLUDED__

#pragma once

//**********************************************************************************

class CSecurityParams {
 public:
  CSecurityParams() { memset(this, 0, sizeof(*this)); }

  INT RETURN_VALUE;

  INT ClassId;
  TCHAR SecCode[13];
  TCHAR ShortNameRus[33];
  TCHAR FullNameRus[129];
  TCHAR ShortNameEng[33];
  TCHAR FullNameEng[129];
  TCHAR ISIN[13];
  TCHAR BaseClassCode[13];
  TCHAR BaseSecCode[13];
  INT TradeCurrencyId;
  INT BaseCurrencyId;
  INT TermCurrencyId;
  INT FaceUnitId;
  INT MinStep;
  SHORT Scale;
  INT MatDate;
  INT LotSize;
  INT GroupId;
  INT CalendarId;
  INT SettleCodeId;
  INT Enabled;
  LONGLONG ClPrice;
  LONGLONG SellDepo;
  LONGLONG BuyDepo;
  LONGLONG StepPrice;
  FLOAT FaceValue;
  LONGLONG PrevPrice;
  INT BasisType;
  TCHAR CFI[7];
  TCHAR RegNumber[31];
  INT SecurityId;
  INT ListLevel;
  INT SubType;
  TCHAR StockCode[13];
  TCHAR StockName[33];
  TCHAR SedolCode[8];
  TCHAR RicCode[33];
  TCHAR CusipCode[10];
  TCHAR FigiCode[21];
  SHORT QtyScale;
  LONGLONG QtyMultiplier;
  INT Ccy1QtyScaleEnabled;
  INT MarketId;
  LONGLONG StrikePrice;
  TCHAR OptionKind[2];
  TCHAR OptionType[2];
  TCHAR OptionSettleType[2];
  INT BondInterestType;

  DBSTATUS ShortNameRus_dbs, FullNameRus_dbs, ShortNameEng_dbs, FullNameEng_dbs,
      ISIN_dbs, FaceUnitId_dbs, MatDate_dbs, BaseClassCode_dbs, BaseSecCode_dbs,
      TradeCurrencyId_dbs, CalendarId_dbs, SettleCodeId_dbs, BaseCurrencyId_dbs,
      TermCurrencyId_dbs, ClPrice_dbs, SellDepo_dbs, BuyDepo_dbs, StepPrice_dbs,
      PrevPrice_dbs, BasisType_dbs, CFI_dbs, RegNumber_dbs, ListLevel_dbs,
      SubType_dbs, StockCode_dbs, StockName_dbs, SedolCode_dbs, RicCode_dbs,
      CusipCode_dbs, FigiCode_dbs, QtyScale_dbs, QtyMultiplier_dbs,
      Ccy1QtyScaleEnabled_dbs, MarketId_dbs, OptionKind_dbs, OptionType_dbs,
      OptionSettleType_dbs, BondInterestType_dbs;

  void ConvertFrom(const QME::CInfoQmeSecurity& info) {
    this->SecurityId = info.SecurityId;
    this->ClassId = info.ClassId;
    this->GroupId = info.GroupId;

    SET_NULLABLE_INT_PARAM(this->CalendarId, this->CalendarId_dbs,
                           info.CalendarId);
    SET_NULLABLE_INT_PARAM(this->SettleCodeId, this->SettleCodeId_dbs,
                           info.SettleCodeId);

    COPY_INTO_FIELD_STDSTR(this->SecCode, info.SecCode);
    SET_NULLABLE_STR_PARAM(this->ShortNameRus, this->ShortNameRus_dbs,
                           info.ShortNameRus);
    SET_NULLABLE_STR_PARAM(this->FullNameRus, this->FullNameRus_dbs,
                           info.FullNameRus);
    SET_NULLABLE_STR_PARAM(this->ShortNameEng, this->ShortNameEng_dbs,
                           info.ShortNameEng);
    SET_NULLABLE_STR_PARAM(this->FullNameEng, this->FullNameEng_dbs,
                           info.FullNameEng);

    this->MinStep = info.MinStep;
    SET_NULLABLE_INT_PARAM(this->FaceUnitId, this->FaceUnitId_dbs,
                           info.FaceUnitId);
    this->FaceValue = info.FaceValue;
    this->Scale = info.Scale;
    SET_NULLABLE_INT_PARAM(this->MatDate, this->MatDate_dbs, info.MatDate);
    this->LotSize = info.LotSize;
    SET_NULLABLE_STR_PARAM(this->ISIN, this->ISIN_dbs, info.ISIN);
    SET_NULLABLE_STR_PARAM(this->CFI, this->CFI_dbs, info.CFI);
    SET_NULLABLE_STR_PARAM(this->RegNumber, this->RegNumber_dbs,
                           info.RegNumber);
    SET_NULLABLE_STR_PARAM(this->BaseClassCode, this->BaseClassCode_dbs,
                           info.BaseClassCode);
    SET_NULLABLE_STR_PARAM(this->BaseSecCode, this->BaseSecCode_dbs,
                           info.BaseSecCode);

    SET_NULLABLE_INT_PARAM(this->TradeCurrencyId, this->TradeCurrencyId_dbs,
                           info.TradeCurrencyId);
    SET_NULLABLE_INT_PARAM(this->BaseCurrencyId, this->BaseCurrencyId_dbs,
                           info.BaseCurrencyId);
    SET_NULLABLE_INT_PARAM(this->TermCurrencyId, this->TermCurrencyId_dbs,
                           info.TermCurrencyId);
    SET_NULLABLE_INT_PARAM(this->BasisType, this->BasisType_dbs,
                           info.BasisType);
    SET_NULLABLE_INT_PARAM(this->PrevPrice, this->PrevPrice_dbs,
                           info.PrevPrice);
    SET_NULLABLE_INT_PARAM(this->ClPrice, this->ClPrice_dbs, info.ClPrice);
    SET_NULLABLE_INT_PARAM(this->SellDepo, this->SellDepo_dbs, info.SellDepo);
    SET_NULLABLE_INT_PARAM(this->BuyDepo, this->BuyDepo_dbs, info.BuyDepo);
    this->StepPrice = info.StepPrice;
    SET_NULLABLE_INT_PARAM(this->ListLevel, this->ListLevel_dbs,
                           info.ListLevel);
    this->Enabled = info.Enabled;

    SET_NULLABLE_INT_PARAM_WITH_SPECIAL_NULL_VALUE(
        this->SubType, this->SubType_dbs, info.SubType,
        QME::CInfoQmeSecurity::SUBTYPE_NULL_VALUE);

    SET_NULLABLE_STR_PARAM(this->StockCode, this->StockCode_dbs,
                           info.StockCode);
    SET_NULLABLE_STR_PARAM(this->StockName, this->StockName_dbs,
                           info.StockName);
    SET_NULLABLE_STR_PARAM(this->SedolCode, this->SedolCode_dbs,
                           info.SedolCode);
    SET_NULLABLE_STR_PARAM(this->RicCode, this->RicCode_dbs, info.RicCode);
    SET_NULLABLE_STR_PARAM(this->CusipCode, this->CusipCode_dbs,
                           info.CusipCode);
    SET_NULLABLE_STR_PARAM(this->FigiCode, this->FigiCode_dbs, info.FigiCode);
    this->QtyScale = info.QtyScale;
    this->QtyMultiplier = info.QtyMultiplier;
    SET_NULLABLE_INT_PARAM(this->Ccy1QtyScaleEnabled,
                           this->Ccy1QtyScaleEnabled_dbs,
                           info.Ccy1QtyScaleEnabled);
    SET_NULLABLE_INT_PARAM_WITH_SPECIAL_NULL_VALUE(
        this->MarketId, this->MarketId_dbs, info.MarketId,
        QME::CInfoQmeSecurity::MARKET_INHERIT_FROM_CLASS);

    this->StrikePrice = info.StrikePrice;
    this->OptionKind[0] = info.OptionKind;
    this->OptionType[0] = info.OptionType;
    this->OptionSettleType[0] = info.OptionSettleType;
    SET_NULLABLE_INT_PARAM(this->BondInterestType, this->BondInterestType_dbs,
                           info.BondInterestType);
  }

  void ConvertTo(QME::CInfoQmeSecurity& info) const {
    info.SecurityId = this->SecurityId;
    info.ClassId = this->ClassId;
    info.GroupId = this->GroupId;

    GET_NULLABLE_INT_PARAM(this->CalendarId, this->CalendarId_dbs,
                           info.CalendarId);
    GET_NULLABLE_INT_PARAM(this->SettleCodeId, this->SettleCodeId_dbs,
                           info.SettleCodeId);

    info.SecCode = this->SecCode, info.SecCode;
    GET_NULLABLE_STR_PARAM(this->ShortNameRus, this->ShortNameRus_dbs,
                           info.ShortNameRus);
    GET_NULLABLE_STR_PARAM(this->FullNameRus, this->FullNameRus_dbs,
                           info.FullNameRus);
    GET_NULLABLE_STR_PARAM(this->ShortNameEng, this->ShortNameEng_dbs,
                           info.ShortNameEng);
    GET_NULLABLE_STR_PARAM(this->FullNameEng, this->FullNameEng_dbs,
                           info.FullNameEng);

    info.MinStep = this->MinStep;
    GET_NULLABLE_INT_PARAM(this->FaceUnitId, this->FaceUnitId_dbs,
                           info.FaceUnitId);
    info.FaceValue = this->FaceValue;
    info.Scale = this->Scale;
    GET_NULLABLE_INT_PARAM(this->MatDate, this->MatDate_dbs, info.MatDate);
    info.LotSize = this->LotSize;
    GET_NULLABLE_STR_PARAM(this->ISIN, this->ISIN_dbs, info.ISIN);
    GET_NULLABLE_STR_PARAM(this->CFI, this->CFI_dbs, info.CFI);
    GET_NULLABLE_STR_PARAM(this->RegNumber, this->RegNumber_dbs,
                           info.RegNumber);
    GET_NULLABLE_STR_PARAM(this->BaseClassCode, this->BaseClassCode_dbs,
                           info.BaseClassCode);
    GET_NULLABLE_STR_PARAM(this->BaseSecCode, this->BaseSecCode_dbs,
                           info.BaseSecCode);

    GET_NULLABLE_INT_PARAM(this->TradeCurrencyId, this->TradeCurrencyId_dbs,
                           info.TradeCurrencyId);
    GET_NULLABLE_INT_PARAM(this->BaseCurrencyId, this->BaseCurrencyId_dbs,
                           info.BaseCurrencyId);
    GET_NULLABLE_INT_PARAM(this->TermCurrencyId, this->TermCurrencyId_dbs,
                           info.TermCurrencyId);
    GET_NULLABLE_INT_PARAM(this->BasisType, this->BasisType_dbs,
                           info.BasisType);
    GET_NULLABLE_INT_PARAM(this->PrevPrice, this->PrevPrice_dbs,
                           info.PrevPrice);
    GET_NULLABLE_INT_PARAM(this->ClPrice, this->ClPrice_dbs, info.ClPrice);
    GET_NULLABLE_INT_PARAM(this->SellDepo, this->SellDepo_dbs, info.SellDepo);
    GET_NULLABLE_INT_PARAM(this->BuyDepo, this->BuyDepo_dbs, info.BuyDepo);
    info.StepPrice = this->StepPrice;
    GET_NULLABLE_INT_PARAM(this->ListLevel, this->ListLevel_dbs,
                           info.ListLevel);
    info.Enabled = (this->Enabled != 0);

    GET_NULLABLE_INT_PARAM_WITH_SPECIAL_NULL_VALUE(
        this->SubType, this->SubType_dbs, info.SubType,
        QME::CInfoQmeSecurity::SUBTYPE_NULL_VALUE);

    GET_NULLABLE_STR_PARAM(this->StockCode, this->StockCode_dbs,
                           info.StockCode);
    GET_NULLABLE_STR_PARAM(this->StockName, this->StockName_dbs,
                           info.StockName);
    GET_NULLABLE_STR_PARAM(this->SedolCode, this->SedolCode_dbs,
                           info.SedolCode);
    GET_NULLABLE_STR_PARAM(this->RicCode, this->RicCode_dbs, info.RicCode);
    GET_NULLABLE_STR_PARAM(this->CusipCode, this->CusipCode_dbs,
                           info.CusipCode);
    GET_NULLABLE_STR_PARAM(this->FigiCode, this->FigiCode_dbs, info.FigiCode);
    info.QtyScale = this->QtyScale;
    info.QtyMultiplier = this->QtyMultiplier;
    GET_NULLABLE_INT_PARAM(this->Ccy1QtyScaleEnabled,
                           this->Ccy1QtyScaleEnabled_dbs,
                           info.Ccy1QtyScaleEnabled);
    GET_NULLABLE_INT_PARAM_WITH_SPECIAL_NULL_VALUE(
        this->MarketId, this->MarketId_dbs, info.MarketId,
        QME::CInfoQmeSecurity::MARKET_INHERIT_FROM_CLASS);

    info.StrikePrice = this->StrikePrice;
    info.OptionKind =
        this->OptionKind_dbs != DBSTATUS_S_ISNULL ? this->OptionKind[0] : 0;
    info.OptionType =
        this->OptionType_dbs != DBSTATUS_S_ISNULL ? this->OptionType[0] : 0;
    info.OptionSettleType = this->OptionSettleType_dbs != DBSTATUS_S_ISNULL
                                ? this->OptionSettleType[0]
                                : 0;
    GET_NULLABLE_INT_PARAM(this->BondInterestType, this->BondInterestType_dbs,
                           info.BondInterestType);
  }
};

//**********************************************************************************

class CSecurityGetAccessor : public CSecurityParams {
 public:
  DEFINE_COMMAND_EX(CSecurityGetAccessor, L"EXEC GQA_GetAllSecurities")

  BEGIN_COLUMN_MAP(CSecurityGetAccessor)
  COLUMN_ENTRY(1, SecurityId)
  COLUMN_ENTRY(2, ClassId)
  COLUMN_ENTRY(3, SecCode)
  COLUMN_ENTRY_STATUS(4, ShortNameRus, ShortNameRus_dbs)
  COLUMN_ENTRY_STATUS(5, FullNameRus, FullNameRus_dbs)
  COLUMN_ENTRY_STATUS(6, ShortNameEng, ShortNameEng_dbs)
  COLUMN_ENTRY_STATUS(7, FullNameEng, FullNameEng_dbs)
  COLUMN_ENTRY(8, MinStep)
  COLUMN_ENTRY_STATUS(9, FaceUnitId, FaceUnitId_dbs)
  COLUMN_ENTRY(10, FaceValue)
  COLUMN_ENTRY(11, Scale)
  COLUMN_ENTRY_STATUS(12, MatDate, MatDate_dbs)
  COLUMN_ENTRY(13, LotSize)
  COLUMN_ENTRY_STATUS(14, ISIN, ISIN_dbs)
  COLUMN_ENTRY_STATUS(15, RegNumber, RegNumber_dbs)
  COLUMN_ENTRY_STATUS(16, PrevPrice, PrevPrice_dbs)
  COLUMN_ENTRY(17, GroupId)
  COLUMN_ENTRY_STATUS(18, CalendarId, CalendarId_dbs)
  COLUMN_ENTRY_STATUS(19, SettleCodeId, SettleCodeId_dbs)
  COLUMN_ENTRY_STATUS(20, ClPrice, ClPrice_dbs)
  COLUMN_ENTRY_STATUS(21, SellDepo, SellDepo_dbs)
  COLUMN_ENTRY_STATUS(22, BuyDepo, BuyDepo_dbs)
  COLUMN_ENTRY(23, StepPrice)
  COLUMN_ENTRY_STATUS(24, BaseClassCode, BaseClassCode_dbs)
  COLUMN_ENTRY_STATUS(25, BaseSecCode, BaseSecCode_dbs)
  COLUMN_ENTRY_STATUS(26, TradeCurrencyId, TradeCurrencyId_dbs)
  COLUMN_ENTRY_STATUS(27, BaseCurrencyId, BaseCurrencyId_dbs)
  COLUMN_ENTRY_STATUS(28, TermCurrencyId, TermCurrencyId_dbs)
  COLUMN_ENTRY_STATUS(29, BasisType, BasisType_dbs)
  COLUMN_ENTRY_STATUS(30, CFI, CFI_dbs)
  COLUMN_ENTRY_STATUS(31, ListLevel, ListLevel_dbs)
  COLUMN_ENTRY(32, Enabled)
  COLUMN_ENTRY_STATUS(33, SubType, SubType_dbs)
  COLUMN_ENTRY_STATUS(34, StockCode, StockCode_dbs)
  COLUMN_ENTRY_STATUS(35, StockName, StockName_dbs)
  COLUMN_ENTRY_STATUS(36, SedolCode, SedolCode_dbs)
  COLUMN_ENTRY_STATUS(37, RicCode, RicCode_dbs)
  COLUMN_ENTRY_STATUS(38, CusipCode, CusipCode_dbs)
  COLUMN_ENTRY_STATUS(39, FigiCode, FigiCode_dbs)
  COLUMN_ENTRY(40, QtyScale)
  COLUMN_ENTRY(41, QtyMultiplier)
  COLUMN_ENTRY_STATUS(42, Ccy1QtyScaleEnabled, Ccy1QtyScaleEnabled_dbs)
  COLUMN_ENTRY_STATUS(43, MarketId, MarketId_dbs)
  COLUMN_ENTRY(44, StrikePrice);
  COLUMN_ENTRY_STATUS(45, OptionKind, OptionKind_dbs);
  COLUMN_ENTRY_STATUS(46, OptionType, OptionType_dbs);
  COLUMN_ENTRY_STATUS(47, OptionSettleType, OptionSettleType_dbs);
  COLUMN_ENTRY_STATUS(48, BondInterestType, BondInterestType_dbs)
  END_COLUMN_MAP()
};

//**********************************************************************************

class CSecurityInsAccessor : public CSecurityParams {
 public:
  DEFINE_COMMAND_EX(CSecurityInsAccessor,
                    L"{ ? = CALL dbo.GQA_NewSecurity( \
			?, ?, ?, ?, ?, ?, ?, ?, ?, ?, \
			?, ?, ?, ?, ?, ?, ?, ?, ?, ?, \
			?, ?, ?, ?, ?, ?, ?, ?, ?, ?, \
			?, ?, ?, ?, ?, ?, ?, ?, ?, ?, \
			?, ?, ?, ?, ?, ?, ?, ?) }");

  BEGIN_PARAM_MAP(CSecurityInsAccessor)

  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, RETURN_VALUE)

  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, ClassId)
  COLUMN_ENTRY(3, SecCode)
  COLUMN_ENTRY(4, GroupId)
  COLUMN_ENTRY_STATUS(5, ShortNameRus, ShortNameRus_dbs)
  COLUMN_ENTRY_STATUS(6, FullNameRus, FullNameRus_dbs)
  COLUMN_ENTRY_STATUS(7, ShortNameEng, ShortNameEng_dbs)
  COLUMN_ENTRY_STATUS(8, FullNameEng, FullNameEng_dbs)
  COLUMN_ENTRY_STATUS(9, ISIN, ISIN_dbs)
  COLUMN_ENTRY(10, MinStep)
  COLUMN_ENTRY(11, FaceValue)
  COLUMN_ENTRY_STATUS(12, FaceUnitId, FaceUnitId_dbs)
  COLUMN_ENTRY(13, Scale)
  COLUMN_ENTRY_STATUS(14, MatDate, MatDate_dbs)
  COLUMN_ENTRY(15, LotSize)
  COLUMN_ENTRY_STATUS(16, BaseClassCode, BaseClassCode_dbs)
  COLUMN_ENTRY_STATUS(17, BaseSecCode, BaseSecCode_dbs)
  COLUMN_ENTRY_STATUS(18, TradeCurrencyId, TradeCurrencyId_dbs)
  COLUMN_ENTRY_STATUS(19, CalendarId, CalendarId_dbs)
  COLUMN_ENTRY_STATUS(20, SettleCodeId, SettleCodeId_dbs)
  COLUMN_ENTRY_STATUS(21, BaseCurrencyId, BaseCurrencyId_dbs)
  COLUMN_ENTRY_STATUS(22, TermCurrencyId, TermCurrencyId_dbs)
  COLUMN_ENTRY(23, Enabled)
  COLUMN_ENTRY_STATUS(24, ClPrice, ClPrice_dbs)
  COLUMN_ENTRY_STATUS(25, SellDepo, SellDepo_dbs)
  COLUMN_ENTRY_STATUS(26, BuyDepo, BuyDepo_dbs)
  COLUMN_ENTRY(27, StepPrice)
  COLUMN_ENTRY_STATUS(28, PrevPrice, PrevPrice_dbs)
  COLUMN_ENTRY_STATUS(29, BasisType, BasisType_dbs)
  COLUMN_ENTRY_STATUS(30, CFI, CFI_dbs)
  COLUMN_ENTRY_STATUS(31, ListLevel, ListLevel_dbs)
  COLUMN_ENTRY_STATUS(32, RegNumber, RegNumber_dbs)
  COLUMN_ENTRY_STATUS(33, SubType, SubType_dbs)
  COLUMN_ENTRY(34, SecurityId)
  COLUMN_ENTRY_STATUS(35, StockCode, StockCode_dbs)
  COLUMN_ENTRY_STATUS(36, StockName, StockName_dbs)
  COLUMN_ENTRY_STATUS(37, SedolCode, SedolCode_dbs)
  COLUMN_ENTRY_STATUS(38, RicCode, RicCode_dbs)
  COLUMN_ENTRY_STATUS(39, CusipCode, CusipCode_dbs)
  COLUMN_ENTRY_STATUS(40, FigiCode, FigiCode_dbs)
  COLUMN_ENTRY(41, QtyScale)
  COLUMN_ENTRY(42, QtyMultiplier)
  COLUMN_ENTRY_STATUS(43, Ccy1QtyScaleEnabled, Ccy1QtyScaleEnabled_dbs)
  COLUMN_ENTRY_STATUS(44, MarketId, MarketId_dbs)
  COLUMN_ENTRY(45, StrikePrice)
  COLUMN_ENTRY_STATUS(46, OptionKind, OptionKind_dbs)
  COLUMN_ENTRY_STATUS(47, OptionType, OptionType_dbs)
  COLUMN_ENTRY_STATUS(48, OptionSettleType, OptionSettleType_dbs)
  COLUMN_ENTRY_STATUS(49, BondInterestType, BondInterestType_dbs)

  END_PARAM_MAP()
};

//**********************************************************************************

class CSecurityDelAccessor {
 public:
  CSecurityDelAccessor() { memset(this, 0, sizeof(*this)); }

  LONG m_RETURN_VALUE;

  LONG m_SECURITY_ID;

  DEFINE_COMMAND_EX(CSecurityDelAccessor,
                    L"{ ? = CALL dbo.GQA_DelSecurity(?) }");

  BEGIN_PARAM_MAP(CSecurityDelAccessor)
  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, m_RETURN_VALUE)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, m_SECURITY_ID)
  END_PARAM_MAP()
};

#endif
