#ifndef __BOM_H__
#define __BOM_H__

#include <QtCore>

//==============================================================================
//
//==============================================================================
class BomFile
{
  friend class BomAddDialog;

  struct t_data
  { QString name;
    QString nominal;
    int     count;
    QString position;
    QString type;
    bool operator<( const t_data &t1 ) const
      { return (type<t1.type)?(true):((type==t1.type)?(name<t1.name):(false)); }
  };

public:
  bool loadFile( const QString filename );
  void process();
  void sort();
private:
  QVector<t_data> data;
};

//==============================================================================
//
//==============================================================================
#include "ui_bomadddialog.h"
class BomAddDialog : public QDialog,
                     public Ui::BomAddDialog
{
  Q_OBJECT

  friend class BomAddDialog2;

  struct t_data
  { int          type;
    QString type_name;
    QString type_name_bom;
    QString name_bom;
    QString name_sklad;
    QString nominal;
    int count;
    int nalichie;
    QString position;
    bool operator<( const t_data &t1 ) const
    { return (type_name<t1.type_name)?(true):
            ((type_name==t1.type_name)?(name_bom<t1.name_bom):(false)); }
  };

public:
  BomAddDialog( QWidget *parent = 0 );
  bool init( QString filename = QString(),
             int izdelie_id = 0, int count = 1 );
private:
  void accept();
  int izdelie_id;
  int count;
  QVector<t_data> data;
  QMap<int,int> tw2data;
private slots:
  void on_tw_cellDoubleClicked(int,int);
};

//==============================================================================
//
//==============================================================================
#include "ui_bomadddialog2.h"
class BomAddDialog2 : public QDialog,
                      public Ui::BomAddDialog2
{
  Q_OBJECT
public:
  BomAddDialog2( BomAddDialog *parent = 0, int data_index = 0 );
private:
  void accept();
  BomAddDialog *parent;
  int data_index;
private slots:
  void on_pb1_clicked();
  void on_tw_cellDoubleClicked();
  void on_le2_textChanged( const QString & );
};

#endif
