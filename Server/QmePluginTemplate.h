#pragma once

template <typename T_GET_ACCESSOR>
BOOL QmePlugin::GetAllObjects(CAlert& a,
                              BOOL bForced,
                              const std::string& strDbUser) {
  BOOL bRes(FALSE);
  std::string strErrDB;
  devent(LL_HIGH, const_cast<char*>(
                      Format("GetTable %s",
                             T_GET_ACCESSOR::TInfoClassType::TABLE_NAME.c_str())
                          .c_str()));

  CAlert table_records;
  CAlert field_names;
  int record_count(0);

  CCommand<CAccessor<T_GET_ACCESSOR>> cmd;
  HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
  if (!ISFAILED(hr, strErrDB, m_OLE_DB_Provider)) {
    while (cmd.MoveNext() == S_OK) {
      cmd.MapNullParams_From_DB();  // Замапить нулевые параметры в дата-объект
                                    // (Вызывать после запроса в БД\перемещения
                                    // курсора)
      CAlert record(CDomainSerializer::ToAlert(&cmd));
      record_count++;
      table_records << record;
      cmd.ClearRecordMemory();
    }

    a.clear();
    BOOL use_filter = FALSE;
    a << T_GET_ACCESSOR::TInfoClassType::TABLE_NAME << bForced << record_count
      << use_filter << table_records << field_names;

    bRes = TRUE;
  } else {
    CreateCommonError(a, T_GET_ACCESSOR::TInfoClassType::TABLE_NAME, strErrDB);
    bRes = FALSE;
  }

  cmd.Close();
  cmd.ReleaseCommand();

  return bRes;
}

template <typename T_EDIT_ACCESSOR>
BOOL QmePlugin::EditObject(CAlert& aRow,
                           const std::string& strDbUser,
                           QME::CrudActions act) {
  BOOL bRes(FALSE);

  std::string strErrDB;
  CCommand<CAccessor<T_EDIT_ACCESSOR>> cmd;
  CDomainSerializer::FromAlert(&cmd, aRow);
  cmd.Action = act;
  // cmd. = act;

  cmd.MapNullParams_To_DB();  // Замапить нулевые параметры в БД (Вызывать перед
                              // запросом в БД)

  HRESULT hr = cmd.Open(m_OLE_DB_Provider[strDbUser]);
  if (ISFAILED(hr, strErrDB, m_OLE_DB_Provider)) {
    CreateDbError(aRow, PCORE::GQA_FUNCTION_INSERT_INTO_TABLE,
                  T_EDIT_ACCESSOR::TInfoClassType::TABLE_NAME, strErrDB, 0);
    bRes = FALSE;
  } else {
    aRow.clear();
    aRow << T_EDIT_ACCESSOR::TInfoClassType::TABLE_NAME;

    int nResult = 1;
    aRow << nResult;
    aRow << std::string("");
    aRow << std::string("");
    aRow << (nResult = -1);
    aRow << (nResult = cmd.ReturnValue);

    bRes = TRUE;
  }

  cmd.Close();
  cmd.ReleaseCommand();

  return bRes;
}
