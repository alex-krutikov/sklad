#include <QtSql>

#include "sqlactions.h"

extern void sql_error_message(QSqlQuery &query, QWidget *parent = 0);

//==============================================================================
// Цвет строк в таблице закупок
//==============================================================================
bool sql_action_zakupka_color(int zakupka_id)
{
    QSqlQuery query;
    query.prepare(" UPDATE zakupki SET color = "
                  "   IF( ( postavshik ) > 0 AND ( CHAR_LENGTH(schet) > 0 ), "
                  "   IF( n <= polucheno,  40 , 30 ), 10 ) "
                  " WHERE id = ? ");
    query.addBindValue(zakupka_id);
    if (!query.exec())
    {
        sql_error_message(query, 0);
        return false;
    }
    return true;
}
