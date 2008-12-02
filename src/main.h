#ifndef __main_h__
#define __main_h__

#include <QString>
#include <QStringList>

#include "pch.h"

#include "main.h"
#include "mainwindow.h"
#include "misc.h"

extern QApplication *appication;
extern MainWindow   *mainwindow;
extern QFont        *bold_font;
extern QSqlDatabase *db;
extern QString       app_header;


extern QStringList kompl_status;
extern QStringList sostav_status;

extern int           permissions;
extern int           user_id;

#define USER_PERMISSION_USERS      0x00001 // редактирование пользователей
#define USER_PERMISSION_KOMPL_SIGN 0x00002 // утверждение комплектации

#if defined( Q_OS_WIN32 )
  #define QSETTINGS_PARAM (qApp->applicationDirPath()+"/sklad.ini"),QSettings::IniFormat
#elif defined( Q_OS_UNIX )
  #define QSETTINGS_PARAM "sklad"
#else
  #error Wrong OS
#endif

void sql_error_message( QSqlQuery &query, QWidget *parent=0);
QString sql_get_string( QSqlQuery &query, int n );
QColor int2color( int a );

#endif
