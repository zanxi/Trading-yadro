#ifndef __DBA___CALENDARS__H__INCLUDED__
#define __DBA___CALENDARS__H__INCLUDED__

const LPCWSTR dbCommandGetCalendars =
    L"{ CALL dbo.GQA_GetAllCalendars(?, ?, ?) }";
const LPCWSTR dbCommandGetCalendarDates =
    L"{ ? = CALL dbo.GQA_GetAllCalendarDates(?, ?, ?) }";
const LPCWSTR dbCommandInsert = L"{? = CALL dbo.GQA_EditCalendar(?,?,?,?) }";
const LPCWSTR dbCommandInsertCDate =
    L"{ CALL dbo.GQA_EditCalendarDate(?,?,?,?) }";

#pragma once

///////////////////////////// Calendars
/////////////////////////////////////////////////////////////////////

class CBasisCalendar {
 public:
  CBasisCalendar() { memset(this, 0, sizeof(*this)); }
  int Action, ReturnValue;
  QME::ParamDbQMECalendar m_fields = {};
};

class CCALENDAR_GetAllAccessor : public CBasisCalendar {
 public:
  DEFINE_COMMAND_EX(CCALENDAR_GetAllAccessor, dbCommandGetCalendars)
  BEGIN_COLUMN_MAP(CCALENDAR_GetAllAccessor)
  COLUMN_ENTRY(1, m_fields.CalendarId)
  COLUMN_ENTRY(2, m_fields.CalendarName)
  COLUMN_ENTRY(3, m_fields.Enabled)
  END_COLUMN_MAP()
};

class CCALENDAR_INSAccessor2 : public CBasisCalendar {
 public:
  CCALENDAR_INSAccessor2() { memset(this, 0, sizeof(*this)); }
  DEFINE_COMMAND_EX(CCALENDAR_INSAccessor2, dbCommandInsert)
  BEGIN_PARAM_MAP(CCALENDAR_INSAccessor2)
  SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
  COLUMN_ENTRY(1, ReturnValue)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, Action)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(3, m_fields.CalendarId)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(4, m_fields.CalendarName)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(5, m_fields.Enabled)
  END_PARAM_MAP()
};

///////////////////////////// CalendarDates
/////////////////////////////////////////////////////////////////////

class CBasisCalendarDate {
 public:
  CBasisCalendarDate() { memset(this, 0, sizeof(*this)); }
  int Action;
  QME::ParamDbQMECalendarDate m_fields = {};
};

class CCALENDARDATE_GetAllAccessor : public CBasisCalendarDate {
 public:
  DEFINE_COMMAND_EX(CCALENDARDATE_GetAllAccessor, dbCommandGetCalendarDates);
  BEGIN_COLUMN_MAP(CCALENDARDATE_GetAllAccessor)
  COLUMN_ENTRY(1, m_fields.CalendarId)
  COLUMN_ENTRY(2, m_fields.Date)
  COLUMN_ENTRY(3, m_fields.TradeIndicator)
  END_COLUMN_MAP()
};

class CCALENDARDATE_INSAccessor2 : public CBasisCalendarDate {
 public:
  CCALENDARDATE_INSAccessor2() { memset(this, 0, sizeof(*this)); }
  DEFINE_COMMAND_EX(CCALENDARDATE_INSAccessor2, dbCommandInsertCDate)
  BEGIN_PARAM_MAP(CCALENDARDATE_INSAccessor2)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(1, Action)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(2, m_fields.CalendarId)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(3, m_fields.Date)
  SET_PARAM_TYPE(DBPARAMIO_INPUT)
  COLUMN_ENTRY(4, m_fields.TradeIndicator)
  END_PARAM_MAP()
};

#endif
