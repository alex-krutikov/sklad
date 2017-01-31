TEMPLATE     = app
DEPENDPATH  += . build
INCLUDEPATH += . build

CONFIG -= exceptions rtti

### CONFIG += console

QT += sql
PRECOMPILED_HEADER = pch.h
RESOURCES = sklad.qrc
win32{ RC_FILE = sklad.rc }

OBJECTS_DIR    = build
UI_HEADERS_DIR = build
MOC_DIR        = build
DESTDIR        = build
RCC_DIR        = build

INCLUDEPATH += mysql/include

win32{
    LIBS += -Lmysql/lib -lmysqlclient /NODEFAULTLIB:LIBCMT
  #  LIBS += -Lmysql/lib -lmysqlclient
}
unix{
  LIBS        += -lmysql
}

# Input
HEADERS  =  mainwindow.h main.h dialogs.h dialogs2.h misc.h bom.h price.h   \
            proektform.h trebnaklad.h users.h

SOURCES  =  main.cpp mainwindow.cpp dialogs.cpp dialogs2.cpp misc.cpp       \
            bom.cpp price.cpp proektform.cpp trebnaklad.cpp users.cpp       \
            sqlactions.cpp

FORMS   =   ui/mainwindow.ui   ui/initdialog.ui     ui/pricedialog.ui      \
            ui/postavshiki.ui  ui/proizvoditeli.ui  ui/sostavadd.ui        \ 
            ui/izdelieadd.ui   ui/prihodfiltr.ui    ui/prihodadd.ui        \
            ui/zamenaadd.ui    ui/zakupkiadd.ui     ui/users.ui            \
            ui/rvvod.ui        ui/rvvodadd.ui                              \
            ui/otpravka.ui     ui/dootpravka.ui                            \
            ui/bomadddialog.ui ui/bomadddialog2.ui  ui/proektform.ui       \
            ui/proektformsostavfind.ui              ui/defichit2.ui        \
            ui/montagdialog.ui ui/defichit2renamedialog.ui                 \
            ui/defichitpricedialog.ui               ui/trebnaklad.ui       \
            ui/usersdialog.ui

SOURCES += mysql_plugin.cpp
HEADERS += mysql_plugin.h