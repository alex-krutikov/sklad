#ifndef __misc_h__
#define __misc_h__

#include "pch.h"

//==============================================================================
/// ������� ������� ������ �� SQL-�������
//==============================================================================
class SqlTable : public QTableWidget
{
  Q_OBJECT;
public:
  SqlTable( QWidget *parent = 0 );
  void refresh();
  void clear_query_fields();
  void add_query_field( QString fieldname,
                        int size,
                        QString sql_field,
                        int alignment=0 );
  void setShownFields( QString str );
  QString getShownFields();
  int  get_selected_id();
  QVector<int> get_selected_id_vect();
  int  set_selected_id( int id);

  QString       query_str_pk_field;    ///< ��� ���� PRIMARY KEY
  QString       query_str_color_field; ///< ��� ���� � ��������� �����
  QString       query_str_main;     ///< ����� ������� FROM ....
  QString       query_str_where;    ///< ����� ������� WHERE ....
  QString       query_str_group_by; ///< ����� ������� GROUP BY ....
  QString       query_str_order_by; ///< ����� ������� ORDER BY ....
  QString       query_str_limit;    ///< ����� ������� LIMIT ....

  QStringList   header_labels; ///< ��� �������� � �������
  QStringList   query_fields;  ///< ��� ���� � �������
  QVector<int>  header_sizes;  ///< �������� ����� ����� � �������
  QVector<int>  header_show_flag; ///< ������ ������ ���������/�� ��������� ����
  QVector<int>  alignment;       ///< ������ ������������ ������
  bool          rows_highlighting;
  bool          row_selection_mode;
private:
  QVector<int>  pk_values;
public slots:
  void my1( const QPoint & pos );
  void my2( int i );
};

//==============================================================================
//
//==============================================================================
class KomplTable : public QTableWidget
{
Q_OBJECT
public:
  KomplTable( QWidget *parent = 0 );
  void refresh( int id );
public slots:
  void add_zamena();
  void status();
private:
  int current_id;
  QVector<int> v_id;
};

//==============================================================================
//
//==============================================================================
#include "ui_defichit2.h"
class Defichit2 : public QWidget,
                  public Ui::Defichit2
{
Q_OBJECT
public:
  Defichit2( QWidget *parent = 0 );
public slots:
  void refresh();
private slots:
  void on_pb_print_clicked();
  void on_tw_cellDoubleClicked(int, int );
  void on_pb_copy_clicked();
  void on_pb_price_clicked();
private:
};

//==============================================================================
//
//==============================================================================
#include "ui_defichit2renamedialog.h"
class Defichit2RenameDialog : public QDialog,
                              public Ui::Defichit2RenameDialog
{
  Q_OBJECT
public:
  Defichit2RenameDialog( QWidget *parent = 0,
                         const QString &name  = QString(),
                         int type = 0,
                         const QString &kompl = QString() );

  QString name;
  int     type;
  QString kompl;
private slots:
  void on_le_filtr_textChanged(const QString &);
  void on_pb_rename_clicked();
};

//==============================================================================
//
//==============================================================================
#include "ui_defichitpricedialog.h"
class DefichitPriceDialog : public QDialog,
                            public Ui::DefichitPriceDialog
{
  Q_OBJECT
public:
  DefichitPriceDialog( QWidget *parent = 0,
                       const QString &items_str = QString() );
private slots:
  void on_pb_copy_clicked();

};

QString tablewidget2str( const QTableWidget *tw );

#endif
