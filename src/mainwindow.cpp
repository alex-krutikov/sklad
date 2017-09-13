#include <QtGui>
#include <QtSql>

#include "mainwindow.h"
#include "main.h"
#include "misc.h"
#include "dialogs.h"
#include "dialogs2.h"
#include "price.h"
#include "users.h"
#include "sqlactions.h"

#include <QShortcut>
#include <QMessageBox>
#include <QProgressDialog>
#include <QInputDialog>
#include <QPrintDialog>
#include <QPrinter>

//#######################################################################################
/// главное окно
//#######################################################################################
MainWindow::MainWindow()
{
  setupUi( this );
  setWindowTitle( app_header );
  statusbar->hide();

  prihod_filtr_dialog = new PrihodFiltrDialog( this );

  QSettings settings( QSETTINGS_PARAM );
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(600, 400)).toSize();
  resize(size);
  move(pos);

  splitter->setChildrenCollapsible( false );
  splitter->restoreState(settings.value("splitter").toByteArray());
  splitter->setChildrenCollapsible( false );
  splitter_3->setChildrenCollapsible( false );
  splitter_3->restoreState(settings.value("splitter_3").toByteArray());
  splitter_3->setChildrenCollapsible( false );

  tabWidget->setCurrentIndex( settings.value("tabindex", 0).toInt()  );

  if( !(permissions & USER_PERMISSION_USERS ))
     action_Users->setVisible( false );

  QShortcut *shortcut1 = new QShortcut(QKeySequence(Qt::Key_F8), this);
  connect( shortcut1,  SIGNAL(  activated() ),
           this,             SLOT( my_Delete()) );
  QShortcut *shortcut2 = new QShortcut(QKeySequence(Qt::Key_Delete), this);
  connect( shortcut2,  SIGNAL(  activated() ),
           this,             SLOT( my_Delete()) );


  connect( tw_prihodHist,  SIGNAL( cellActivated(int,int) ),
           this,             SLOT( my1A()) );
  connect( le_prihodFiltr,  SIGNAL( textChanged(const QString&) ),
           this,              SLOT(  my1(QString) ) );
  connect( pb_kompl_addZamena,    SIGNAL( clicked() ),
           tw_komplKomp,         SLOT( add_zamena() ) );
  connect( pb_komplStatus,    SIGNAL( clicked() ),
           tw_komplKomp,      SLOT( status() ) );
  connect( tabWidget,     SIGNAL( currentChanged(int)),
           this,          SLOT( my3(int) ) );
  connect( tw_zakupkiHist,  SIGNAL( cellActivated(int,int) ),
           this,             SLOT( my4()) );
  connect( le_skladFiltr,  SIGNAL( textChanged(const QString&) ),
           this,             SLOT( my5()) );
  connect( cb_sklad_type,  SIGNAL( currentIndexChanged(int) ),
           this,             SLOT( my5()) );
  connect( le_zakupkiFiltr,  SIGNAL( textChanged(const QString&) ),
           this,             SLOT( my6()) );
  connect( le_zakupkiFiltrSchet,  SIGNAL( textChanged(const QString&) ),
           this,                  SLOT( my6()) );
  connect( cb_zakupkiFiltrPostavshik,  SIGNAL( currentIndexChanged(int) ),
           this,                       SLOT( my6()) );
  connect( cb_sklad_type_zakupki,  SIGNAL( currentIndexChanged(int) ),
           this,             SLOT( my6()) );
  connect( le_rashodFiltr,  SIGNAL( textChanged(const QString&) ),
           this,             SLOT( my7()) );
  connect( tw_komplPrih, SIGNAL(cellActivated(int,int)),
           this, SLOT(on_pb_komplSnat_clicked()) );
  connect( tw_komplKomp,    SIGNAL( cellActivated(int,int) ),
           this,            SLOT( on_pb_komplSnat_clicked() ) );
  connect( projects_widget, SIGNAL( pereraschet_needed() ),
           this,            SLOT(   pereraschet() ) );

  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));
  timer->start(100);


  //===========================================================================
  // Наличие
  //
  tw_sklad->clear_query_fields();
  tw_sklad->rows_highlighting = true;
  tw_sklad->query_str_pk_field = "MAX(prihod.id)";
  tw_sklad->add_query_field( "Тип",          100,  "MAX(typename)"   );
  tw_sklad->add_query_field( "Наименование", 200,  "prihod.name"           );
  tw_sklad->add_query_field( "Остаток",       70,  "SUM(ostatok)"          );
  tw_sklad->add_query_field( "Место",        200,  "GROUP_CONCAT(DISTINCT mesto)"             );
  tw_sklad->add_query_field( "Цена",          70,  "SUM(price*kolichestvo)/SUM(kolichestvo) " );
  tw_sklad->query_str_main =
    " FROM prihod "
    " LEFT JOIN types           ON prihod.type = types.id ";
  tw_sklad->query_str_group_by = " GROUP BY name ";
  tw_sklad->query_str_order_by = " ORDER BY MAX(typename), name ASC ";
  tw_sklad->query_str_limit = "LIMIT 0,200 ";
  tw_sklad->setShownFields( settings.value("sklad", "").toString() );


  //===========================================================================
  // Закупки
  //
  tw_zakupkiHist->clear_query_fields();
  tw_zakupkiHist->query_str_pk_field = "zakupki.id";
  tw_zakupkiHist->query_str_color_field = "zakupki.color";
  tw_zakupkiHist->add_query_field( "Тип",          100,  "typename"              );
  tw_zakupkiHist->add_query_field( "Наименование", 200,  "zakupki.name"          );
  tw_zakupkiHist->add_query_field( "Pаименование", 200,  "zakupki.name"          );
  tw_zakupkiHist->add_query_field( "Кол-во",        70,  "n"                     );
  tw_zakupkiHist->add_query_field( "Получено",      70,  "polucheno"             );
  tw_zakupkiHist->add_query_field( "Поставщик",    100,  "postavshiki.name"      );
  tw_zakupkiHist->add_query_field( "Счет",         200,  "schet"                 );
  tw_zakupkiHist->add_query_field( "№ платежа",    100,  "platej"                );
  tw_zakupkiHist->add_query_field( "Примечания",   200,  "notes"                 );
  tw_zakupkiHist->add_query_field( "Оператор",     100,  "users1.username"       );
  tw_zakupkiHist->add_query_field( "Дата",          70,  "zakupki.date"          );
  tw_zakupkiHist->add_query_field( "Оператор2",    100,  "users2.username"       );
  tw_zakupkiHist->add_query_field( "Дата2",         70,  "zakupki.date2"         );

  tw_zakupkiHist->query_str_main =
    " FROM zakupki "
      " LEFT JOIN types           ON zakupki.type = types.id "
      " LEFT JOIN postavshiki     ON zakupki.postavshik = postavshiki.id "
      " LEFT JOIN users AS users1 ON zakupki.user  = users1.id "
      " LEFT JOIN users AS users2 ON zakupki.user2 = users2.id ";
  tw_zakupkiHist->query_str_order_by =
      " ORDER BY zakupki.color ASC, zakupki.id DESC ";
  tw_zakupkiHist->setShownFields( settings.value("zakupki", "").toString() );

  //===========================================================================
  // Приход
  //
  tw_prihodHist->clear_query_fields();
  tw_prihodHist->rows_highlighting = true;
  tw_prihodHist->query_str_pk_field = "prihod.id";
  tw_prihodHist->add_query_field( "Тип",          100,  "typename"              );
  tw_prihodHist->add_query_field( "Наименование", 200,  "prihod.name"           );
  tw_prihodHist->add_query_field( "Примечания",   200,  "prihod.notes"          );
  tw_prihodHist->add_query_field( "Код",          100,  "prihod.kod"            );
  tw_prihodHist->add_query_field( "Место",         70,  "mesto", Qt::AlignCenter  );
  tw_prihodHist->add_query_field( "Остаток",       70,  "ostatok"               );
  tw_prihodHist->add_query_field( "Производитель",100,  "proizvoditeli.name"    );
  tw_prihodHist->add_query_field( "Поставщик",    100,  "postavshiki.name"      );
  tw_prihodHist->add_query_field( "Счет",         200,  "schet"                 );
  tw_prihodHist->add_query_field( "Накладная",    200,  "prihod.naklad"         );
  tw_prihodHist->add_query_field( "Кол-во",        70,  "kolichestvo"           );
  tw_prihodHist->add_query_field( "Цена",          70,  "price"                 );
  tw_prihodHist->add_query_field( "Оператор",     100,  "users1.username"       );
  tw_prihodHist->add_query_field( "Дата",          70,  "prihod.date"           );
  tw_prihodHist->add_query_field( "Оператор2",    100,  "users2.username"       );
  tw_prihodHist->add_query_field( "Дата2",         70,  "prihod.date2"          );

  tw_prihodHist->query_str_main =
    " FROM prihod "
      " LEFT JOIN types           ON prihod.type = types.id "
      " LEFT JOIN zakupki         ON prihod.zakupka = zakupki.id "
      " LEFT JOIN proizvoditeli   ON prihod.proizvoditel = proizvoditeli.id "
      " LEFT JOIN postavshiki     ON zakupki.postavshik = postavshiki.id "
      " LEFT JOIN users AS users1 ON prihod.user  = users1.id "
      " LEFT JOIN users AS users2 ON prihod.user2 = users2.id ";
  tw_prihodHist->query_str_order_by =
      " ORDER BY prihod.id DESC ";
  tw_prihodHist->query_str_limit = "LIMIT 0,200 ";
  tw_prihodHist->setShownFields( settings.value("prihod", "").toString() );

  //===========================================================================
  // Расход
  //
  tw_rashodHist->clear_query_fields();
  tw_rashodHist->rows_highlighting  = true;
  tw_rashodHist->row_selection_mode = true;
  tw_rashodHist->query_str_pk_field = "rashod.id";
  tw_rashodHist->add_query_field( "№ Спец.",       70,  "sostav.id"             );
  tw_rashodHist->add_query_field( "Тип",          100,  "types.typename"        );
  tw_rashodHist->add_query_field( "Наименование", 200,  "prihod.name"           );
  tw_rashodHist->add_query_field( "Код",          100,  "prihod.kod"            );
  tw_rashodHist->add_query_field( "Кол-во",        70,  "rashod.n"              );
  tw_rashodHist->add_query_field( "Цена",          70,  "price"                 );
  tw_rashodHist->add_query_field( "Оператор",     100,  "users1.username"       );
  tw_rashodHist->add_query_field( "Дата",          70,  "rashod.date"           );
  tw_rashodHist->add_query_field( "Проект",       150,  "proekt.name"           );
  tw_rashodHist->add_query_field( "Заказ",        150,  "zakaz.name"            );
  tw_rashodHist->add_query_field( "Изделие",      150,  "izdelie.name"          );
  tw_rashodHist->add_query_field( "Cостав",       150,  "sostav.name"           );
  tw_rashodHist->add_query_field( "Место",         70,  "mesto", Qt::AlignCenter );
  tw_rashodHist->add_query_field( "Производитель",100,  "proizvoditeli.name"    );
  tw_rashodHist->add_query_field( "Поставщик",    100,  "postavshiki.name"      );
  tw_rashodHist->add_query_field( "Счет",         100,  "schet"                 );
  tw_rashodHist->add_query_field( "Накладная",    200,  "prihod.naklad"         );
  tw_rashodHist->add_query_field( "№ платежа",    100,  "platej"                );

  tw_rashodHist->query_str_main =
    " FROM rashod "
      " LEFT JOIN prihod          ON rashod.prihod = prihod.id "
      " LEFT JOIN types           ON prihod.type   = types.id "
      " LEFT JOIN zamena          ON rashod.zamena = zamena.id "
      " LEFT JOIN proizvoditeli   ON prihod.proizvoditel = proizvoditeli.id "
      " LEFT JOIN zakupki         ON prihod.zakupka = zakupki.id "
      " LEFT JOIN postavshiki     ON zakupki.postavshik = postavshiki.id "
      " LEFT JOIN kompl           ON zamena.kompl = kompl.id "
      " LEFT JOIN sostav          ON kompl.sostav = sostav.id "
      " LEFT JOIN izdelie         ON sostav.izdelie = izdelie.id "
      " LEFT JOIN zakaz           ON izdelie.zakaz = zakaz.id "
      " LEFT JOIN proekt          ON zakaz.proekt = proekt.id "
      " LEFT JOIN users AS users1 ON rashod.user  = users1.id ";
  tw_rashodHist->query_str_order_by =
      " ORDER BY rashod.id DESC ";
  tw_rashodHist->query_str_limit = " LIMIT 0,150 ";
  tw_rashodHist->setShownFields( settings.value("rashod", "").toString() );

  //===========================================================================
  // Комплектация задания
  //
  tw_komplZad->clear_query_fields();
  tw_komplZad->query_str_pk_field = "sostav.id";
  tw_komplZad->query_str_color_field = "sostav.color";
  tw_komplZad->add_query_field( "№",                  30,  "sostav.id"                          );
  tw_komplZad->add_query_field( "Статус",             80,  "sostav.status2",   Qt::AlignCenter  );
  tw_komplZad->add_query_field( "Зак.",               40,  "sostav.zakupka_status", Qt::AlignCenter  );
  tw_komplZad->add_query_field( "Пр.",                40,  "sostav.prioritet", Qt::AlignCenter  );
  tw_komplZad->add_query_field( "Монтаж",            100,  "sostav.notes"                       );
  tw_komplZad->add_query_field( "Дата",               70,  "sostav.date"                        );
  tw_komplZad->add_query_field( "Спецификация",      200,  "sostav.name"                        );
  tw_komplZad->add_query_field( "Кол-во",             50,  "sostav.n1"                          );
  tw_komplZad->add_query_field( "Изделие",           100,  "izdelie.name"                       );
  tw_komplZad->add_query_field( "Заказ",             100,  "zakaz.name"                         );
  tw_komplZad->add_query_field( "Проект",            100,  "proekt.name"                        );

  tw_komplZad->query_str_main =
    " FROM sostav "
      " LEFT JOIN izdelie           ON sostav.izdelie = izdelie.id "
      " LEFT JOIN zakaz             ON izdelie.zakaz  = zakaz.id "
      " LEFT JOIN proekt            ON zakaz.proekt   = proekt.id ";
  tw_komplZad->query_str_order_by =
      " ORDER BY sostav.prioritet DESC, sostav.date DESC, sostav.id DESC ";
  tw_komplZad->setShownFields( settings.value("komplzad", "").toString() );

  //===========================================================================
  // Комплектация задания --- Приход
  //
  tw_komplPrih->clear_query_fields();
  tw_komplPrih->query_str_pk_field = "prihod.id";
  tw_komplPrih->add_query_field( "Тип",          100,  "typename"              );
  tw_komplPrih->add_query_field( "Наименование", 200,  "prihod.name"           );
  tw_komplPrih->add_query_field( "Код",          100,  "prihod.kod"            );
  tw_komplPrih->add_query_field( "Место",         70,  "mesto", Qt::AlignCenter  );
  tw_komplPrih->add_query_field( "Остаток",       70,  "ostatok"               );
  tw_komplPrih->add_query_field( "Производитель",100,  "proizvoditeli.name"    );
  tw_komplPrih->add_query_field( "Поставщик",    100,  "postavshiki.name"      );
  tw_komplPrih->add_query_field( "Счет",         100,  "schet"                 );
  tw_komplPrih->add_query_field( "Накладная",    200,  "prihod.naklad"         );
  tw_komplPrih->add_query_field( "Кол-во",        70,  "kolichestvo"           );
  tw_komplPrih->add_query_field( "Цена",          70,  "price"                 );
  tw_komplPrih->add_query_field( "Оператор",     100,  "users1.username"       );
  tw_komplPrih->add_query_field( "Дата",          70,  "prihod.date"           );
  tw_komplPrih->add_query_field( "Оператор2",    100,  "users2.username"       );
  tw_komplPrih->add_query_field( "Дата2",         70,  "prihod.date2"          );

  tw_komplPrih->query_str_main =
    " FROM prihod "
      " LEFT JOIN types           ON prihod.type = types.id "
      " LEFT JOIN zakupki         ON prihod.zakupka = zakupki.id "
      " LEFT JOIN proizvoditeli   ON prihod.proizvoditel = proizvoditeli.id "
      " LEFT JOIN postavshiki     ON zakupki.postavshik = postavshiki.id "
      " LEFT JOIN users AS users1 ON prihod.user  = users1.id "
      " LEFT JOIN users AS users2 ON prihod.user2 = users2.id ";
  tw_komplPrih->query_str_order_by =
      " ORDER BY prihod.id DESC ";
  tw_komplPrih->query_str_limit = " LIMIT 0,200 ";
  tw_komplPrih->query_str_where = " WHERE prihod.id = 0 ";
  tw_komplPrih->setShownFields( settings.value("komplprihod", "").toString() );

  //===========================================================================
  cb_sklad_type->addItem( "", 0 );
  cb_sklad_type_zakupki->addItem( "", 0 );
  QSqlQuery query;
  query.prepare( "SELECT id, typename FROM types WHERE used = 1 ORDER BY typename ASC" );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { cb_sklad_type->addItem( query.value(1).toString(),  query.value(0) );
    cb_sklad_type_zakupki->addItem( query.value(1).toString(),  query.value(0) );
  }
  cb_sklad_type->setCurrentIndex(0);
  cb_sklad_type_zakupki->setCurrentIndex(0);

  //===========================================================================
  cb_zakupkiFiltrPostavshik->addItem( "", 0 );
  query.prepare( "SELECT id, name FROM postavshiki ORDER BY name ASC" );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { cb_zakupkiFiltrPostavshik->addItem( query.value(1).toString(),  query.value(0) );
  }
  cb_zakupkiFiltrPostavshik->setCurrentIndex(0);

  //===========================================================================
  on_cb_komplAll_toggled( false );
  my3( tabWidget->currentIndex() );
  //===========================================================================
  cb_rashod_kompl->hide();
  pb_prihodFiltr->hide();
  //===========================================================================
  kompl_status.clear();
  kompl_status << "";                 // 0
  kompl_status << "Не комплектовать"; // 1
  kompl_status << "Не отправлять";    // 2
  //===========================================================================
  sostav_status.clear();
  if( permissions & USER_PERMISSION_KOMPL_SIGN )
  { sostav_status << "Не утвержден";
  }
  sostav_status << "Утвержден";
  sostav_status << "В закупке";
  sostav_status << "В комплектации";
  sostav_status << "В монтаже";
  sostav_status << "В архиве";
  sostav_status << "На разборку";

  if( !( permissions & USER_PERMISSION_KOMPL_SIGN ) )
  { tabWidget->setTabEnabled(0,false);
  }

  tabWidget->removeTab(7);
}

//==============================================================================
/// Вызов меню "поставщики"
//==============================================================================
void MainWindow::on_action_postavshiki_triggered()
{
  Postvshiki dialog( this );
  dialog.exec();
}

//==============================================================================
/// Вызов меню "производители"
//==============================================================================
void MainWindow::on_action_proizvoditeli_triggered()
{
  Proizvoditeli dialog( this );
  dialog.exec();
}

//==============================================================================
/// Нажатие кнопки "Фильтр" на вкладке приход
//==============================================================================
void MainWindow::on_pb_prihodFiltr_clicked()
{
  prihod_filtr_dialog->exec();
}

//==============================================================================
//
//==============================================================================
void MainWindow::on_pb_prihodAdd_clicked()
{
  PrihodAddDialog dialog( this );
  if( dialog.exec() == QDialog::Accepted )
  { on_action_Pereraschet_triggered();
  }
}

//==============================================================================
//
//==============================================================================
void MainWindow::my_Delete()
{
  int i,n;
  QString str;
  QSqlQuery query;

  switch( tabWidget->currentIndex() )
  { //==================== ЗАКУПКИ ===================================================
    case(2):
      {
      i = tw_zakupkiHist->get_selected_id();
      query.prepare( " SELECT name, schet FROM zakupki WHERE id = ? " );
      query.addBindValue( i );
      if( !query.exec() )
      {  sql_error_message( query, this );
	       return;
      }
      if( query.size() != 1 ) return;
      while( query.next() )
      { str = sql_get_string( query, 0 );
        if( sql_get_string( query, 1 ).size() )
        {  QMessageBox::critical(this, app_header,
              tr("Нельзя удалить запись - поле \'Счет\' не пусто!") );
            return;
        }
      }
      str = tr("Удалить закупку\n\n") + str + " ?";
      QMessageBox mb( app_header,  str,
                           QMessageBox::Warning,
                           QMessageBox::Yes,
                           QMessageBox::No | QMessageBox::Default | QMessageBox::Escape ,
                           QMessageBox::NoButton  );
      mb.setButtonText(QMessageBox::Yes, tr("Удалить"));
      mb.setButtonText(QMessageBox::No, tr("Отмена"));
      if( mb.exec() == QMessageBox::Yes )
      {  query.prepare( " DELETE FROM zakupki WHERE id = ? " );
         query.addBindValue( i );
         if( !query.exec() )
         {  sql_error_message( query, this );
   	       return;
         }
      }
      tw_zakupkiHist->refresh();
      }
      break;
    //==================== ПРИХОД ===================================================
    case(3):
      {
      int zakupka_id=0;
      int kolichestvo=0;
      i = tw_prihodHist->get_selected_id();
      if( !query.exec(" LOCK TABLES prihod READ ") )
      {  sql_error_message( query, this );
	     return;
      }
      query.prepare( " SELECT name,zakupka,kolichestvo FROM prihod WHERE id = ? " );
      query.addBindValue( i );
      if( !query.exec() )
      {  sql_error_message( query, this );
	       return;
      }
      if( query.size() != 1 ) return;
      while( query.next() )
      { str = sql_get_string( query, 0 );
        zakupka_id  = query.value(1).toInt();
        kolichestvo = query.value(2).toInt();
      }
      str = tr("Удалить\nзапись о приходе\n\n") + str + " ?";
      QMessageBox mb( app_header,  str,
                           QMessageBox::Warning,
                           QMessageBox::Yes,
                           QMessageBox::No | QMessageBox::Default | QMessageBox::Escape ,
                           QMessageBox::NoButton  );
      mb.setButtonText(QMessageBox::Yes, tr("Удалить"));
      mb.setButtonText(QMessageBox::No, tr("Отмена"));
      if( mb.exec() == QMessageBox::Yes )
      {  if( !query.exec(" LOCK TABLES prihod WRITE, zakupki WRITE ") )
         {  sql_error_message( query, this );
	        return;
         }
         query.prepare( " DELETE FROM prihod WHERE id = ? " );
         query.addBindValue( i );
         if( !query.exec() )
         {  sql_error_message( query, this );
   	       return;
         }

         if( zakupka_id  )
         { query.prepare( " UPDATE zakupki SET polucheno = polucheno - ? WHERE id = ? " );
           query.addBindValue( kolichestvo );
           query.addBindValue( zakupka_id );
           if( !query.exec() )
           {  sql_error_message( query, this );
     	      return;
           }

           sql_action_zakupka_color( zakupka_id );
         }

         if( !query.exec(" UNLOCK TABLES ") )
         {  sql_error_message( query, this );
	        return;
         }
      }
      tw_prihodHist->refresh();
      }
    break;
  //==================== РАСХОД ===================================================
  case(4):
      {
      int zamena_id, prihod_id, z;
      QVector<int> v = tw_rashodHist->get_selected_id_vect();

      if( v.count() == 0 ) return;

      query.prepare( "DROP TEMPORARY TABLE IF EXISTS t1;");
      if( !query.exec() )
      {  sql_error_message( query, this );
	       return;
      }
      query.prepare( "CREATE TEMPORARY TABLE t1 (id INT UNIQUE) ENGINE = MEMORY");
      if( !query.exec() )
      {  sql_error_message( query, this );
	       return;
      }

      query.prepare( "INSERT INTO t1 VALUES (?)" );
      foreach( z, v )
      { query.bindValue(0, z );
        if( !query.exec() )
        {  sql_error_message( query, this );
    	      return;
        }
      }

      str = "Удалить запись о расходе\n и возвратить на склад: \n\n";
      query.prepare( " SELECT name, n, zamena, prihod FROM rashod "
                     " LEFT JOIN prihod ON rashod.prihod = prihod.id "
                     " WHERE rashod.id IN ( SELECT id FROM t1 ) " );
      if( !query.exec() )
      {  sql_error_message( query, this );
	       return;
      }
      while( query.next() )
      { str += query.value(0).toString() + ", ";
      }
      str.chop(2);
      QMessageBox mb( app_header,  str,
                           QMessageBox::Warning,
                           QMessageBox::Yes,
                           QMessageBox::No | QMessageBox::Default | QMessageBox::Escape ,
                           QMessageBox::NoButton  );
      mb.setButtonText(QMessageBox::Yes, "Возвратить на склад");
      mb.setButtonText(QMessageBox::No,  "Отмена");
      if( mb.exec() == QMessageBox::Yes )
      {  foreach( z, v )
         { query.prepare( " SELECT n,zamena,prihod "
                          " FROM rashod WHERE id = ? " );
           query.addBindValue( z );
           if( !query.exec() )
           {  sql_error_message( query, this );
	            return;
           }
           query.next();
           n = query.value(0).toInt();
           zamena_id = query.value(1).toInt();
           prihod_id = query.value(2).toInt();

           query.prepare( " UPDATE zamena SET snato = snato - ? WHERE id = ? " );
           query.addBindValue( n );
           query.addBindValue( zamena_id );
           if( !query.exec() )
           {  sql_error_message( query, this );
	            return;
           }
           query.prepare( " UPDATE prihod SET ostatok = ostatok + ? WHERE id = ? " );
           query.addBindValue( n );
           query.addBindValue( prihod_id );
           if( !query.exec() )
           {  sql_error_message( query, this );
	            return;
           }
           query.prepare( " DELETE FROM rashod WHERE id = ? " );
           query.addBindValue( z );
           if( !query.exec() )
           {  sql_error_message( query, this );
   	         return;
           }
         }
         query.prepare( "DROP TEMPORARY TABLE IF EXISTS t1;");
         if( !query.exec() )
         {  sql_error_message( query, this );
    	      return;
         }
         on_action_Pereraschet_triggered();
         tw_rashodHist->refresh();
      }
      }
    break;
  //==================== КОМПЛЕКТАЦИЯ - ЗАМЕНА =================================
  case(5):
    { if( tw_komplZad->hasFocus() )  // Удаление "На разборку"
      { i = tw_komplZad->get_selected_id();
        if( i < 0 ) return;

        query.prepare( " SELECT name, status2 FROM sostav WHERE id = ? " );
        query.addBindValue( i );
        if( !query.exec() )
        {  sql_error_message( query, this );
           return;
        }
        if( query.size() != 1 )
        { QMessageBox::critical(this,app_header,"Ошибка выборки из базы данных." );
          return;
        }
        query.next();
        if( query.value(1).toString() != "На разборку" )
        { QMessageBox::critical(this, app_header,
            "Нельзя удалить задание:\nего статус не соответствует \"На разборку\"." );
            return;
        }

        str = query.value(0).toString();
        str = "Удалить задание:\n\n" + str + " ?";
        query.prepare( " SELECT SUM(snato) FROM kompl WHERE sostav = ? " );
        query.addBindValue( i );
        if( !query.exec() )
        {  sql_error_message( query, this );
	         return;
        }
        while( query.next() )
        { if( query.value(0).toInt() > 0 )
          { QMessageBox::critical(this, app_header,
                        "Нельзя удалить задание:\nесть снятые со склада компоненты." );
            return;
          }
        }
        QMessageBox mb( app_header,  str,
                       QMessageBox::Warning,
                       QMessageBox::Yes,
                       QMessageBox::No | QMessageBox::Default | QMessageBox::Escape ,
                       QMessageBox::NoButton  );
        mb.setButtonText(QMessageBox::Yes, "Удалить");
        mb.setButtonText(QMessageBox::No, "Отмена");
        if( mb.exec() == QMessageBox::Yes )
        {  query.prepare( " DELETE sostav,kompl,zamena FROM sostav,kompl,zamena "
                          " WHERE sostav.id = ? "
                          " AND kompl.sostav=sostav.id AND zamena.kompl=kompl.id " );
           query.addBindValue( i );
           if( !query.exec() )
           {  sql_error_message( query, this );
   	          return;
           }
           on_action_Pereraschet_triggered();
        }
      }
      else if( tw_komplKomp->hasFocus() ) // Удаление "Замена"
      { QList<QTableWidgetSelectionRange> sr = tw_komplKomp->selectedRanges();
        if( sr.count() == 0 ) return;
        i = sr.at(0).topRow();
        if( i < 0 ) break;
        if( tw_komplKomp->item(i,0)->data(Qt::UserRole+3) == 2 )
        { i = tw_komplKomp->item(i,0)->data(Qt::UserRole+2).toInt();
          query.prepare( " SELECT name FROM zamena WHERE id = ? " );
          query.addBindValue( i );
          if( !query.exec() )
          {  sql_error_message( query, this );
	           return;
          }
          if( query.size() != 1 ) return;
          while( query.next() )
          { str = sql_get_string( query, 0 );
          }
          str = tr("Удалить замену: \n\n") + str + " ?";
          QMessageBox mb( app_header,  str,
                             QMessageBox::Warning,
                             QMessageBox::Yes,
                             QMessageBox::No | QMessageBox::Default | QMessageBox::Escape ,
                             QMessageBox::NoButton  );
          mb.setButtonText(QMessageBox::Yes, "Удалить" );
          mb.setButtonText(QMessageBox::No, "Отмена"   );
          if( mb.exec() != QMessageBox::Yes ) return;
          query.prepare( " DELETE FROM zamena WHERE id = ? " );
          query.addBindValue( i );
          if( !query.exec() )
          { sql_error_message( query, this );
     	      return;
          }
         on_action_Pereraschet_triggered();
       }
      }
    }
    break;
  //==================== ДИФЕЦИТ =================================
  case(6):
    break;
  //==================== ПРОЕКТЫ =================================
  case(0):
    projects_widget->delete_item();
    break;
  }
}

//==============================================================================
//
//==============================================================================
void MainWindow::on_action_Pereraschet_triggered()
{
  int i;
  QStringList sl;
  QSqlQuery query;
  int progrss_i = 0;

  QProgressDialog progress("Перерасчет базы","Отмена", 0,12, this );
  progress.setWindowTitle("Перерасчет");
  progress.setWindowFlag(Qt::WindowCloseButtonHint, false);
  progress.setCancelButton(0);
  progress.show();

  progress.setValue(progrss_i++);

  if( !query.exec( "  LOCK TABLES kompl WRITE, types WRITE, prihod WRITE, "
                   "  zakupki WRITE, sostav WRITE, izdelie WRITE, zamena WRITE " ) )
  {  sql_error_message( query, this );
	 return;
  }

  progress.setValue(progrss_i++);
  qApp->processEvents();

  //========= Статус "не комплектовать" для типа "Модуль" в комплектации ======
  query.prepare(
    " UPDATE kompl LEFT JOIN types ON kompl.type=types.id "
    " SET kompl.status=1 WHERE typename='Модуль' " );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  progress.setValue(progrss_i++);
  qApp->processEvents();
  //==================== Комплектация "need" ===================================
  query.prepare( " UPDATE kompl "
                 " LEFT JOIN sostav  ON kompl.sostav   = sostav.id "
                 " LEFT JOIN izdelie ON sostav.izdelie = izdelie.id "
                 " SET kompl.need = kompl.n*sostav.n1 " );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  progress.setValue(progrss_i++);
  qApp->processEvents();
  //==================== Комплектация "снято" ===================================
  query.prepare( " UPDATE kompl, "
                 "   ( SELECT kompl, SUM( snato ) AS s1 FROM zamena GROUP BY kompl ) AS t1 "
                 "   SET kompl.snato = t1.s1 WHERE t1.kompl=kompl.id" );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  progress.setValue(progrss_i++);
  qApp->processEvents();
  //==================== Комплектация "color" ===================================
   static const char query_text[] =
           "DROP TEMPORARY TABLE IF EXISTS t1;"
           //-------
           "CREATE  TEMPORARY TABLE t1 (name char(30) UNIQUE, s1 int) ENGINE = MEMORY "
           "   SELECT name,SUM(ostatok) AS s1 "
           "   FROM prihod GROUP BY name; "
           //-------
           "DROP TEMPORARY TABLE IF EXISTS t2; "
           //-------
           "CREATE TEMPORARY TABLE t2 (kompl int UNIQUE, name char(30), s2 int ) ENGINE = MEMORY "
           "  SELECT zamena.kompl, MAX(zamena.name) ,SUM(s1) AS s2 "
           "  FROM zamena "
           "  INNER JOIN t1 ON t1.name = zamena.name "
           "  GROUP BY zamena.kompl; "
           //-------
           "UPDATE kompl "
           "  LEFT JOIN   t2  ON t2.kompl = kompl.id "
           "  SET kompl.color =  "
           "  IF( kompl.need <= kompl.snato, 40, "
           "  IF( kompl.need >  t2.s2 OR ISNULL( t2.s2 ) , 10, 0 ) )";
  sl = QString( query_text ).split(';');
  for(i=0; i<sl.count(); i++ )
  { if( !query.exec( sl.at(i) ) )
    {  sql_error_message( query, this );
	     return;
    }
    progress.setValue(progrss_i++);
    qApp->processEvents();
  }
  progress.setValue(progrss_i++);
  qApp->processEvents();
  //==================== Комплектация "color" через статус =======================
  query.prepare( " UPDATE kompl "
                 " SET color = 40 WHERE status = 1 " );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  progress.setValue(progrss_i++);
  qApp->processEvents();
  //==================== Состав "color" ===================================
  query.prepare( " UPDATE sostav "
                 " LEFT JOIN "
                 "   ( SELECT sostav, "
                 "       IF(MIN(color)=MAX(color) AND MAX(color)=40,40, "
                 "         IF(SUM(snato)>0, 30,20 ) ) AS f1 "
                 "     FROM kompl GROUP BY sostav ) AS t1 "
                 "     ON t1.sostav = sostav.id "
                 " SET sostav.color = t1.f1 " );


  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  progress.setValue(progrss_i++);
  qApp->processEvents();

  if( !query.exec( " UNLOCK TABLES " ) )
  {  sql_error_message( query, this );
	   return;
  }

  my3( tabWidget->currentIndex() );
}

//==============================================================================
//
//==============================================================================
void MainWindow::on_action_export_all_kompl_triggered()
{
  int i;
  QString str;
  QSqlQuery query;

  QMap<QString,QString> map;

  map["20"] = "желтый";
  map["40"] = "зеленый";

  query.prepare( " SELECT sostav.id,sostav.color,sostav.status2,sostav.zakupka_status,sostav.prioritet,  "
                 "         sostav.notes, sostav.date, sostav.name, sostav.n1, izdelie.name, "
                 "         zakaz.name, proekt.name "
                 " FROM sostav "
                 " LEFT JOIN izdelie           ON sostav.izdelie = izdelie.id "
                 " LEFT JOIN zakaz             ON izdelie.zakaz  = zakaz.id "
                 " LEFT JOIN proekt            ON zakaz.proekt   = proekt.id "
                 " ORDER BY sostav.prioritet DESC, sostav.date DESC, sostav.id DESC " );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  str += "№\tЦвет\tСтатус\tЗакупка\tПриоритет\tМонтаж\tДата\tСпецификация\tКол-во\tИзделие\tЗаказ\tПроект\r\n";

  while( query.next() )
  { for(i=0;i<12;i++)
    if( i==1 )
    { str += "\"" + map.value( query.value(i).toString() ) + "\"\t";
    } else
    { str += "\"" + query.value(i).toString() + "\"\t";
    }
    str.chop(1);
    str += "\r\n" ;
  }
  str.chop(1);
  QApplication::clipboard()->setText( str );
}

//==============================================================================
//
//==============================================================================
void MainWindow::on_action_Users_triggered()
{
  QMessageBox::information( this, app_header,
    "Данная функция находится в разработке." );
  //UsersDialog dialog(this);
  //UsersDialog2 dialog(this);
  //dialog.exec();
}

//==============================================================================
//
//==============================================================================
void MainWindow::on_pb_zakupkiAdd_clicked()
{
  ZakupkiAddDialog dialog( this );
  if( dialog.exec() == QDialog::Accepted )
  { on_action_Pereraschet_triggered();
  }
}

//==============================================================================
//
//==============================================================================
void MainWindow::prihod_snat(int prihod_id,int zamena_id, int n )
{
  QSqlQuery query;
  query.prepare( "INSERT INTO rashod SET zamena = ?, prihod = ?, n = ?, "
                 "   user = ?, date = ?" );
  query.addBindValue( zamena_id );
  query.addBindValue( prihod_id );
  query.addBindValue( n );
  query.addBindValue( user_id );
  query.addBindValue( QDate::currentDate() );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  query.prepare( "UPDATE zamena SET snato = snato + ? WHERE id = ? ");
  query.addBindValue( n );
  query.addBindValue( zamena_id );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  query.prepare( "UPDATE prihod SET ostatok = ostatok - ? WHERE id = ? ");
  query.addBindValue( n );
  query.addBindValue( prihod_id );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  on_action_Pereraschet_triggered();
}

//==============================================================================
//
//==============================================================================
void MainWindow::on_pb_komplSnat_clicked()
{
  int i,n=0,k,prihod_id,zamena_id;
  bool ok;
  QSqlQuery query;

  i = tw_komplKomp->currentRow();
  if( i < 0 ) return;
  i = tw_komplPrih->currentRow();
  if( i < 0 )
  { tw_komplPrih->selectRow(0);
  }
  i = tw_komplPrih->currentRow();
  if( i < 0 ) return;
  prihod_id = tw_komplPrih->get_selected_id();
  zamena_id = tw_komplKomp->item( tw_komplKomp->currentRow(), 0 )
                 ->data(Qt::UserRole+2).toInt();
  k=0;
  for(i=0;i<100;i++)
  { bool ok;
    int a1 = tw_komplKomp->item( tw_komplKomp->currentRow()-i, 2 )->text().toInt(&ok);
    if( !ok ) continue;
    int a2 = tw_komplKomp->item( tw_komplKomp->currentRow()-i, 3 )->text().toInt(&ok);
    if( !ok ) continue;
    k = a1-a2;
    break;
  }

  query.prepare( "SELECT ostatok FROM prihod WHERE id = ? " );
  query.addBindValue( prihod_id );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { n = query.value(0).toInt();
  }
  if(n==0) return;

  n = QInputDialog::getInt(this, app_header,
                               tr("Количество:"), k, 1, n, 1, &ok);
  if(!ok) return;

  prihod_snat( prihod_id, zamena_id, n );
}

//==============================================================================
//
//==============================================================================
void MainWindow::on_pb_komplCard_clicked()
{ int i;
  i = tw_komplPrih->get_selected_id();
  if( i < 1 ) return;

  PrihodAddDialog dialog( this, i );
  dialog.exec();
  on_action_Pereraschet_triggered();
}


//==============================================================================
//
//==============================================================================
void MainWindow::on_pb_komplPrint_clicked()
{
  QMenu menu;
  QAction *act1 = menu.addAction("Комплектация");
  QAction *act2 = menu.addAction("Отправка");
  QAction *act3 = menu.addAction("Доотправка");
  QAction *act = menu.exec( QCursor::pos() );
  if( act == 0 ) return;
  if(      act == act1 )  { kompl_print(); }
  else if( act == act2 )  { otpravka_print(); }
  else if( act == act3 )  { dootpravka_print(); }
}

//==============================================================================
//
//==============================================================================
void MainWindow::kompl_print()
{
  const int XSIZE = 1000;
  const int YSIZE = XSIZE*297/210;
  const int TABLE_Y = 80;

  QRect rect;
  int id,nn=0;
  id = tw_komplZad->get_selected_id();
  if( id < 1 ) return;

  int n,i,j,w,h,x,cur_y,page;
  const int ww[6] = {300,130,100,120,100,100};
  Qt::Alignment alig;
  QString str,str2;
  QVector<QRect> v;

  QString proekt,zakaz,izdelie,sostav;

  QSqlQuery query;
  query.prepare(" SELECT proekt.name, zakaz.name ,izdelie.name ,sostav.name, sostav.n1 "
                " FROM sostav "
                " LEFT JOIN izdelie ON izdelie.id = sostav.izdelie "
                " LEFT JOIN zakaz   ON zakaz.id   = izdelie.zakaz  "
                " LEFT JOIN proekt  ON proekt.id  = zakaz.proekt   "
                " WHERE sostav.id = ? " );
  query.addBindValue( id );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { proekt  = sql_get_string( query, 0 );
    zakaz   = sql_get_string( query, 1 );
    izdelie = sql_get_string( query, 2 );
    sostav  = sql_get_string( query, 3 );
    nn      = query.value(4).toInt();
  }

  QPrinter printer;
  QPrintDialog printDialog(&printer, this);
  if (printDialog.exec() == QDialog::Accepted)
  {
     QPainter painter( &printer );
     painter.setWindow( 0,0,XSIZE, YSIZE );


     QFont font("Arial");
     font.setBold(   false );
     font.setItalic( false );
     font.setPointSize(11);

     QFont font2("Tahoma");
     font2.setBold(   false );
     font2.setItalic( false );
     font2.setPointSize(11);

     QFont font3(font2);
     font3.setBold(   true );
     font3.setItalic( false );
     font3.setPointSize(11);

     QFont font4(font2);
     font4.setBold(   false );
     font4.setItalic( false );
     font4.setPointSize(14);

     h = font.pointSize()+13;
     i = 0;
     page = 1;
     painter.setFont( font );

     while(1)
     {
       cur_y = TABLE_Y;
       if( page!=1 ) cur_y -= 1*h;

       painter.setFont( font3 );
       x=110;
       w = ww[0];
       rect.setX( x ); x += w ;
       rect.setY( cur_y-4 );
       rect.setWidth( w  );
       rect.setHeight( h );
       v << rect;
       painter.drawText( rect, Qt::AlignVCenter|Qt::AlignLeft, tr("  Наименование")  );
       w = ww[1];
       rect.setX( x ); x += w ;
       rect.setWidth( w  );
       v << rect;
       painter.drawText( rect, Qt::AlignCenter, tr("Номинал")  );
       w = ww[2];
       rect.setX( x ); x += w ;
       rect.setWidth( w  );
       v << rect;
       painter.drawText( rect, Qt::AlignCenter, tr("Требуется")  );
       w = ww[3];
       rect.setX( x ); x += w ;
       rect.setWidth( w  );
       v << rect;
       painter.drawText( rect, Qt::AlignCenter, tr("Снято")  );
       w = ww[4];
       rect.setX( x ); x += w ;
       rect.setWidth( w  );
       v << rect;
       painter.drawText( rect, Qt::AlignCenter, tr("Место")  );
       w = ww[5];
       rect.setX( x ); x += w ;
       rect.setWidth( w  );
       v << rect;
       painter.drawText( rect, Qt::AlignCenter, tr("В наличии")  );

       n = tw_komplKomp->rowCount();
       while(i<n)
       { x = 110;
         cur_y += h;
         if( tw_komplKomp->item(i,0)->data(Qt::UserRole+3) == 10 )
         { painter.setFont( font3 );
           w = ww[0]+ww[1]+ww[2]+ww[3]+ww[4]+ww[5];
           rect.setX( x ); x += w;
           rect.setY( cur_y );
           rect.setWidth( w  );
           rect.setHeight( h );
           painter.save();
           painter.fillRect( rect, QBrush(QColor("gray")) );
           v << rect;
           painter.restore();
           str = tw_komplKomp->item(i,0)->text();
           alig = Qt::AlignVCenter|Qt::AlignLeft;
           painter.drawText( rect, alig, "  " + str );
           i++;
           continue;
         }
         painter.setFont( font );
         if( tw_komplKomp->item(i,0)->data(Qt::UserRole+4).toInt() )
         { painter.drawText( x-15, cur_y+17, "X" );
         }
         for(j=0;j<6;j++)
         { w = ww[j];
           rect.setX( x ); x += w;
           rect.setY( cur_y );
           rect.setWidth( w  );
           rect.setHeight( h );
           str = tw_komplKomp->item(i,j)->text();
           v << rect;
           if(( j==2 )&&(str=="0")) str="";
           alig = Qt::AlignVCenter;
           switch( j )
           { case(0):
               alig |= Qt::AlignLeft;
               str = "  "+str;
               break;
             case(4):
               alig |= Qt::AlignHCenter;
               break;
             case(1):
             case(2):
             case(3):
             case(5):
               alig |= Qt::AlignRight;
               str = str + "  ";
               break;
           }
           painter.drawText( rect, alig, str  );
         }
         i++;
         if( cur_y >  YSIZE - 120 ) break;
       }

       painter.setFont( font4 );
       str2 = tr("Спецификация: %1 (%2 шт.)").arg( sostav ).arg( nn );
       if( page == 1 )
       { painter.setFont( font4 );
         rect.setRect(0,40,1000,30);
         painter.drawText( rect, Qt::AlignCenter, str2  );
       } else
       { painter.setFont( font2 );
         rect.setRect(0,40,1000,60);
         painter.drawText(200,  40,  str2 );
       }

       painter.setFont( font2 );
       painter.drawText(50,  90,       QString("N: %1").arg( id ) );
       painter.drawText(900, 40,       QDate::currentDate().toString("dd.MM.yy") );

       str2  = "проект: " + proekt;
       str2 += "; заказ: " + zakaz;
       str2 += "; изделие: " + izdelie;
       painter.drawText(50,  YSIZE-30, str2 );

       if(( i != n ) || ( page != 1 ))
         painter.drawText(900, YSIZE-30, tr("стр. %1").arg(page) );

       painter.drawRects( v );
       v.clear();

       if( i == n ) break;
       printer.newPage();
       page++;
     }
  }
}

//==============================================================================
//
//==============================================================================
void MainWindow::otpravka_print()
{
  OtpravkaDialog dialog( this );
  dialog.sostav_id = tw_komplZad->get_selected_id();
  dialog.refresh();
  dialog.exec();
}

//==============================================================================
//
//==============================================================================
void MainWindow::dootpravka_print()
{
  QSqlQuery query;
  int id = tw_komplZad->get_selected_id();
  if( id == 0 ) return;

  query.prepare(
    "  SELECT min(rashod.date), max(rashod.date) "
    "  FROM rashod "
    "    LEFT JOIN zamena ON rashod.zamena = zamena.id "
    "    LEFT JOIN kompl  ON zamena.kompl  = kompl.id "
    "  WHERE kompl.sostav = ? " );
  query.addBindValue( id );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  query.next();

  DootpravkaDialog dialog( this );
  dialog.sostav_id = id;
  dialog.de1 ->setDate( query.value(0).toDate() );
  dialog.de2 ->setDate( query.value(1).toDate() );
  dialog.refresh();
  dialog.exec();
}

//==============================================================================
//
//==============================================================================
void MainWindow::on_pb_kompl_rashod_clicked()
{
  int sostav_id = tw_komplZad->get_selected_id();
  if( sostav_id < 0 ) return;

  QString proekt,zakaz,izdelie,sostav;
  int  nn=0;

  QSqlQuery query;
  query.prepare(" SELECT proekt.name, zakaz.name ,izdelie.name ,sostav.name, sostav.n1 "
                " FROM sostav "
                " LEFT JOIN izdelie ON izdelie.id = sostav.izdelie "
                " LEFT JOIN zakaz   ON zakaz.id = izdelie.zakaz "
                " LEFT JOIN proekt  ON proekt.id = zakaz.proekt "
                " WHERE sostav.id = ? " );
  query.addBindValue( sostav_id );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  while( query.next() )
  { proekt  = sql_get_string( query, 0 );
    zakaz   = sql_get_string( query, 1 );
    izdelie = sql_get_string( query, 2 );
    sostav  = sql_get_string( query, 3 );
    nn      = query.value(4).toInt();
  }

  cb_rashod_kompl->setText( "КОМПЛЕКТАЦИЯ:   "
                             + sostav   + "  |  "
                             + izdelie  + "\n"
                             + zakaz    + "  |  "
                             + proekt   + "  |  "
                             + "( "+QString::number(nn)+" шт.)"
                              );
  cb_rashod_kompl->show();
  cb_rashod_kompl->setChecked( true );

  sql_where_rashod_kompl = QString(" sostav.id = %1 ").arg( sostav_id );

  tabWidget->setCurrentIndex(4);
  my7();
}

//==============================================================================
//
//==============================================================================
void MainWindow::on_cb_rashod_kompl_toggled(bool b)
{
  if( !b )
  { sql_where_rashod_kompl.clear();
    cb_rashod_kompl->hide();
    my7();
  }
}

//==============================================================================
//
//==============================================================================
void MainWindow::on_cb_rashod_show_all_toggled(bool b)
{
  tw_rashodHist->query_str_limit = b ? "" : " LIMIT 0,150 ";
  my7();
}

//==============================================================================
//
//==============================================================================
void MainWindow::my1( const QString & text )
{
  prihod_filtr_dialog->bcb3->setChecked( !text.isEmpty() );
  prihod_filtr_dialog->le2->setText( text );
  prihod_filtr_dialog->on_pb1_clicked();
}

//==============================================================================
//
//==============================================================================
void MainWindow::my1A()
{
  PrihodAddDialog dialog( this, tw_prihodHist->get_selected_id() );
  if( dialog.exec() == QDialog::Accepted )
  { on_action_Pereraschet_triggered();
  }
}

//==============================================================================
//
//==============================================================================
void MainWindow::my5()
{
  QString str2;
  int i;

  str2 = " "+le_skladFiltr->text()+" ";
  i = cb_sklad_type->itemData( cb_sklad_type->currentIndex()  ).toInt();
  str2.replace(" ","%");
  str2 = tr(" WHERE name LIKE \'%1\' ").arg(str2);
  if( i )
  { str2 += QString(" AND type = %1 ").arg(i);
  }


  tw_sklad->query_str_where = str2;
  tw_sklad->refresh();
}

//==============================================================================
// изменение данных в поле фильтра закупок
//==============================================================================
void MainWindow::my6()
{
  int i=cb_sklad_type_zakupki->itemData( cb_sklad_type_zakupki->currentIndex()).toInt();
  QString str1 = " "+le_zakupkiFiltr->text()+" ";
  str1.replace(" ","%");
  QString str2 = " "+le_zakupkiFiltrSchet->text()+" ";
  str2.replace(" ","%");
  QString str3 =    " WHERE zakupki.name LIKE \'" + str1 + "\'";
  if( !le_zakupkiFiltrSchet->text().simplified().isEmpty() )
  { str3 +=           " AND zakupki.schet LIKE \'" + str2 + "\'";
  }
  if( i )
  { str3 += QString(" AND zakupki.type = %1 ").arg(i);
  }

  i = cb_zakupkiFiltrPostavshik->itemData( cb_zakupkiFiltrPostavshik->currentIndex()  ).toInt();
  if( i )
  { str3 += QString(" AND zakupki.postavshik = %1 ").arg(i);
  }

  tw_zakupkiHist->query_str_where = str3;
  tw_zakupkiHist->refresh();
}

//==============================================================================
//  изменение данных в поле фильтра расхода
//==============================================================================
void MainWindow::my7()
{
  QString str;
  sql_where_rashod_filtr.clear();
  if( le_rashodFiltr->text().length() )
  { str = " " + le_rashodFiltr->text() + " ";
    str.replace(" ","%");
    sql_where_rashod_filtr = QString(" prihod.name LIKE \'%1\' ").arg(str);
    str.clear();
  }

  if( !sql_where_rashod_filtr.isEmpty() )
  { str = " WHERE " + sql_where_rashod_filtr;
  }
  if(!sql_where_rashod_kompl.isEmpty())
  { str += str.isEmpty() ? " WHERE " : " AND ";
    str += sql_where_rashod_kompl;
  }

  tw_rashodHist->query_str_where = str;
  tw_rashodHist->refresh();
}

//==============================================================================
//
//==============================================================================
void MainWindow::on_cb_komplAll_toggled( bool state )
{
  QString str;

  if( !( permissions & USER_PERMISSION_KOMPL_SIGN ) )
     str = " WHERE sostav.status2 != \'Не утвержден\' ";

  if( !state )
  { if( !str.isEmpty() ) str += " AND ";
    if(  str.isEmpty() ) str += " WHERE ";
    str += " color < 40 ";
  }

  tw_komplZad->query_str_where  = str;
  tw_komplZad->refresh();
}

//==============================================================================
//
//==============================================================================
void MainWindow::my3( int i)
{
  switch(i)
  {
    case(0):
      projects_widget->refresh();
      break;
    case(1):
      tw_sklad->refresh();
      break;
    case(2):
      tw_zakupkiHist->refresh();
      break;
    case(3):
      tw_prihodHist->refresh();
      break;
    case(4):
      tw_rashodHist->refresh();
      break;
    case(5):
      tw_komplZad->refresh();
      tw_komplKomp->refresh( tw_komplZad->get_selected_id() );
      tw_komplPrih->refresh();
      break;
    case(6):
      widget_def->refresh();
      break;

  }
}

//==============================================================================
//
//==============================================================================
void MainWindow::my4()
{
  ZakupkiAddDialog dialog( this, tw_zakupkiHist->get_selected_id() );
  if( dialog.exec() == QDialog::Accepted )
  { on_action_Pereraschet_triggered();
  }
}

//==============================================================================
//
//==============================================================================
void MainWindow::pereraschet()
{
  on_action_Pereraschet_triggered();
  my3( tabWidget->currentIndex() );
}

//==============================================================================
//
//==============================================================================
void MainWindow::timer_slot()
{
  static int komplZad_id    = -1000;
  static int komplKompl_row = -1000;
  int id,i;

  id = tw_komplZad->get_selected_id();
  if( komplZad_id != id )
  { komplZad_id = id;
    if( id >= 0 )
    { QSqlQuery query;
      query.prepare( " SELECT id FROM sostav "
                     " WHERE id = ? AND status2 = \'Утвержден\' " );
      query.addBindValue( id );
      if( !query.exec() )
      {  sql_error_message( query, this );
	       return;
      }
      tw_komplKomp->refresh( id );
      tw_komplKomp->clearSelection();
      komplKompl_row = -1000;
    } else
    { tw_komplKomp->setRowCount(0);
      tw_komplKomp->setColumnCount(0);
      tw_komplKomp->clearSelection();
      tw_komplPrih->setRowCount(0);
      tw_komplPrih->setColumnCount(0);
      tw_komplPrih->clearSelection();
    }
  }

  QList<QTableWidgetSelectionRange> sr = tw_komplKomp->selectedRanges();
  if( sr.count() == 0 )
  { i=-1;
  } else
  { i = sr.at(0).topRow();
  }

  if( komplKompl_row != i )
  { komplKompl_row = i;
    if( i >= 0 )
    { QString str;
      str = tw_komplKomp->item(i,4)->text();
      str = str.isEmpty() ? "AND ISNULL( prihod.mesto ) "
                          : "AND prihod.mesto = \'" + str + "\' ";
      tw_komplPrih->query_str_where = "WHERE prihod.name = \'"
                                   + tw_komplKomp->item(i,0)->data(Qt::UserRole+1).toString()
                                   + "\' " + str
                                   + " AND prihod.ostatok > 0";
      tw_komplPrih->refresh();
    } else
    { tw_komplPrih->setRowCount(0);
      tw_komplPrih->setColumnCount(0);
      tw_komplPrih->clearSelection();
    }
  }
}

//==============================================================================
//
//==============================================================================
void MainWindow::on_pb_kompl_status_clicked()
{
  int i;
  QMenu menu;
  QMap< QAction*, int> map;

  int sostav_id = tw_komplZad->get_selected_id();
  if( sostav_id < 0 ) return;

  QSqlQuery query;
  QString current_status;
  QString current_zakupka_status;
  query.prepare(" SELECT status2, zakupka_status FROM sostav WHERE id = ? " );
  query.addBindValue( sostav_id );
  if( !query.exec() )
  {  sql_error_message( query, this );
     return;
  }
  query.next();
  current_status         = query.value(0).toString();
  current_zakupka_status = query.value(1).toString();

  for( i=0; i<sostav_status.count(); i++ )
  { QAction *act = menu.addAction( sostav_status.at(i) );
    if( act->text() == current_status )
    { act->setCheckable( true );
      act->setChecked( true );
    }
    map[ act ] = i;
  }
  menu.addSeparator();
  QAction *act_zakupka_no  = menu.addAction( "Закупка: НЕТ" );
  if( current_zakupka_status.toUpper() == "НЕТ" )
  {  act_zakupka_no->setCheckable( true );
     act_zakupka_no->setChecked( true );
  }
  QAction *act_zakupka_yes = menu.addAction( "Закупка: ДА" );
  if( current_zakupka_status.toUpper() == "ДА" )
  {  act_zakupka_yes->setCheckable( true );
     act_zakupka_yes->setChecked( true );
  }
  menu.addSeparator();
  QAction *act_notes = menu.addAction( "Монтаж..." );
  QAction *act = menu.exec( QCursor::pos() );

  if(( act == act_zakupka_no ) || (act == act_zakupka_yes) )
  { if( act == act_zakupka_no  ) current_zakupka_status = "Нет";
    if( act == act_zakupka_yes ) current_zakupka_status = "Да";
    query.prepare("UPDATE sostav SET zakupka_status = ? WHERE id = ?" );
    query.addBindValue( current_zakupka_status );
    query.addBindValue( sostav_id );
    if( !query.exec() )
    {  sql_error_message( query, this );
	     return;
    }
    tw_komplZad->refresh();
    tw_komplKomp->refresh( tw_komplZad->get_selected_id() );
    tw_komplPrih->refresh();
  }
  else if( map.contains( act ) )
  { QString str = sostav_status.value( map[ act ] );
    query.prepare("UPDATE sostav SET status2 = ? WHERE id = ?" );
    query.addBindValue( str );
    query.addBindValue( sostav_id );
    if( !query.exec() )
    {  sql_error_message( query, this );
	     return;
    }
    tw_komplZad->refresh();
    tw_komplKomp->refresh( tw_komplZad->get_selected_id() );
    tw_komplPrih->refresh();
  } else if( act == act_notes )
  { query.prepare("SELECT notes FROM sostav WHERE id = ?" );
    query.addBindValue( sostav_id );
    if( !query.exec() )
    {  sql_error_message( query, this );
	     return;
    }
    if( query.size() != 1 ) return;
    query.next();
    MontagDialog dialog( this, query.value(0).toString() );
    if( dialog.exec() == QDialog::Accepted )
    { query.prepare("UPDATE sostav SET notes = ? WHERE id = ?" );
      query.addBindValue( dialog.text() );
      query.addBindValue( sostav_id );
      if( !query.exec() )
      {  sql_error_message( query, this );
	       return;
      }
      tw_komplZad->refresh();
      tw_komplKomp->refresh( tw_komplZad->get_selected_id() );
      tw_komplPrih->refresh();
    }
  }
}

//==============================================================================
//
//==============================================================================
void MainWindow::on_pb_kompl_price1_clicked()
{
  int sostav_id = tw_komplZad->get_selected_id();
  if( sostav_id < 0 ) return;

  PriceDialog *dialog = new PriceDialog( 0, sostav_id );
  dialog->show();
}

//==============================================================================
//
//==============================================================================
void MainWindow::closeEvent ( QCloseEvent * event )
{
  Q_UNUSED( event );

  QSettings settings( QSETTINGS_PARAM );
  settings.setValue("pos", pos());
  settings.setValue("size", size());

  settings.setValue("zakupki",  tw_zakupkiHist->getShownFields() );
  settings.setValue("sklad",    tw_sklad->getShownFields() );
  settings.setValue("prihod",   tw_prihodHist->getShownFields() );
  settings.setValue("rashod",   tw_rashodHist->getShownFields() );
  settings.setValue("komplzad", tw_komplZad->getShownFields() );
  settings.setValue("tabindex",      tabWidget->currentIndex() );
  settings.setValue("komplprihod",   tw_komplPrih->getShownFields() );
  settings.setValue("splitter",  splitter->saveState() );
  settings.setValue("splitter_3", splitter_3->saveState() );
}
