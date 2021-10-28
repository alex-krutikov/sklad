/*! \mainpage Склад

    \section intro Версии

    20060926 - начальная тестовая версия

*/

#include "main.h"
#include "dialogs.h"
#include "mainwindow.h"
#include "misc.h"

#include <QErrorMessage>
#include <QSplashScreen>
#include <QSqlError>
#include <QSqlQuery>

//=======================================================================================
// глобальные переменные
//=======================================================================================
QApplication *application;
MainWindow *mainwindow_ptr;
QFont *bold_font;
QSqlDatabase *db;
QString app_header;

int permissions;
int user_id;

QStringList kompl_status;
QStringList sostav_status;


//=======================================================================================
// вывод ошибки SQL-запроса
//=======================================================================================
void sql_error_message(QSqlQuery &query, QWidget *parent)
{
    QErrorMessage msg(parent);
    msg.setModal(true);
    msg.setWindowTitle(QObject::tr("Ошибка запроса SQL"));
    msg.showMessage(query.lastError().driverText() + " | "
                    + query.lastError().databaseText() + " | QUERY: \""
                    + query.lastQuery() + "\"");
    msg.exec();

    //  QMessageBox::critical(parent, QObject::tr("Ошибка запроса SQL"),
    //                             query.lastError().driverText()
    //                   + "\n"  + query.lastError().databaseText()
    //                                                                 );
}

//=======================================================================================
//
//=======================================================================================
QColor int2color(int a)
{
    char *s = (char *)"white";
    switch (a)
    {
    case (10):
        s = (char *)"lightpink";
        break;
    case (20):
        s = (char *)"yellow";
        break;
    case (30):
        s = (char *)"white";
        break;
    case (40):
        s = (char *)"lightgreen";
        break;
    }
    return QColor(s);
}
//=======================================================================================
//
//=======================================================================================
QString sql_get_string(QSqlQuery &query, int n)
{
    return query.value(n).toString();
}

Q_IMPORT_PLUGIN(QMYSQLDriverPlugin)

//=======================================================================================
// MAIN
//=======================================================================================
int main(int argc, char *argv[])
{
    // QApplication::setStyle("cleanlooks");

    QApplication application(argc, argv);

    QTranslator qt_translator;
    if (qt_translator.load(":tr/qtbase_ru.qm"))
        application.installTranslator(&qt_translator);

    {
        InitDialog dialog;
        if (dialog.exec() == QDialog::Rejected) return 0;
    }

    QPixmap splashPixmap{":icons/icons/splash.png"};
    QSplashScreen splash{splashPixmap};
    splash.show();
    application.processEvents();


    MainWindow mainwindow;
    mainwindow_ptr = &mainwindow;
    mainwindow.show();

    splash.close();

    return application.exec();
}

// Q_IMPORT_PLUGIN(mysql)
