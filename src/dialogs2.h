#ifndef __dialogs2_h__
#define __dialogs2_h__

//=======================================================================================
//
//=======================================================================================
#include "ui_otpravka.h"
class OtpravkaDialog : public QDialog,
                       public Ui::OtpravkaDialog
{
  Q_OBJECT
public:
  OtpravkaDialog( QWidget *parent = 0 );
  void refresh();
  int sostav_id;
private slots:
  void on_pb_print_clicked();
  void on_pb_copy_clicked();
private:
  struct t_data
  { int flag;
    int defichit_flag;
    QString name;
    QString nominal;
    int treb;
    int snato;
    QString position;
  };
  QList<t_data> data;
};

//=======================================================================================
//
//=======================================================================================
#include "ui_dootpravka.h"
class DootpravkaDialog : public QDialog,
                         public Ui::DootpravkaDialog
{
  Q_OBJECT
public:
  DootpravkaDialog( QWidget *parent = 0 );
  int sostav_id;
private slots:
  void on_pb_print_clicked();
  void on_pb_copy_clicked();
public slots:
  void refresh();
private:
};

//=======================================================================================
//
//=======================================================================================
#include "ui_montagdialog.h"
class MontagDialog : public QDialog,
                     public Ui::MontagDialog
{
  Q_OBJECT
public:
  MontagDialog( QWidget *parent = 0, QString name = QString() );
  QString text() { return le->text(); }
private slots:
  void on_pb_date_clicked();
  void on_pb_piter_clicked();
  void on_pb_ilya_clicked();
};

//=======================================================================================
//
//=======================================================================================
#include "ui_typesdialog.h"
class TypesDialog : public QDialog,
                    public Ui::TypesDialog
{
  Q_OBJECT
public:
  TypesDialog( QWidget *parent = 0);
  void refresh_table();
private slots:
  void on_pb_add_clicked();
  void on_pb_edit_clicked();
  void on_pb_delete_clicked();
};

//=======================================================================================
//
//=======================================================================================
#include "ui_typesitemdialog.h"
class TypesItemDialog : public QDialog,
                    public Ui::TypesItemDialog
{
  Q_OBJECT
public:
  TypesItemDialog(int id = -1, QWidget *parent = 0);
private slots:
  void accept();
private:
  const int id;
};

#endif
