#pragma once

//**********************************************************************************

class CAuctionsParams {
 public:
  CAuctionsParams() { memset(this, 0, sizeof(*this)); }

  INT RETURN_VALUE;

  INT AuctionId;
  INT SecurityId;
  INT IssuerId;
  INT BuySell;
  INT AuctionDate;
  INT OrderEntryPhaseStartTime;
  INT OrderEntryPhaseDuration;
  INT IssuerPhaseDuration;
  INT NumIssuerPhaseAutoProlongations;
  LONGLONG AuctionQty;
  INT MaxActiveOrdersPerFirm;
  LONGLONG OrderValueMultiplier;
  INT Flags;
  INT ParentAuctionId;
  INT AdditionalAuctionQtyPercent;
  INT Action;
  INT OperatorId;

  DBSTATUS AuctionId_dbs, SecurityId_dbs, IssuerId_dbs, BuySell_dbs,
      AuctionDate_dbs, OrderEntryPhaseStartTime_dbs,
      OrderEntryPhaseDuration_dbs, IssuerPhaseDuration_dbs,
      NumIssuerPhaseAutoProlongations_dbs, AuctionQty_dbs,
      MaxActiveOrdersPerFirm_dbs, OrderValueMultiplier_dbs, Flags_dbs,
      ParentAuctionId_dbs, AdditionalAuctionQtyPercent_dbs, OperatorId_dbs;

  INT GetAuctionFlags(const QME::CInfoQmeAuction* pInfo) {
    INT bitResult = 0;

    if (pInfo->ViewDOMDuringOrderEntryPhase)
      bitResult |= QME::CInfoQmeAuction::FlagsBitValues::Bit0;

    if (pInfo->AuctionType)
      bitResult |= QME::CInfoQmeAuction::FlagsBitValues::Bit1;
    return bitResult;
  }

  void ConvertFrom(const QME::CInfoQmeAuction& info) {
    this->AuctionId = info.AuctionId;
    this->SecurityId = info.SecurityId;
    this->IssuerId = info.IssuerId;

    SET_NULLABLE_INT_PARAM(this->BuySell, this->BuySell_dbs, info.BuySell);
    SET_NULLABLE_INT_PARAM(this->AuctionDate, this->AuctionDate_dbs,
                           info.AuctionDate);
    SET_NULLABLE_INT_PARAM(this->OrderEntryPhaseStartTime,
                           this->OrderEntryPhaseStartTime_dbs,
                           info.OrderEntryPhaseStartTime);
    SET_NULLABLE_INT_PARAM(this->OrderEntryPhaseDuration,
                           this->OrderEntryPhaseDuration_dbs,
                           info.OrderEntryPhaseDuration);
    SET_NULLABLE_INT_PARAM(this->IssuerPhaseDuration,
                           this->IssuerPhaseDuration_dbs,
                           info.IssuerPhaseDuration);

    this->NumIssuerPhaseAutoProlongations =
        info.NumIssuerPhaseAutoProlongations;
    SET_NULLABLE_INT_PARAM(this->AuctionQty, this->AuctionQty_dbs,
                           info.AuctionQty);
    this->MaxActiveOrdersPerFirm = info.MaxActiveOrdersPerFirm;
    SET_NULLABLE_INT_PARAM(this->OrderValueMultiplier,
                           this->OrderValueMultiplier_dbs,
                           info.OrderValueMultiplier);
    this->Flags =
        this->Flags & ~QME::CInfoQmeAuction::FlagsBitValues::AllRelevantBits |
        GetAuctionFlags(&info);
    this->ParentAuctionId = info.ParentAuctionId;
    SET_NULLABLE_INT_PARAM(this->AdditionalAuctionQtyPercent,
                           this->AdditionalAuctionQtyPercent_dbs,
                           info.AdditionalAuctionQtyPercent);
    this->Action = (AuctionId == -1) ? 0 : 1;
    SET_NULLABLE_INT_PARAM(this->OperatorId, this->OperatorId_dbs,
                           info.OperatorId);
  }

  void ConvertTo(QME::CInfoQmeAuction& info) const {
    info.AuctionId = this->AuctionId;
    info.SecurityId = this->SecurityId;
    info.IssuerId = this->IssuerId;

    GET_NULLABLE_INT_PARAM(this->BuySell, this->BuySell_dbs, info.BuySell);
    GET_NULLABLE_INT_PARAM(this->AuctionDate, this->AuctionDate_dbs,
                           info.AuctionDate);
    GET_NULLABLE_INT_PARAM(this->OrderEntryPhaseStartTime,
                           this->OrderEntryPhaseStartTime_dbs,
                           info.OrderEntryPhaseStartTime);
    GET_NULLABLE_INT_PARAM(this->OrderEntryPhaseDuration,
                           this->OrderEntryPhaseDuration_dbs,
                           info.OrderEntryPhaseDuration);
    GET_NULLABLE_INT_PARAM(this->IssuerPhaseDuration,
                           this->IssuerPhaseDuration_dbs,
                           info.IssuerPhaseDuration);

    GET_NULLABLE_INT_PARAM(this->OrderValueMultiplier,
                           this->OrderValueMultiplier_dbs,
                           info.OrderValueMultiplier);

    info.NumIssuerPhaseAutoProlongations =
        this->NumIssuerPhaseAutoProlongations;
    GET_NULLABLE_INT_PARAM(this->AuctionQty, this->AuctionQty_dbs,
                           info.AuctionQty);

    info.MaxActiveOrdersPerFirm = this->MaxActiveOrdersPerFirm;
    info.OrderValueMultiplier = this->OrderValueMultiplier;

    long flags = this->Flags;
    info.ViewDOMDuringOrderEntryPhase =
        (bool)(flags & QME::CInfoQmeAuction::FlagsBitValues::Bit0);
    info.AuctionType =
        (bool)(flags & QME::CInfoQmeAuction::FlagsBitValues::Bit1);

    info.ParentAuctionId = this->ParentAuctionId;
    GET_NULLABLE_INT_PARAM(this->AdditionalAuctionQtyPercent,
                           this->AdditionalAuctionQtyPercent_dbs,
                           info.AdditionalAuctionQtyPercent);
    GET_NULLABLE_INT_PARAM(this->OperatorId, this->OperatorId_dbs,
                           info.OperatorId);
  }
};

//**********************************************************************************

class CAuctionGetAccessor : public CAuctionsParams {
 public:
  DEFINE_COMMAND_EX(CAuctionGetAccessor, L"EXEC GQA_GetAllAuctionSchedule")

  BEGIN_COLUMN_MAP(CAuctionGetAccessor)
  COLUMN_ENTRY(1, AuctionId)
  COLUMN_ENTRY(2, SecurityId)
  COLUMN_ENTRY(3, IssuerId)
  COLUMN_ENTRY_STATUS(4, OperatorId, OperatorId_dbs)
  COLUMN_ENTRY_STATUS(5, BuySell, BuySell_dbs)
  COLUMN_ENTRY_STATUS(6, AuctionDate, AuctionDate_dbs)
  COLUMN_ENTRY_STATUS(7, OrderEntryPhaseStartTime, OrderEntryPhaseStartTime_dbs)
  COLUMN_ENTRY_STATUS(8, OrderEntryPhaseDuration, OrderEntryPhaseDuration_dbs)
  COLUMN_ENTRY_STATUS(9, IssuerPhaseDuration, IssuerPhaseDuration_dbs)
  COLUMN_ENTRY(10, NumIssuerPhaseAutoProlongations)
  COLUMN_ENTRY(11, Flags)
  COLUMN_ENTRY_STATUS(12, AuctionQty, AuctionQty_dbs)
  COLUMN_ENTRY(13, MaxActiveOrdersPerFirm)
  COLUMN_ENTRY_STATUS(14, OrderValueMultiplier, OrderValueMultiplier_dbs)
  COLUMN_ENTRY(15, ParentAuctionId)
  COLUMN_ENTRY_STATUS(16,
                      AdditionalAuctionQtyPercent,
                      AdditionalAuctionQtyPercent_dbs)
  END_COLUMN_MAP()
};

//**********************************************************************************

class CAuctionInsAccessor : public CAuctionsParams {
 public:
  DEFINE_COMMAND_EX(CAuctionInsAccessor,
                    L"{ ? = CALL dbo.GQA_EditAuctionSchedule( \
			?, ?, ?, ?, ?, ?, ?, ?, ?, ?, \
			?, ?, ?, ?, ?, ?, ?) }");

  BEGIN_PARAM_MAP(CAuctionInsAccessor)
  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, RETURN_VALUE)

  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, Action)
  COLUMN_ENTRY(3, AuctionId)
  COLUMN_ENTRY(4, SecurityId)
  COLUMN_ENTRY(5, IssuerId)
  COLUMN_ENTRY_STATUS(6, OperatorId, OperatorId_dbs)
  COLUMN_ENTRY_STATUS(7, BuySell, BuySell_dbs)
  COLUMN_ENTRY_STATUS(8, AuctionDate, AuctionDate_dbs)
  COLUMN_ENTRY_STATUS(9, OrderEntryPhaseStartTime, OrderEntryPhaseStartTime_dbs)
  COLUMN_ENTRY_STATUS(10, OrderEntryPhaseDuration, OrderEntryPhaseDuration_dbs)
  COLUMN_ENTRY_STATUS(11, IssuerPhaseDuration, IssuerPhaseDuration_dbs)
  COLUMN_ENTRY(12, NumIssuerPhaseAutoProlongations)
  COLUMN_ENTRY(13, Flags)
  COLUMN_ENTRY_STATUS(14, AuctionQty, AuctionQty_dbs)
  COLUMN_ENTRY(15, MaxActiveOrdersPerFirm)
  COLUMN_ENTRY_STATUS(16, OrderValueMultiplier, OrderValueMultiplier_dbs)
  COLUMN_ENTRY(17, ParentAuctionId)
  COLUMN_ENTRY(18, AdditionalAuctionQtyPercent)
  END_PARAM_MAP()
};

//**********************************************************************************

class CAuctionDelAccessor {
 public:
  CAuctionDelAccessor() { memset(this, 0, sizeof(*this)); }

  LONG m_RETURN_VALUE;

  LONG m_AUCTION_ID;

  DEFINE_COMMAND_EX(CAuctionDelAccessor,
                    L"{ ? = CALL dbo.GQA_DelAuctionSchedule(?) }");

  BEGIN_PARAM_MAP(CAuctionDelAccessor)
  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, m_RETURN_VALUE)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, m_AUCTION_ID)
  END_PARAM_MAP()
};