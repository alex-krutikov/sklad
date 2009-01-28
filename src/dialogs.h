#ifndef __dialogs_h__
#define __dialogs_h__

#include "pch.h"

//==============================================================================
// начальный диалог
//==============================================================================
#include "ui_initdialog.h"
class InitDialog : public QDialog,
                   public Ui::InitDialog
{
  Q_OBJECT
public:
  InitDialog();
private slots:
  void on_pb_db_clicked();
  void accept();
};

//=======================================================================================
//
//=======================================================================================
#include "ui_postavshiki.h"
class Postvshiki : public QDialog,
                   public Ui::Postavshiki
{
  Q_OBJECT
public:
  Postvshiki( QWidget *parent = 0);
private:
  void refresh();
private slots:
  void on_pb1_clicked();
  void on_pb2_clicked();
  void on_pb3_clicked();
};

//=======================================================================================
//
//=======================================================================================
#include "ui_proizvoditeli.h"
class Proizvoditeli : public QDialog,
                      public Ui::Proizvoditeli
{
  Q_OBJECT
public:
  Proizvoditeli( QWidget *parent = 0);
private:
  void refresh();
private slots:
  void on_pb1_clicked();
  void on_pb2_clicked();
  void on_pb3_clicked();
};

//=======================================================================================
//
//=======================================================================================
#include "ui_sostavadd.h"
class SostavAddDialog : public QDialog,
                        public Ui::SostavAddDialog
{
  Q_OBJECT
public:
  SostavAddDialog( QWidget *parent = 0, int id_arg=0 );
  void accept();
private:
  int id;
};

//=======================================================================================
//
//=======================================================================================
#include "ui_prihodfiltr.h"
class PrihodFiltrDialog : public QDialog,
                          public Ui::PrihodFiltrDialog
{
  Q_OBJECT
public:
  PrihodFiltrDialog( QWidget *parent = 0 );
private:
  int id;
public slots:
  void on_pb1_clicked();
};

//=======================================================================================
//
//=======================================================================================
#include "ui_prihodadd.h"
class PrihodAddDialog : public QDialog,
                        public Ui::PrihodAddDialog
{
  Q_OBJECT
public:
  PrihodAddDialog( QWidget *parent = 0, int id_arg = 0, int zakupka_arg = 0 );
private:
  int id;
  int zakupka;
  void accept();
  QCompleter *le2_completer;
  QCompleter *le10_completer;
  QMap<QString,int> map;
private slots:
  void my1( QString str );
  void on_pb1_clicked();
  void on_pb2_clicked();
};

//=======================================================================================
//
//=======================================================================================
#include "ui_zamenaadd.h"
class ZamenaAddDialog : public QDialog,
                        public Ui::ZamenaAddDialog
{
  Q_OBJECT
public:
  ZamenaAddDialog( QWidget *parent = 0, int id_arg = 0 );
private slots:
  void my1();
  void on_pb_rename_clicked();
  void on_pb_add_zamena_clicked();
private:
  int id,type;
};

//=======================================================================================
//
//=======================================================================================
#include "ui_zakupkiadd.h"
class ZakupkiAddDialog : public QDialog,
                         public Ui::ZakupkiAddDialog
{
  Q_OBJECT
public:
  ZakupkiAddDialog( QWidget *parent = 0, int id_arg = 0 );
private:
  int id;
  void accept();
  QCompleter *le1_completer;
  QMap<QString,int> map;
private slots:
  void my1( QString str );
  void on_pb1_clicked();
};

//=======================================================================================
//
//=======================================================================================
#include "ui_users.h"
class UsersDialog : public QDialog,
                    public Ui::UsersDialog
{
  Q_OBJECT
public:
  UsersDialog( QWidget *parent = 0 );
private:
private slots:
  void on_pb1_clicked();
  void on_pb2_clicked();
  void on_pb3_clicked();
  void my1();
};

//=======================================================================================
//
//=======================================================================================
#include "ui_rvvod.h"
class RVvodDialog : public QDialog,
                    public Ui::RVvodDialog
{
  Q_OBJECT
public:
  RVvodDialog( QWidget *parent = 0, int n_arg=0,int id_arg=0 );
  void accept();
private slots:
  void on_pb_add_clicked();
private:
  int id,n;
};

//=======================================================================================
//
//=======================================================================================
#include "ui_rvvodadd.h"
class RVvodAddDialog : public QDialog,
                       public Ui::RVvodAddDialog
{
  Q_OBJECT
public:
  RVvodAddDialog( QWidget *parent = 0 );
  QString name;
  QString type_name;
  int     type;
private slots:
  void tw_refresh();
private:
  void accept();
};

#endif
