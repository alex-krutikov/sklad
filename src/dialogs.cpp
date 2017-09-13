#include <QtGui>
#include <QtSql>

#include "dialogs.h"
#include "main.h"
#include "bom.h"
#include "mainwindow.h"
#include "sqlactions.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QCompleter>

//#######################################################################################
// начальный диалог
//#######################################################################################
InitDialog::InitDialog()
{
  int i;
  app_header = "Склад";

  setupUi( this );
  setWindowTitle( app_header );

	QSettings settings( QSETTINGS_PARAM );
	QString str = settings.value( "mysqlserver" ).toString();
	i = cb1->findText( str );
	if( i >=0 )
	{ cb1->setCurrentIndex( i );
	}	else
	{ if( !str.isEmpty() )
	  { cb1->addItem( str );
	    cb1->setCurrentIndex( cb1->count()-1 );
	  }
	}
	le2->setText( settings.value( "username" ).toString() );
	le3->setFocus();
	if( le2->text().isEmpty() ) le2->setFocus();

    le_db_name->setText( settings.value( "mysqlname","sklad4" ).toString() );
    le_db_user->setText( settings.value( "mysqluser","sklad" ).toString() );
    le_db_pass->setText( settings.value( "mysqlpass","crkfl" ).toString() );

	frame->hide();
	adjustSize();
}

//==============================================================================
// начальный диалог -- OK
//==============================================================================
void InitDialog::on_pb_db_clicked()
{
  frame->show();
}

//==============================================================================
// начальный диалог -- OK
//==============================================================================
void InitDialog::accept()
{
  bool ok;

  db = new QSqlDatabase;
  *db = QSqlDatabase::addDatabase("QMYSQL");
  db->setHostName( cb1->currentText() );
  db->setPort(3306);

  db->setDatabaseName( le_db_name->text() );

  db->setUserName(le_db_user->text());
  db->setPassword(le_db_pass->text());
  ok = db->open();
  if( !ok )
  { QMessageBox::critical(0, "Ошибка",  db->lastError().driverText()
                           );
    done( QDialog::Rejected );
    return;
  }

  QSettings settings( QSETTINGS_PARAM );
  settings.setValue( "mysqlserver", cb1->currentText() );
  settings.setValue( "mysqlname", le_db_name->text() );
  settings.setValue( "mysqluser", le_db_user->text() );
  settings.setValue( "mysqlpass", le_db_pass->text() );

  QSqlQuery query;
  if( !query.exec("SET NAMES \'utf8\'") )
  { QMessageBox::critical(0, "Ошибка", "Не могу установить кодировку"
                             + query.lastError().driverText()
                                        );
    done( QDialog::Rejected );
	  return;
  }

  if(    ( le2->text() == "root" )
      && ( le3->text() == "123456" ) )
  { permissions = -1;
    done( QDialog::Accepted );
    return;
  }

  query.prepare( "SELECT id, passw, permissions "
                 "   FROM users WHERE nickname = ? " );
  query.addBindValue( le2->text() );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  if( query.size() == 1 )
  {  query.next();
     if( QString::fromUtf8( query.value(1).toByteArray() ) == le3->text() )
     { user_id = query.value(0).toInt();
       permissions = query.value(2).toInt();

       settings.setValue( "username", le2->text() );
       done( QDialog::Accepted );
       return;
     }
  }
  QMessageBox::critical(0, app_header, "Имя пользователя или пароль неверны."  );
}

//#######################################################################################
//
//#######################################################################################
RVvodDialog::RVvodDialog( QWidget *parent, int n_arg ,int id_arg )
  : QDialog( parent ), id( id_arg ), n( n_arg )
{
  QSqlQuery query;
  QStringList sl;

  setupUi( this );
  setWindowTitle( "Ручной ввод" );
  resize(710,500);

  tw->setColumnCount( 4 );
  tw->verticalHeader()->setDefaultSectionSize(font().pointSize()+11);
  sl << "Тип"
     << "Наименование"
     << "Номинал"
     << "Кол-во";
  tw->setHorizontalHeaderLabels( sl );
  tw->horizontalHeader()->resizeSection( 0 , 150 );
  tw->horizontalHeader()->resizeSection( 1 , 150 );
  tw->horizontalHeader()->resizeSection( 2 , 100 );
  tw->horizontalHeader()->resizeSection( 3 ,  50 );
  tw->setEditTriggers( QAbstractItemView::NoEditTriggers );
}

//=======================================================================================
//
//=======================================================================================
bool RVvodDialog::init_from_sostav_id(int sostav_id)
{
    QSqlQuery query;

    query.prepare("SELECT `name` FROM `sostav` WHERE `id` = ?");
    query.addBindValue( sostav_id );
    if( !query.exec() )
    {  sql_error_message( query, this );
       return false;
    }

    if (query.size() != 1) {
        QMessageBox::information(0, "Склад", QString("Спецификация с номером %1 не найдена.").arg(sostav_id));
        return false;
    }

    query.next();
    le1->setText(query.value(0).toString());

    query.prepare("SELECT `name`, `nominal`, `type`, `typename`, `n` "
                  "FROM `kompl` LEFT JOIN `types` ON `kompl`.`type`=`types`.`id` "
                  "WHERE `kompl`.`sostav` = ? ORDER BY `kompl`.`id`");
    query.addBindValue( sostav_id );
    if( !query.exec() )
    {  sql_error_message( query, this );
       return false;
    }
    tw->setRowCount(query.size());
    int row_index = 0;
    while(query.next())
    {
        QTableWidgetItem *ti = new QTableWidgetItem;
        ti->setText( query.value(3).toString() );
        ti->setData( Qt::UserRole, query.value(2) );
        tw->setItem( row_index, 0 , ti);
        ti = new QTableWidgetItem;
        ti->setText( query.value(0).toString() );
        tw->setItem( row_index, 1 , ti);
        ti = new QTableWidgetItem;
        ti->setText( query.value(1).toString() );
        tw->setItem( row_index, 2 , ti);
        ti = new QTableWidgetItem;
        ti->setText( query.value(4).toString());
        tw->setItem( row_index, 3 , ti);
        ++row_index;
    }

    return true;
}

//=======================================================================================
//
//=======================================================================================
void RVvodDialog::on_pb_add_clicked()
{
  QTableWidgetItem *ti;
  RVvodAddDialog dialog( this );
  dialog.exec();
  if( dialog.result() )
  { tw->setRowCount( tw->rowCount() + 1 );
    ti = new QTableWidgetItem;
    ti->setText( dialog.type_name );
    ti->setData( Qt::UserRole, dialog.type );
    tw->setItem( tw->rowCount()-1, 0 , ti);
    ti = new QTableWidgetItem;
    ti->setText( dialog.name );
    tw->setItem( tw->rowCount()-1, 1 , ti);
    ti = new QTableWidgetItem;
    ti->setText( dialog.le_nominal->text() );
    tw->setItem( tw->rowCount()-1, 2 , ti);
    ti = new QTableWidgetItem;
    ti->setText( QString::number( dialog.sb_n->value() ) );
    tw->setItem( tw->rowCount()-1, 3 , ti);

  }
}

//=======================================================================================
//
//=======================================================================================
void RVvodDialog::accept()
{
  int i,last_id,last_id2;
  QProgressDialog progress("Copying files...", 0, 0, tw->rowCount(), this);

  if( le1->text().isEmpty() )
  {QMessageBox::critical(this, app_header, "Поле \"Название\" не заполнено" );
    le1->setFocus();
    return;
  }

  QSqlQuery query;

  query.prepare( "INSERT INTO sostav ( izdelie, name, n1 , status2) "
                 "VALUES ( ?,?,?,? ) " );
  query.addBindValue( id );
  query.addBindValue( le1->text().toUtf8() );
  query.addBindValue( n );
  query.addBindValue( "Не утвержден" );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  last_id = query.lastInsertId().toInt();

  for( i=0; i<tw->rowCount(); i++ )
  {
    query.prepare( "INSERT INTO kompl ( sostav, name, nominal, type, n ) "
                   "VALUES ( ?,?,?,?,? ) " );
    query.addBindValue( last_id );
    query.addBindValue( tw->item(i,1)->text().toUtf8()      );
    query.addBindValue( tw->item(i,2)->text().toUtf8()      );
    query.addBindValue( tw->item(i,0)->data( Qt::UserRole ) );
    query.addBindValue( tw->item(i,3)->text().toInt()       );
    if( !query.exec() )
    {  sql_error_message( query, this );
	     return;
    }

    last_id2 = query.lastInsertId().toInt();

    query.prepare( "INSERT INTO zamena ( kompl, name )  "
                   "VALUES ( ?,? ) " );
    query.addBindValue( last_id2 );
    query.addBindValue( tw->item(i,1)->text().toUtf8()    );
    if( !query.exec() )
    {  sql_error_message( query, this );
	     return;
    }

    progress.setValue(i);
  }
  done( QDialog::Accepted );
}

//#######################################################################################
//
//#######################################################################################
RVvodAddDialog::RVvodAddDialog( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  setWindowTitle( "Добавить" );

  connect( cb_type,  SIGNAL( currentIndexChanged(int) ),
           this,       SLOT( tw_refresh() ) );
  connect( le_filtr, SIGNAL( textChanged(const QString &) ),
           this,       SLOT( tw_refresh() ) );

  QSqlQuery query;
  query.prepare( "SELECT id, typename FROM types WHERE used = 1 ORDER BY typename ASC" );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  cb_type->addItem( "" , -1 );
  while( query.next() )
  { cb_type->addItem( query.value(1).toString(),  query.value(0) );
  }
}

//=======================================================================================
//
//=======================================================================================
void RVvodAddDialog::tw_refresh()
{
  int i;
  QString str;
  QTableWidgetItem *ti;
  QStringList sl;
  tw->clear();
  tw->setColumnCount( 3 );
  tw->verticalHeader()->hide();
  tw->verticalHeader()->setDefaultSectionSize(font().pointSize()+11);
  sl << "Тип"
     << "Название"
     << "Остаток";
  tw->setHorizontalHeaderLabels( sl );
  tw->horizontalHeader()->resizeSection( 0 , 150 );
  tw->horizontalHeader()->resizeSection( 1 , 150 );
  tw->horizontalHeader()->resizeSection( 2 ,  70 );
  tw->setSelectionBehavior( QAbstractItemView::SelectRows );
  tw->setSelectionMode( QAbstractItemView::SingleSelection );
  tw->setEditTriggers( QAbstractItemView::NoEditTriggers );

  str = " "+le_filtr->text()+" ";
  str.replace(" ","%");
  i = cb_type->itemData( cb_type->currentIndex() ).toInt();

  QSqlQuery query;
  query.prepare( "SELECT type, MAX(typename), name, sum(ostatok) FROM prihod "
                 "  LEFT JOIN types ON prihod.type = types.id "
                 "  WHERE ( type = ? OR ? < 0 ) AND name LIKE ? "
                 "  GROUP BY type, name ORDER BY name ASC " );
  query.addBindValue( i );
  query.addBindValue( i );
  query.addBindValue( str );
  if( !query.exec() )
  {  sql_error_message( query, mainwindow );
	   return;
  }
  i=0;
  tw->setRowCount( query.size() );
  while( query.next() )
  { ti = new QTableWidgetItem;
    ti -> setData( Qt::UserRole, query.value(0).toString() );
    ti -> setText( query.value(1).toString() );
    tw->setItem( i, 0, ti );
    ti = new QTableWidgetItem;
    ti -> setText( query.value(2).toString() );
    tw->setItem( i, 1, ti );
    ti = new QTableWidgetItem;
    ti -> setText( query.value(3).toString() );
    ti -> setTextAlignment( Qt::AlignRight|Qt::AlignVCenter );
    tw->setItem( i, 2, ti );
    i++;
  }
}

//=======================================================================================
//
//=======================================================================================
void RVvodAddDialog::accept()
{
  int i = tw->currentRow();

  if( i < 0 )
  { QMessageBox::critical(this, app_header,"Элемент не выбран!" );
    return;
  }
  name      = tw->item( i, 1 )->text();
  type_name = tw->item( i, 0 )->text();
  type      = tw->item( i, 0 )->data( Qt::UserRole ).toInt();
  done(  QDialog::Accepted );
}

//#######################################################################################
//
//#######################################################################################
Postvshiki::Postvshiki( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  setWindowTitle( app_header );

  refresh();
}

//=======================================================================================
//
//=======================================================================================
void Postvshiki::refresh()
{
  QSqlQuery query;
  QListWidgetItem *item;

  if( !query.exec( "SELECT id, name FROM postavshiki ORDER BY name" ) )
  {  sql_error_message( query, this );
	   return;
  }
  lw->clear();
  while( query.next() )
  { item = new QListWidgetItem;
    item -> setText( sql_get_string( query, 1 ) );
    item -> setData( Qt::UserRole, query.value(0) );
    lw->addItem( item );
  }
}
//=======================================================================================
//
//=======================================================================================
void Postvshiki::on_pb1_clicked()
{
  QString str;
  bool ok;
  str = QInputDialog::getText(this, app_header,
                                    "Название поставщика:",
                                    QLineEdit::Normal, "", &ok);
  if( !ok ) return;
  if( str.isEmpty() ) return;

  QSqlQuery query;
  query.prepare( " INSERT INTO postavshiki ( name ) VALUES ( ? ) " );
  query.addBindValue( str.toUtf8() );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  refresh();
}

//=======================================================================================
//
//=======================================================================================
void Postvshiki::on_pb2_clicked()
{
  QString str;
  bool ok;
  if( lw->currentRow() < 0 ) return;
  str = QInputDialog::getText(this, app_header, "Название поставщика:",
                                    QLineEdit::Normal,
                                    lw->currentItem()->text(), &ok);
  if( !ok ) return;
  if( str.isEmpty() ) return;

  QSqlQuery query;
  query.prepare( " UPDATE postavshiki SET name = ? WHERE id = ? " );
  query.addBindValue( str.toUtf8() );
  query.addBindValue( lw->currentItem()->data( Qt::UserRole ) );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  refresh();
}

//=======================================================================================
//
//=======================================================================================
void Postvshiki::on_pb3_clicked()
{
  if( lw->currentRow() < 0 ) return;
  QSqlQuery query;
  query.prepare( " DELETE FROM postavshiki WHERE id = ? " );
  query.addBindValue( lw->currentItem()->data( Qt::UserRole ) );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  refresh();
}

//#######################################################################################
//
//#######################################################################################
Proizvoditeli::Proizvoditeli( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  setWindowTitle( app_header );

  refresh();
}

//=======================================================================================
//
//=======================================================================================
void Proizvoditeli::refresh()
{
  QSqlQuery query;
  QListWidgetItem *item;

  if( !query.exec( "SELECT id, name FROM proizvoditeli ORDER BY name" ) )
  {  sql_error_message( query, this );
	   return;
  }
  lw->clear();
  while( query.next() )
  { item = new QListWidgetItem;
    item -> setText( sql_get_string( query, 1 ) );
    item -> setData( Qt::UserRole, query.value(0) );
    lw->addItem( item );
  }
}
//=======================================================================================
//
//=======================================================================================
void Proizvoditeli::on_pb1_clicked()
{
  QString str;
  bool ok;
  str = QInputDialog::getText(this, app_header , "Название производителя:",
                                    QLineEdit::Normal, "", &ok);
  if( !ok ) return;
  if( str.isEmpty() ) return;

  QSqlQuery query;
  query.prepare( " INSERT INTO proizvoditeli ( name ) VALUES ( ? ) " );
  query.addBindValue( str.toUtf8() );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  refresh();
}

//=======================================================================================
//
//=======================================================================================
void Proizvoditeli::on_pb2_clicked()
{
  QString str;
  bool ok;
  if( lw->currentRow() < 0 ) return;
  str = QInputDialog::getText(this, app_header , "Название производителя:",
                                    QLineEdit::Normal,
                                    lw->currentItem()->text(), &ok);
  if( !ok ) return;
  if( str.isEmpty() ) return;

  QSqlQuery query;
  query.prepare( " UPDATE proizvoditeli SET name = ? WHERE id = ? " );
  query.addBindValue( str.toUtf8() );
  query.addBindValue( lw->currentItem()->data( Qt::UserRole ) );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  refresh();
}

//=======================================================================================
//
//=======================================================================================
void Proizvoditeli::on_pb3_clicked()
{
  if( lw->currentRow() < 0 ) return;
  QSqlQuery query;
  query.prepare( " DELETE FROM proizvoditeli WHERE id = ? " );
  query.addBindValue( lw->currentItem()->data( Qt::UserRole ) );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  refresh();
}

//#######################################################################################
//
//#######################################################################################
SostavAddDialog::SostavAddDialog( QWidget *parent,int id_arg )
  : QDialog( parent ), id( id_arg )
{
  setupUi( this );
  setWindowTitle( "Добавить в состав изделя" );
}

//=======================================================================================
//
//=======================================================================================
void SostavAddDialog::accept()
{
  switch( cb1->currentIndex() )
  { case( 0 ): // чтение BOM-файла
    { QSettings settings( QSETTINGS_PARAM );
      QString str = QFileDialog::getOpenFileName(
                      this,
                      "Импорт BOM-файла",
                      settings.value( "bompath", "" ).toString(),
                      "BOM (*.bom)");
      if( !str.isEmpty() )
      { BomAddDialog dialog(this);
        if( !dialog.init( str,id,sb1->value() ) ) { done(QDialog::Rejected) ; return; }
        if( dialog.exec() == QDialog::Rejected )  { done(QDialog::Rejected) ; return; }
        str.left( str.lastIndexOf("/") );
        settings.setValue( "bompath", str );
        done(QDialog::Accepted);
      }
    }
    break;
  case( 1 ):  // ручной ввод
    { RVvodDialog dialog( this, sb1->value(), id );
      dialog.exec();

      if (dialog.result())
        done(QDialog::Accepted);
    }
    break;
  case( 2 ):  // повтор спецификации по номеру
    { QInputDialog sostav_id_dialog(this);
      sostav_id_dialog.setWindowTitle("Ввод номера спецификации");
      sostav_id_dialog.setInputMode(QInputDialog::IntInput);
      sostav_id_dialog.setLabelText("Номер спецификации:");
      sostav_id_dialog.setIntMinimum(1);
      sostav_id_dialog.setIntMaximum(1000000);
      sostav_id_dialog.exec();

      if (sostav_id_dialog.result())
      {   RVvodDialog rv_dialog( this, sb1->value(), id );
          bool ok = rv_dialog.init_from_sostav_id(sostav_id_dialog.intValue());
          if (ok)
          { rv_dialog.exec();
            if (rv_dialog.result())
              done(QDialog::Accepted);
          }
      }
    }
    break;
  }
}

//#######################################################################################
//
//#######################################################################################
PrihodFiltrDialog::PrihodFiltrDialog( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  setWindowTitle( "Фильтр прихода");

  connect( bcb2,  SIGNAL( toggled(bool)), cb1, SLOT( setEnabled(bool) ) );
  connect( bcb2,  SIGNAL( clicked()    ), cb1, SLOT( setFocus()       ) );
  connect( bcb3,  SIGNAL( toggled(bool)), le2, SLOT( setEnabled(bool) ) );
  connect( bcb3,  SIGNAL( clicked()    ), le2, SLOT( setFocus()       ) );
  connect( bcb5,  SIGNAL( toggled(bool)), cb2, SLOT( setEnabled(bool) ) );
  connect( bcb5,  SIGNAL( clicked()    ), cb2, SLOT( setFocus()       ) );
  connect( bcb6,  SIGNAL( toggled(bool)), cb3, SLOT( setEnabled(bool) ) );
  connect( bcb6,  SIGNAL( clicked()    ), cb3, SLOT( setFocus()       ) );
  connect( bcb7,  SIGNAL( toggled(bool)), le4, SLOT( setEnabled(bool) ) );
  connect( bcb7,  SIGNAL( clicked()    ), le4, SLOT( setFocus()       ) );
  connect( bcb8,  SIGNAL( toggled(bool)), le5, SLOT( setEnabled(bool) ) );
  connect( bcb8,  SIGNAL( clicked()    ), le5, SLOT( setFocus()       ) );
  connect( bcb9,  SIGNAL( toggled(bool)), de1, SLOT( setEnabled(bool) ) );
  connect( bcb9,  SIGNAL( clicked()    ), de1, SLOT( setFocus()       ) );
  connect( bcb10, SIGNAL( toggled(bool)), de2, SLOT( setEnabled(bool) ) );
  connect( bcb10, SIGNAL( clicked()    ), de2, SLOT( setFocus()       ) );
  connect( bcb11, SIGNAL( toggled(bool)), le6, SLOT( setEnabled(bool) ) );
  connect( bcb11, SIGNAL( clicked()    ), le6, SLOT( setFocus()       ) );



  QSqlQuery query;
  // тип
  if( !query.exec( "SELECT id, typename FROM types WHERE used = TRUE ORDER BY typename" ) )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { cb1->addItem( sql_get_string( query, 1 ), query.value(0) );
  }

  // производители
  if( !query.exec( "SELECT id, name FROM proizvoditeli ORDER BY name" ) )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { cb2->addItem( sql_get_string( query, 1 ), query.value(0) );
  }

  // поставщики
  if( !query.exec( "SELECT id, name FROM postavshiki ORDER BY name" ) )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { cb3->addItem( sql_get_string( query, 1 ), query.value(0) );
  }

  de1->setDate( QDate::currentDate() );
  de2->setDate( QDate::currentDate() );
}

//=======================================================================================
//
//=======================================================================================
void PrihodFiltrDialog::on_pb1_clicked()
{
  QString str,str2;
  //QMessageBox::critical(this, QObject::tr("Ошибка запроса SQL"), "sdf" );
  str.clear();

  // накладная
  if( bcb11->isChecked() )
  {str2 = le6->text();
   str2.replace(" ","%");
   str += " naklad LIKE '%"+str2+"%' ";
  }

  // тип
  if( bcb2->isChecked() )
  { if( !str.isEmpty() ) str += "AND";
    str2 = cb1->itemData( cb1->currentIndex() ).toString();
    //str2.replace(" ","%");
    str += " prihod.type = '" + str2 + "' ";
  }

  // название
  if( bcb3->isChecked() )
  { if( !str.isEmpty() ) str += "AND";
    str2 = le2->text();
    str2.replace(" ","%");
    str += " prihod.name LIKE '%"+str2+"%' ";
  }

  // производитель
  if( bcb5->isChecked() )
  { if( !str.isEmpty() ) str += "AND";
    str += " prihod.proizvoditel = '"
           + cb2->itemData( cb2->currentIndex() ).toString() + "' ";
  }

  // поставщик
  if( bcb6->isChecked() )
  { if( !str.isEmpty() ) str += "AND";
    str += " zakupki.postavshik = '"
           + cb3->itemData( cb3->currentIndex() ).toString() + "' ";
  }

  // счет
  if( bcb7->isChecked() )
  { if( !str.isEmpty() ) str += "AND";
    str2 = le4->text();
    str2.replace(" ","%");
    str += " zakupki.schet LIKE '%"+str2+"%' ";
  }

  // место
  if( bcb8->isChecked() )
  { if( !str.isEmpty() ) str += "AND";
    str += " mesto LIKE '%"+le5->text()+"%' ";
  }

  // начальная дата
  if( bcb9->isChecked() )
  { if( !str.isEmpty() ) str += "AND";
    str += " prihod.date >= '"+de1->date().toString("yyyy-MM-dd")+"' ";
  }
  // конечная дата
  if( bcb10->isChecked() )
  { if( !str.isEmpty() ) str += "AND";
    str += " prihod.date <= '"+de2->date().toString("yyyy-MM-dd")+"' ";
  }

  if( !str.isEmpty() ) str = " WHERE " + str;

  mainwindow->tw_prihodHist->query_str_limit.clear();
  if( !bcb12->isChecked() )
  { mainwindow->tw_prihodHist->query_str_limit = "LIMIT 0,200 ";
  }

  mainwindow->tw_prihodHist->query_str_where=str;
  mainwindow->tw_prihodHist->refresh();
}

//#######################################################################################
//
//#######################################################################################
PrihodAddDialog::PrihodAddDialog( QWidget *parent,int id_arg,int zakupka_arg )
  : QDialog( parent ), id( id_arg ), zakupka( zakupka_arg )
{
  QString str,str2;
  QStringList sl;

  setupUi( this );
  if( id )  setWindowTitle( "Изменить данные в приходе");
  else      setWindowTitle( "Добавить приход");

  connect( pbOK,     SIGNAL( clicked() ),  this, SLOT( accept() ) );
  connect( pbCANCEL, SIGNAL( clicked() ),  this, SLOT( reject() ) );

  QSqlQuery query;
  if( !query.exec( "SELECT id, typename FROM types WHERE used = TRUE ORDER BY typename" ) )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { cb1->addItem( sql_get_string( query, 1 ), query.value(0) );
  }

  if( !query.exec( "SELECT id, name FROM proizvoditeli ORDER BY name" ) )
  {  sql_error_message( query, this );
	   return;
  }
  cb2->addItem( "", 0 );
  while( query.next() )
  { cb2->addItem( sql_get_string( query, 1 ), query.value(0) );
  }

  if( !query.exec( "SELECT  DISTINCT name,type FROM prihod ORDER BY name ASC" ) )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { str = sql_get_string( query, 0 );
    sl << str;
    int a = query.value(1).toInt();
    if( map.contains( str ) && ( map[str] != a ) )
    { QMessageBox::critical( this, app_header,
        "Ошибка! База прихода содержит несколько одинаковых наименований с разными типами!\n\n" + str );
    }
    map[ str ] = a;
  }
  le2_completer = new QCompleter( sl, this );
  le2_completer->setCaseSensitivity(Qt::CaseInsensitive);
  le2->setCompleter(le2_completer);

  sl.clear();
  if( !query.exec( "SELECT  DISTINCT naklad FROM prihod ORDER BY id DESC LIMIT 10" ) )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { str = sql_get_string( query, 0 );
    sl << str;
  }
  le10_completer = new QCompleter( sl, this );
  le10_completer->setCaseSensitivity(Qt::CaseInsensitive);
  le10->setCompleter(le10_completer);


  le3->setValidator( new QDoubleValidator(this) );
  le4->setValidator( new QIntValidator(this) );
  le6->setValidator( new QIntValidator(this) );

  if( zakupka )
  { query.prepare( "SELECT type,name "
                 "FROM zakupki WHERE id = ?" );
    query.addBindValue( zakupka );
    if( !query.exec() )
    {  sql_error_message( query, this );
	     return;
    }
    while( query.next() )
    { cb1 ->setCurrentIndex( cb1->findData( query.value(0) ) );             // тип
      le2 ->setText( sql_get_string( query, 1 ) ); // наименование
    }
  }

  if( id || zakupka ) bcb1->setChecked( false );
  connect( le2, SIGNAL( textChanged( QString ) ), this, SLOT(  my1( QString ) ) );
  setFocus();

  if( id == 0 )
  {  le4->hide();
     label_5->hide();
     pb1->setEnabled( false );
     return;
  }

  query.prepare( "SELECT type,name,kod,proizvoditel,naklad,price,"
                 "kolichestvo,ostatok,mesto,notes "
                 "FROM prihod WHERE id = ?" );
  query.addBindValue( id );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { cb1 ->setCurrentIndex( cb1->findData( query.value(0) ) );             // тип
    le2 ->setText( sql_get_string( query, 1 ) );                          // наименование
    le9 ->setText( sql_get_string( query, 2 ) );                          // код
    cb2 ->setCurrentIndex( cb2->findData( query.value(3) ) );             // производитель
    le10->setText( sql_get_string( query, 4 ) );                          // накладная
    le3->setText( query.value(5).toString() );                            // цена
    le6->setText( query.value(6).toString() );                            // кол-во
    le4->setText( query.value(7).toString() );                            // остаток
    str = sql_get_string( query, 8 );                                     // место
    str2 = query.value(9).toString();
   //QMessageBox::critical(this, QObject::tr("Ошибка запроса SQL"), QString::number(cb1->findData( query.value(0) ) ) );
    le1 ->clear();
    le1 ->insertPlainText( str2 ); // примечания
  }

  le_mesto->setText( str );
  setFocus();
}

//=======================================================================================
//
//=======================================================================================
void PrihodAddDialog::my1( QString str )
{
  int i;
  i = cb1->findData( map[str] );
  if( i < 0 ) return;
  cb1->setCurrentIndex( i );
}

//=======================================================================================
//
//=======================================================================================
void PrihodAddDialog::on_pb1_clicked()
{
  QString str = "Копировать карточку: \n\n" + le2->text() + " ?";
  QMessageBox mb( app_header,  str,
                  QMessageBox::Warning,
                  QMessageBox::Yes,
                  QMessageBox::No | QMessageBox::Default | QMessageBox::Escape ,
                  QMessageBox::NoButton  );
  mb.setButtonText(QMessageBox::Yes, "Копировать");
  mb.setButtonText(QMessageBox::No,  "Отмена");
  if( mb.exec() == QMessageBox::Yes )
  { id = 0;
    accept();
    return;
  }
  reject();
}

//=======================================================================================
//
//=======================================================================================
void PrihodAddDialog::on_pb2_clicked()
{
  le1->insertPlainText( "нет документов" );
}

//=======================================================================================
//
//=======================================================================================
void PrihodAddDialog::accept()
{
  if( le2->text().isEmpty() )
  { QMessageBox::information(this, "Склад",
    "Поле \"Наименование\" не заполненно!"  );
    return;
  }

  QSqlQuery query;
  QString str,mesto;

  if( bcb1->isChecked() )
  { le3->setText( QString::number(le3->text().toDouble() * 1.18) );
  }

  mesto = le_mesto->text();

  if( zakupka || ( id == 0 ) )
  { le4->setText( le6->text() );
  }

  if( !query.exec(" LOCK TABLES prihod WRITE, zakupki WRITE ") )
  {  sql_error_message( query, this );
	   return;
  }

  str = (id) ? " UPDATE prihod " : " INSERT INTO prihod ";
  str +=  " SET type = ?,name = ?, kod = ?, "
          " proizvoditel = ?, naklad = ?, "
          " mesto = ?, ostatok = ?, kolichestvo = ?, price = ?, notes = ?, ";
  str += (id) ? " user2 = ?, date2 = ? ": " user = ?, date = ?, zakupka = ?";
  if( id ) str += " WHERE id = ? ";

  query.prepare( str );
  query.addBindValue( cb1->itemData( cb1->currentIndex() ) ); // тип
  query.addBindValue( le2->text().toUtf8() );                 // наименование
  query.addBindValue( le9->text().toUtf8() );                 // код
  query.addBindValue( cb2->itemData( cb2->currentIndex() ) ); // производитель
  query.addBindValue( le10->text().toUtf8() );                // накладная
  query.addBindValue( mesto );                                // место
  query.addBindValue( le4 ->text().toInt() );                 // остаток
  query.addBindValue( le6 ->text().toInt() );                 // количество
  query.addBindValue( le3->text().toDouble() );               // цена
  query.addBindValue( le1-> toPlainText().toUtf8() );         // примечания
  query.addBindValue( user_id );
  query.addBindValue( QDate::currentDate() );
  if( id==0 )  query.addBindValue( zakupka );                 // закупка
  if( id    )  query.addBindValue( id );

  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  query.prepare( "UPDATE prihod SET type = ? WHERE name = ?" );
  query.addBindValue( cb1->itemData( cb1->currentIndex() ) ); // тип
  query.addBindValue( le2->text().toUtf8() );                 // наименование
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  if( zakupka )
  { query.prepare( "UPDATE zakupki SET polucheno = polucheno + ? WHERE id = ?" );
    query.addBindValue( le6 ->text().toInt() );                 // количество
    query.addBindValue( zakupka );
    if( !query.exec() )
    {  sql_error_message( query, this );
	   return;
    }
    sql_action_zakupka_color( zakupka );
  }

  if( !query.exec(" UNLOCK TABLES ") )
  {  sql_error_message( query, this );
	   return;
  }


  done(  QDialog::Accepted  );
}

//#######################################################################################
//
//#######################################################################################
ZamenaAddDialog::ZamenaAddDialog( QWidget *parent, int id_arg )
  : QDialog( parent ), id( id_arg )
{
  setupUi( this );

  connect( le1,  SIGNAL( textChanged(const QString) ), SLOT( my1() )  );

  tw->clear_query_fields();
  tw->query_str_pk_field = " MAX(t.id)";
  tw->add_query_field( "Наименование", 200,  "t.name"           );
  tw->add_query_field( "Остаток",      70,  "SUM(t.ostatok)"    );
  tw->query_str_main = " FROM ( SELECT id,type,name,ostatok FROM prihod "
                       "  UNION SELECT 0,type,name,0        FROM zakupki) AS t ";
  tw->query_str_order_by = " GROUP BY t.name ORDER BY t.name ASC ";
  tw->setShownFields( "1" );
  tw->horizontalHeader()->setStretchLastSection( true );

  QSqlQuery query;
  query.prepare( " SELECT kompl.type,kompl.name,sostav.status2 "
                 " FROM kompl LEFT JOIN sostav ON sostav.id = kompl.sostav "
                 " WHERE kompl.id = ?" );
  query.addBindValue( id );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  while( query.next() )
  { type = query.value(0).toInt();
    le1->setText( sql_get_string( query,1 ) );
  }
  setWindowTitle( "Замена:" + le1->text() );
}

//=======================================================================================
//
//=======================================================================================
void ZamenaAddDialog::my1()
{
  QString str;

  str = " "+le1->text()+" ";
  str.replace(" ","%");

  tw->query_str_where = QString(" WHERE t.type = %1 AND t.name LIKE \'%2\'")
     .arg( type )
     .arg( str );
  tw->refresh();
}

//=======================================================================================
//
//=======================================================================================
void ZamenaAddDialog::on_pb_add_zamena_clicked()
{
  QSqlQuery query;
  QString str;

  QList<QTableWidgetSelectionRange> slist = tw->selectedRanges();

  if( slist.isEmpty() )
  { QMessageBox::warning( this, app_header, "Строка таблицы не выбрана." );
    return;
  }

  str = tw->item( slist.at(0).bottomRow(), 0 )->text();
  query.prepare( " SELECT id FROM zamena WHERE kompl = ? AND name = ? " );
  query.addBindValue( id );
  query.addBindValue( str );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  if( query.size() )
  { QMessageBox::critical( this, app_header, "Невозможно добавить замену:\n"
                                "элемент с таким именем уже есть." );
    return;
  }
  query.prepare( " INSERT INTO zamena ( kompl, name ) VALUES ( ?,? )" );
  query.addBindValue( id );
  query.addBindValue( str );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  done(  QDialog::Accepted  );
}

//=======================================================================================
//
//=======================================================================================
void ZamenaAddDialog::on_pb_rename_clicked()
{
  QSqlQuery query;
  int zamena_id;

  QList<QTableWidgetSelectionRange> slist =  tw->selectedRanges();

  if( slist.isEmpty() )
  { QMessageBox::warning( this, app_header, "Строчка таблицы не выбрана." );
    return;
  }

  QString str = tw->item( slist.at(0).bottomRow(), 0 )->text();

  query.prepare( "SELECT zamena.id, zamena.snato FROM zamena,kompl "
                 "    WHERE zamena.kompl = kompl.id  "
                 "      AND zamena.name = kompl.name "
                 "      AND kompl.id = ? " );
  query.addBindValue( id );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  if( query.size() == 1 )
  { query.next();
    if( query.value(1) == 0 )
    { zamena_id = query.value(0).toInt();
      query.prepare( " SELECT id FROM zamena WHERE kompl = ? AND name = ? " );
      query.addBindValue( id );
      query.addBindValue( str );
      if( !query.exec() )
      {  sql_error_message( query, this );
	       return;
      }
      if( query.size() )
      { QMessageBox::critical( this, app_header, "Невозможно переименовать:\n"
                                    "элемент с таким именем уже есть." );
        return;
      }
      query.prepare( "UPDATE kompl  SET name = ? WHERE id = ?" );
      query.addBindValue( str );
      query.addBindValue( id  );
      if( !query.exec() )
      {  sql_error_message( query, this );
	       return;
      }
      query.prepare( "UPDATE zamena SET name = ? WHERE id = ?" );
      query.addBindValue( str );
      query.addBindValue( zamena_id );
      if( !query.exec() )
      {  sql_error_message( query, this );
	       return;
      }
      done( QDialog::Accepted );
      return;
    } else
    { QMessageBox::critical( this, app_header, "Переименовать невозможно:\n"
                             "количество снятых деталей не равно нулю.\n" );
      return;
    }
  }
  QMessageBox::critical( this, app_header, QString("Переименовать невозможно:\n"
                             "ОШИБКА БАЗЫ. [KOMPL_ID = %1]").arg( id ) );
}

//#######################################################################################
//
//#######################################################################################
ZakupkiAddDialog::ZakupkiAddDialog( QWidget *parent,int id_arg )
  : QDialog( parent ), id( id_arg )
{
  QString str;

  setupUi( this );
  if( id )  setWindowTitle( "Изменить данные в в закупке");
  else      setWindowTitle( "Добавить закупку");

  connect( pbOK,     SIGNAL( clicked() ),  this, SLOT( accept() ) );
  connect( pbCANCEL, SIGNAL( clicked() ),  this, SLOT( reject() ) );

  if( id==0 ) pb1->setEnabled( false );

  QSqlQuery query;
  if( !query.exec( "SELECT id, typename FROM types WHERE used = TRUE ORDER BY typename" ) )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { cb1->addItem( sql_get_string( query, 1 ), query.value(0) );
  }
  //-------------------------------------------------------------------------
  if( !query.exec( "SELECT id, name FROM postavshiki ORDER BY name" ) )
  {  sql_error_message( query, this );
	   return;
  }
  cb2->addItem( "", 0 );
  while( query.next() )
  { cb2->addItem( sql_get_string( query, 1 ), query.value(0) );
  }
  //-------------------------------------------------------------------------
  if( !query.exec( " SELECT DISTINCT name,type FROM "
                   " ( SELECT name,type FROM prihod  UNION "
                   "   SELECT name,type FROM zakupki   ) AS t1 "
                   "   ORDER BY name ASC " ) )
  {  sql_error_message( query, this );
	   return;
  }
  QStringList sl;
  while( query.next() )
  { str = sql_get_string( query, 0 );
    sl << str;
    map[ str ] = query.value(1).toInt();
  }
  le1_completer = new QCompleter( sl, this );
  le1_completer->setCaseSensitivity(Qt::CaseInsensitive);
  le1->setCompleter(le1_completer);
  connect( le1, SIGNAL( textChanged( QString ) ),
           this, SLOT(  my1( QString ) ) );
  //-------------------------------------------------------------------------
  cb5->clear();
  if( !query.exec( "SELECT DISTINCT schet FROM zakupki ORDER BY id DESC" ) )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { str = sql_get_string( query, 0 );
    if( !str.isEmpty() ) cb5->addItem( str );
  }
  if( id==0 ) cb5->clearEditText();
  //-------------------------------------------------------------------------
  cb6->clear();
  if( !query.exec( "SELECT DISTINCT platej FROM zakupki ORDER BY id DESC" ) )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { str = sql_get_string( query, 0 );
    if( !str.isEmpty() ) cb6->addItem( str );
  }
  if( id==0 ) cb6->clearEditText();
  //-------------------------------------------------------------------------
  le6->setValidator( new QIntValidator(this) );

  query.prepare( "SELECT type,name,n,postavshik,schet,platej,color,notes "
                 "FROM zakupki WHERE id = ?" );
  query.addBindValue( id );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { cb1 ->setCurrentIndex( cb1->findData( query.value(0) ) );             // тип
    le1 ->setText( sql_get_string( query, 1 ) );                          // наименование
    le6->setText( query.value(2).toString() );                            // кол-во
    cb2 ->setCurrentIndex( cb2->findData( query.value(3) ) );             // поставщик
    cb5 ->setCurrentIndex( cb5->findText( sql_get_string( query, 4 ) ) ); // счет
    cb6 ->setCurrentIndex( cb6->findText( sql_get_string( query, 5 ) ) ); // платеж
    te_notes->setPlainText( query.value(7).toString() ); // примечания
    if( query.value(6).toInt() < 30 ) pb1->setEnabled( false );
  }
}

//=======================================================================================
//
//=======================================================================================
void ZakupkiAddDialog::my1( QString str )
{
  int i;
  i = cb1->findData( map[str] );
  if( i < 0 ) return;
  cb1->setCurrentIndex( i );
}

//=======================================================================================
//
//=======================================================================================
void ZakupkiAddDialog::on_pb1_clicked()
{
  PrihodAddDialog dialog(this, 0, id );
  dialog.exec();
  mainwindow->on_action_Pereraschet_triggered();
  done( QDialog::Rejected );
}

//=======================================================================================
//
//=======================================================================================
void ZakupkiAddDialog::accept()
{
  QString str;

  if( le1->text().isEmpty() )
  { QMessageBox::information(this,  "Склад" ,
     "Поле \"Наименование\" не заполненно!"  );
    return;
  }

  QSqlQuery query;
  str = (id) ? " UPDATE zakupki " : " INSERT INTO zakupki ";
  str +=  " SET type = ?,name = ?, n = ?, "
          " postavshik = ?, schet = ?, "
          " platej = ?, notes = ?, ";
  str += (id) ? " user2 = ?, date2 = ? " : " user = ?, date = ?, color = 30 ";

  if( id ) str += " WHERE id = ? ";

  query.prepare( str );
  query.addBindValue( cb1->itemData( cb1->currentIndex() ) ); // тип
  query.addBindValue( le1->text() );                          // наименование
  query.addBindValue( le6->text().toInt() );                  // кол-во
  query.addBindValue( cb2->itemData( cb2->currentIndex() ) ); // поставщик
  query.addBindValue( cb5->currentText().toUtf8() );          // счет
  query.addBindValue( cb6->currentText().toUtf8() );          // платеж
  query.addBindValue( te_notes->toPlainText() );              // примечания
  query.addBindValue( user_id );
  query.addBindValue( QDate::currentDate() );
  if( id ) query.addBindValue( id );

  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  done( QDialog::Accepted );
}

//#######################################################################################
//
//#######################################################################################
UsersDialog::UsersDialog( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  setWindowTitle( app_header );

  connect( tw, SIGNAL(  currentCellChanged(int,int,int,int) ),
           this, SLOT( my1() ) );

  tw->clear_query_fields();
  tw->query_str_pk_field = "users.id";
  tw->add_query_field( "Имя",            200,  "username"        );
  tw->add_query_field( "Логин",          100,  "nickname"        );
  tw->query_str_main =
    " FROM users ";
  tw->query_str_order_by =
      " ORDER BY username ASC ";
  tw->setShownFields( "1,1,1,1,1" );
  tw->refresh();

}

//=======================================================================================
//
//=======================================================================================
void UsersDialog::on_pb1_clicked()
{
  int i;
  if( tw->get_selected_id() <= 0 ) return;

  i = -1;
  if( !bcb_7->isChecked() ) i &= (~USER_PERMISSION_USERS      );
  if( !bcb_8->isChecked() ) i &= (~USER_PERMISSION_KOMPL_SIGN );
  QSqlQuery query;
  query.prepare( " UPDATE users "
                 " SET nickname = ?, username = ?, passw = ?, "
                 " permissions = ? WHERE id = ? " );
  query.addBindValue( le_login    -> text().toUtf8() );
  query.addBindValue( le_username -> text().toUtf8() );
  query.addBindValue( le_passw    -> text().toUtf8() );
  query.addBindValue( i );
  query.addBindValue( tw->get_selected_id() );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  tw->refresh();
}

//=======================================================================================
//
//=======================================================================================
void UsersDialog::on_pb2_clicked()
{
 QSqlQuery query;
  query.prepare( " INSERT INTO users ( username, nickname, passw, permissions ) "
                 " VALUES (\'Новый пользователь\',\'guest\',\'\',0 ) " );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  tw->refresh();
}

//=======================================================================================
//
//=======================================================================================
void UsersDialog::on_pb3_clicked()
{
  if( tw->get_selected_id() <= 0 ) return;
  QSqlQuery query;
  query.prepare( " DELETE FROM users WHERE id = ? " );
  query.addBindValue( tw->get_selected_id() );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  tw->refresh();
}

//=======================================================================================
//
//=======================================================================================
void UsersDialog::my1()
{
  int i;
  QSqlQuery query;
  query.prepare( "SELECT nickname, username, passw, permissions FROM users WHERE id = ? " );
  query.addBindValue( tw->get_selected_id() );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { le_login    -> setText( sql_get_string( query, 0 ) );
    le_username -> setText( sql_get_string( query, 1 ) );
    le_passw    -> setText( sql_get_string( query, 2 ) );
    i            = query.value(3).toInt();

    bcb_7->setChecked( i & USER_PERMISSION_USERS      );
    bcb_8->setChecked( i & USER_PERMISSION_KOMPL_SIGN );
  }
}
