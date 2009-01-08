#ifndef __mainwindow_h_
#define __mainwindow_h_

#include "pch.h"
#include "dialogs.h"

//==============================================================================
// главное окно
//==============================================================================
#include "ui_mainwindow.h"
class MainWindow : public QMainWindow,
                   public Ui::MainWindow
{
  Q_OBJECT
public:
  MainWindow();
  void prihod_snat(int prihod_id,int zamena_id, int n );
private:
  void closeEvent ( QCloseEvent * event );
  void kompl_print();
  void otpravka_print();
  void dootpravka_print();
  PrihodFiltrDialog *prihod_filtr_dialog;
  QString sql_where_rashod_kompl;
  QString sql_where_rashod_filtr;
public slots:
  void on_action_postavshiki_triggered();
  void on_action_proizvoditeli_triggered();
  void on_action_Pereraschet_triggered();
  void on_action_Stoimost_triggered();
  void on_action_export_all_kompl_triggered();
  void my_Delete();
  void on_action_Users_triggered();
  void on_pb_prihodFiltr_clicked();
  void on_pb_prihodAdd_clicked();
  void on_pb_zakupkiAdd_clicked();
  void on_pb_komplSnat_clicked();
  void on_pb_komplCard_clicked();
  void on_pb_komplPrint_clicked();
  void on_pb_kompl_status_clicked();
  void on_pb_kompl_price1_clicked();
  void on_cb_komplAll_toggled(bool);
  void on_pb_kompl_rashod_clicked();
  void on_cb_rashod_kompl_toggled(bool);
  void on_cb_rashod_show_all_toggled(bool);
  void pereraschet();

  void my1(const QString &);
  void my1A();
  void my3( int i );
  void my4();
  void my5();
  void my6();
  void my7();
  void timer_slot();

};

#endif
