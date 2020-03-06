#pragma once
#include <Domain/QME/InfoQmeCoupon.h>

namespace QME {

//**********************************************************************************

class CCouponGetAccessor : public CInfoQmeCoupon {
 public:
  DEFINE_COMMAND_EX(CCouponGetAccessor, L"EXEC GQA_GetAllCoupons")

  BEGIN_COLUMN_MAP(CCouponGetAccessor)

  COLUMN_ENTRY(1, _CouponId)
  COLUMN_ENTRY(2, _SecCode)
  COLUMN_ENTRY(3, _EmitDate)
  COLUMN_ENTRY(4, _ExpireDate)
  COLUMN_ENTRY(5, _CouponValue)
  COLUMN_ENTRY(6, _CouponValueUnits)

  END_COLUMN_MAP()

  void MapNullParams_From_DB(){};
};

//**********************************************************************************

class CCouponEditAccessor : public CInfoQmeCoupon {
 public:
  INT ReturnValue, Action;

  CCouponEditAccessor() : ReturnValue(0), Action(-1) {}

  DEFINE_COMMAND_EX(CCouponEditAccessor,
                    L"{ ? = CALL dbo.GQA_EditCoupons(?, ?, ?, ?, ?, ?, ?) }");

  BEGIN_PARAM_MAP(CCouponEditAccessor)

  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, ReturnValue)

  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, Action)
  COLUMN_ENTRY(3, _SecCode)
  COLUMN_ENTRY(4, _EmitDate)
  COLUMN_ENTRY(5, _ExpireDate)
  COLUMN_ENTRY(6, _CouponValue)
  COLUMN_ENTRY(7, _CouponValueUnits)
  COLUMN_ENTRY(8, _CouponId)
  END_PARAM_MAP()

  void MapNullParams_To_DB(){};
};
}  // namespace QME