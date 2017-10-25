#include <QtGui>
#include <QtSql>

#include "bom.h"
#include "main.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>

namespace {

QString get_positions_string(const QStringList &positions_items)
{
    QString ret;

    const QRegExp rx("^(\\D+)(\\d+)$");

    if (!positions_items.length())
        return "";

    rx.indexIn(positions_items[0]);
    QString prefix = rx.cap(1);

    if(prefix.isEmpty())
        return "Error in postion list";

    QVector<int> positions;

    for (const QString &str : positions_items)
    {
        rx.indexIn(str);
        if (rx.cap(1) != prefix)
            return "Error in postion list";
        bool ok;
        positions << rx.cap(2).toInt(&ok);
        if (!ok)
            return "Error in postion list";
    }

    qSort(positions);

    for (int i = 0; i < positions.size();)
    {
        int j;
        for (j = (i + 1); j < positions.size(); ++j)
        {
            if (positions[j] != (positions[j - 1] + 1))
                break;
        }
        --j;

        if ((j - i) > 1)
        {
            ret += QString("%1%2 - %3%4, ").arg(prefix).arg(positions[i]).arg(prefix).arg(positions[j]);
        } else if ((j - i) == 1)
        {
            ret += QString("%1%2, %3%4, ").arg(prefix).arg(positions[i]).arg(prefix).arg(positions[j]);
        } else
        {
            ret += QString("%1%2, ").arg(prefix).arg(positions[i]);
        }
        i = j + 1;
    }

    ret.chop(2);

    return ret;
}

}

//#######################################################################################
//
//#######################################################################################
bool BomFile::loadFile( const QString filename )
{
  data.clear();

  QFile file( filename );
  QRegExp rx1("\\d");
  QRegExp rx2("\\D");
  QString str,str3;
  QStringList sl;
  int i,k,fnmax;
  QVector<int> fn;
  QVector<int> v,v2;
  t_data ss;

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {QMessageBox::critical(mainwindow, "Импорт BOM-файла",
              QString("Не могу открыть файл \"%1\"").arg( filename ) );
    return false;
  }

  str = QString::fromLocal8Bit( file.readLine() ).toUpper().simplified();
  str.remove("\"");
  sl = str.split(";");
  sl << "qwert"; // добавляем лишнюю секцию для безглючности

  fn.resize(4);
  fn[0] = sl.indexOf("COUNT");
  fnmax = fn[0];
  if( fn[0] < 0 )
  { QMessageBox::critical(0, "Импорт BOM-файла",
              "Не найдено поле \"Count\" в первой строке BOM-файла!" );
    return false;
  }
  fn[1] = sl.indexOf("COMPONENTNAME");
  if( fnmax < fn[1] ) fnmax=fn[1];
  if( fn[1] < 0 )
  { QMessageBox::critical(0, "Импорт BOM-файла",
              "Не найдено поле \"ComponentName\" в первой строке BOM-файла!" );
    return false;
  }
  fn[2] = sl.indexOf("REFDES");
  if( fnmax < fn[2] ) fnmax=fn[2];
  if( fn[2] < 0 )
  { QMessageBox::critical(0, "Импорт BOM-файла",
              "Не найдено поле \"RefDes\" в первой строке BOM-файла!" );
    return false;
  }
  fn[3] = sl.indexOf("VALUE");
  if( fnmax < fn[3] ) fnmax=fn[3];
  if( fn[3] < 0 )
  { QMessageBox::critical(0, "Импорт BOM-файла",
              QString("Не найдено поле \"Value\" в первой строке BOM-файла! %1 %2")
                .arg( sl.indexOf("VALUE") )
                .arg( sl.at(4) )  );
    return false;
  }

  k = 0;
  i = 1;
  while (!file.atEnd())
  {
    str = QString::fromLocal8Bit( file.readLine() );
    i++;
    str.remove("\"");
    sl = str.split(";");
    if( sl.size() < fnmax ) continue;

    ss.name    = sl[ fn[1] ]; // название
    ss.nominal = sl[ fn[3] ]; // номинал
    ss.count   = sl[ fn[0] ].toInt(); // кол-во

    // позиция
    k = sl[ fn[0] ].toInt() - 1;
    if( k < 0 )
    { QMessageBox::critical(0, "Импорт BOM-файла",
              QString("Ошибка в поле \"Count\" (строка %1)! ").arg(i) );
      return false;
    }

    v.clear();
    str3 = QString(sl[ fn[2] ]).remove(rx1);
    v << sl[ fn[2] ].remove(rx2).toInt();


    while( k && (!file.atEnd()))
    { str = QString::fromLocal8Bit( file.readLine() );
      i++;
      str.remove("\"");
      sl = str.split(";");
      if( sl.size() < fnmax ) continue;
      v << sl[ fn[2] ].remove(rx2).toInt();
      k--;
      if(sl[ fn[0] ].count() > 0 )
      {
        QMessageBox::critical(0, "Импорт BOM-файла",
              QString("Ошибка! Присутствует кол-во в подчиненной строке. (строка %1)! ").arg(i) );
              data.clear();
              return false;
      }
    }

    // заполнение полного списка позиций
    ss.position_items.clear();
    foreach (const int pos, v)
    {
        ss.position_items << QString("%1%2").arg(str3).arg(pos);
    }

    ss.type     = str3;  // тип

    data << ss;
  }

  return true;
}

//==============================================================================
//
//==============================================================================
void BomFile::process()
{
  int i,j;

  QVector<t_data> d;
  QMap<QString,int> map;

  // поиск повторений
  for(i=0; i<data.count(); i++)
  { if( map.contains( data[i].name ) )
    { j = map[ data[i].name ];
      if( data[i].type != d[j].type )
      {  QMessageBox::critical( 0, "Проверка BOM файла",
                        QString("Ошибка! Найдено повторение наименования при разных типах: \n\n"
                                "  %1 ( %2 шт.) тип:%3 \n"
                                "  %4 ( %5 шт.) тип:%6 \n")
                                  .arg( d[j].name    ).arg( d[j].count    ).arg( d[j].type    )
                                  .arg( data[i].name ).arg( data[i].count ).arg( data[i].type ) );
         data.clear();
         return;
      }
      if( data[i].nominal != d[j].nominal )
      {  QMessageBox::critical( 0, "Проверка BOM файла",
                        QString("Ошибка! Найдено повторение наименования при разных номиналах: \n\n"
                                "  %1 ( %2 шт.) номинал:%3 \n"
                                "  %4 ( %5 шт.) номинал:%6 \n")
                                  .arg( d[j].name    ).arg( d[j].count    ).arg( d[j].nominal    )
                                  .arg( data[i].name ).arg( data[i].count ).arg( data[i].nominal ) );
         data.clear();
         return;
      }
      QMessageBox::information( 0, "Проверка BOM файла",
                        QString("Найдено повторение позиций:\n\n"
                                "  %1 ( %2 шт.) %3 \n"
                                "  %4 ( %5 шт.) %6 \n\nПозиции будут объединены.")
                                  .arg( d[j].name    ).arg( d[j].count    ).arg( d[j].position_items.join(", "))
                                  .arg( data[i].name ).arg( data[i].count ).arg( data[i].position_items.join(", ")));
      d[j].count    += data[i].count;
      d[j].position_items += data[i].position_items;

      continue;
    }
    d << data[i];
    map[data[i].name] = d.count()-1;
  }
  data = d;
}

//==============================================================================
//
//==============================================================================
void BomFile::sort()
{
  qSort( data );
}

//#######################################################################################
//
//#######################################################################################
BomAddDialog::BomAddDialog( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  resize(1024,600);

  QStringList sl;

  //-------------------------------------------------------------------------------------
  sl << "Наименование BOM"
     << "Номинал"
     << "Кол-во"
     << "Наименование СКЛАД"
     << "Наличие"
     << "Позиция"
     << "Исключенные позиции";
  tw->setColumnCount(TW_COLUMNS_COUNT);
  tw->verticalHeader()->setDefaultSectionSize(font().pointSize()+11);
  tw->verticalHeader()->hide();
  tw->setHorizontalHeaderLabels( sl );
  tw->horizontalHeader()->resizeSection( TW_COLUMN_BOM_NAME, 150 );
  tw->horizontalHeader()->resizeSection( TW_COLUMN_NOMINAL, 100 );
  tw->horizontalHeader()->resizeSection( TW_COLUMN_COUNT,  50 );
  tw->horizontalHeader()->resizeSection( TW_COLUMN_NAME_SKLAD, 150 );
  tw->horizontalHeader()->resizeSection( TW_COLUMN_NALICHIE,  70 );
  tw->horizontalHeader()->resizeSection( TW_COLUMN_POSITION, 300 );
  tw->horizontalHeader()->resizeSection( TW_COLUMN_POSITION_EXCLUDED, 100 );
  tw->horizontalHeader()->setStretchLastSection( true );
  tw->setSelectionBehavior( QAbstractItemView::SelectRows );
  tw->setSelectionMode( QAbstractItemView::SingleSelection );
}

//==============================================================================
//
//==============================================================================
bool BomAddDialog::init( QString filename, int izdelie_id, int count )
{
  QSqlQuery query;
  QSqlQuery query2;
  QSqlQuery query3;
  QString str;
  QStringList sl;
  QTableWidgetItem *titem;
  int i,j,k;

  this->izdelie_id = izdelie_id;
  this->count = count;

  //-------------------------------------------------------------------------------------
  query.prepare(" SELECT name FROM izdelie WHERE id = ?" );
  query.addBindValue( izdelie_id );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return false;
  }
  query.next();
  setWindowTitle( "Добавить в состав изделия: "+query.value(0).toString() );
  //-------------------------------------------------------------------------------------
  str = filename;
  str.chop(4);
  str.remove(0, str.lastIndexOf('/')+1);
  str = tr("Плата ")+str;
  le1->setText( str );
  str.clear();
  //-------------------------------------------------------------------------------------
  BomFile bf;
  bool ok = bf.loadFile( filename );
  if (!ok)
      return false;
  bf.process();
  //-------------------------------------------------------------------------------------
  // типы элементов
  QMap<QString,int> types_map;
  query.prepare(" SELECT id,typename,symbol FROM types WHERE used != 0" );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return false;
  }
  i=0;
  while( query.next() )
  { types_map[ query.value(2).toString() ] = i;
    i++;
  }
 //-------------------------------------------------------------------------------------
 // наименования BOM ==> SKLAD
  QMap<QString,int> b2s_map;
  query2.prepare(" SELECT name_bom,name_sklad FROM bom2sklad" );
  if( !query2.exec() )
  {  sql_error_message( query2, this );
	   return false;
  }
  i=0;
  while( query2.next() )
  { b2s_map[ query2.value(0).toString() ] = i;
    i++;
  }
 //-------------------------------------------------------------------------------------
 // наличие на складе
  QMap<QString,int> nalichie_map;
  query3.prepare(" SELECT name, sum(ostatok) FROM prihod GROUP BY type,name ");
  if( !query3.exec() )
  {  sql_error_message( query3, this );
	   return false;
  }
  i=0;
  while( query3.next() )
  { str = query3.value(0).toString();
    if( nalichie_map.contains( str ) )
    {  QMessageBox::critical( 0, "Импорт BOM файла",
                        QString("Ошибка! В базе прихода найдено повторение\nнаименования разных типов: \n\n"
                                "  %1 ").arg( str ) );
       return false;
    }
    nalichie_map[ str ] = i;
    i++;
  }
  //-------------------------------------------------------------------------------------

  t_data ss;
  for( i=0; i<bf.data.count(); i++ )
  { //--------------------- тип -----

    if( types_map.contains( bf.data[i].type ) )
    { query.seek( types_map[ bf.data[i].type ] );
      ss.type      = query.value(0).toInt();
      ss.type_name = query.value(1).toString();
    } else
    { ss.type = 0;
      ss.type_name.clear();
    }
    ss.is_enabled = true;
    //--------------------- название BOM ---
    ss.name_bom = bf.data[i].name;
    //--------------------- номинал ---
    ss.nominal = bf.data[i].nominal;
    //--------------------- кол-во ---
    ss.count_from_bom = bf.data[i].count;
    ss.count = ss.count_from_bom;
    //--------------------- наименование СКЛАД ---
    if( b2s_map.contains( bf.data[i].name ) )
    { query2.seek( b2s_map[ bf.data[i].name ] );
      ss.name_sklad = query2.value(1).toString();
    } else
    { ss.name_sklad.clear();
    }
    //--------------------- наличие ---
    str = ss.name_sklad;
    if( str.isEmpty() ) str = ss.name_bom;
    if( nalichie_map.contains( str ) )
    { query3.seek( nalichie_map[ str ] );
      ss.nalichie = query3.value(1).toInt();
    } else
    { ss.nalichie = -1;
    }
    //--------------------- позиция ---
    ss.position_items = bf.data[i].position_items;
    ss.position = get_positions_string(ss.position_items);
    //----------------------------------
    data << ss;
  }
  qSort( data );
  //-------------------------------------------------------------------------------------
  k = -1;
  j = 0;
  for( i=0; i<data.count(); i++ )
  { if( k != data[i].type )
    { QFont boldfont = font();
      boldfont.setBold( true );
      k = data[i].type;
      tw->setRowCount( j+1 );
      titem = new QTableWidgetItem;
      titem -> setText( data[i].type_name );
      titem -> setFlags( Qt::ItemIsEnabled );
      titem -> setBackgroundColor( QColor("lightgray") );
      titem -> setFont( boldfont );
      tw->setItem( j, 0, titem );
      tw->setSpan(j,0,1,TW_COLUMNS_COUNT);
      j++;
    }
    tw->setRowCount( j+1 );
    tw2data[j]=i;
    data2tw[i]=j;
    //--------------------- название BOM ---
    titem = new QTableWidgetItem;
    titem -> setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
    titem -> setCheckState(Qt::Checked);
    titem->setText( "   " + data[i].name_bom );
    tw->setItem(j, TW_COLUMN_BOM_NAME, titem );
    //--------------------- номинал ---
    titem = new QTableWidgetItem;
    titem -> setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    titem->setText( data[i].nominal );
    tw->setItem(j, TW_COLUMN_NOMINAL, titem );
    //--------------------- кол-во ---
    titem = new QTableWidgetItem;
    titem -> setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    titem -> setTextAlignment( Qt::AlignRight|Qt::AlignVCenter );
    titem->setText( QString::number( data[i].count ));
    tw->setItem(j, TW_COLUMN_COUNT, titem );
    //--------------------- наименование СКЛАД ---
    titem = new QTableWidgetItem;
    titem -> setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    titem->setText( data[i].name_sklad );
    tw->setItem(j, TW_COLUMN_NAME_SKLAD, titem );
    //--------------------- наличие ---
    titem = new QTableWidgetItem;
    titem -> setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    titem -> setTextAlignment( Qt::AlignRight|Qt::AlignVCenter );
    if( data[i].nalichie >= 0 )
      titem->setText( QString::number( data[i].nalichie ) );
    tw->setItem(j, TW_COLUMN_NALICHIE, titem );
    //--------------------- позиция ---
    titem = new QTableWidgetItem;
    titem -> setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    titem->setText( data[i].position );
    tw->setItem(j, TW_COLUMN_POSITION, titem );
    //--------------------- исключенные позиции ---
    titem = new QTableWidgetItem;
    titem -> setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    tw->setItem(j, TW_COLUMN_POSITION_EXCLUDED, titem );
    j++;
  }
  return true;
}


//==============================================================================
//
//==============================================================================
void BomAddDialog::on_tw_cellDoubleClicked( int row, int column )
{
  Q_UNUSED( column );

  if( !tw2data.contains( row ) ) return;
  BomAddDialog2 dialog(this, tw2data[row] );
  if( dialog.exec() == QDialog::Accepted )
  { QString str = data[tw2data[row]].name_sklad;
    tw->item(row, TW_COLUMN_NAME_SKLAD)->setText( str );
    if( str.isEmpty() ) str = tw->item(row,TW_COLUMN_BOM_NAME)->text();
    QSqlQuery query;
    query.prepare(" SELECT SUM(ostatok) FROM prihod WHERE name = ?" );
    query.addBindValue( str );
    if( !query.exec() )
    {  sql_error_message( query, this );
	     return;
    }
    if( query.next() )
    { tw->item(row, TW_COLUMN_NALICHIE)->setText( query.value(0).toString() );
      data[tw2data[row]].nalichie = query.value(0).toInt();
    } else
    { tw->item(row, TW_COLUMN_NALICHIE)->setText( QString() );
      data[tw2data[row]].nalichie = -1;
    }
  }
}

//=======================================================================================
//
//=======================================================================================
void BomAddDialog::on_tw_cellChanged(int row, int column)
{
    if (column != TW_COLUMN_BOM_NAME)
        return;

    if (!(tw->item(row, TW_COLUMN_BOM_NAME)->flags() & Qt::ItemIsUserCheckable))
        return;

    bool checked = tw->item(row, TW_COLUMN_BOM_NAME)->checkState() == Qt::Checked;

    data[tw2data[row]].is_enabled = checked;

    QColor color = checked ? "black" : "gray";
    for (int j = 0; j < TW_COLUMNS_COUNT; ++j) {
        QTableWidgetItem *item = tw->item(row, j);
        if (item)
            item->setTextColor(color);
    }
}

//=======================================================================================
//
//=======================================================================================
void BomAddDialog::on_pb_variants_file_clicked()
{
    //============ Открытие файла вариантов ==============

    QSettings settings( QSETTINGS_PARAM );
    QString filename = QFileDialog::getOpenFileName(
                this,
                "Импорт VAR-файла",
                settings.value( "varpath", "" ).toString(),
                "VAR (*.var)");
    if(filename.isEmpty())
        return;

    settings.setValue("varpath", filename);

    //============ Парсинг файла вариантов ==============

    variants.clear();

    QFile var_file(filename);
    var_file.open(QIODevice::ReadOnly);
    QTextStream fs(&var_file);

    t_variant v;

    while(!fs.atEnd())
    {
        QString line = fs.readLine();
        if (line.startsWith("Variant Name:"))
        {
            v = t_variant();
            v.name = line.split(':').value(1).simplified();
            qDebug() << "%%%" << v.name;
            continue;
        }
        if (line.startsWith("Variant Description:"))
        {
            v.desc = line.split(':').value(1).simplified();
            continue;
        }
        if (line == "PropertyTypeExcludedComponents")
        {
            QString line = fs.readLine();
            v.items = line.split(';').toSet();
            variants << v;
            continue;
        }
    }

    //============ Заполнение комбобокса вариантов ==============

    cb_variants->clear();
    for (int i=0; i < variants.length(); ++i)
    {
        cb_variants->addItem(QIcon(), QString("%1 (%2)").arg(variants[i].name).arg(variants[i].desc));
    }
}

void BomAddDialog::on_cb_variants_currentIndexChanged(int index)
{
    const QSet<QString> excludes = (index < 0) ? QSet<QString>() : variants[index].items;

    for (int i = 0; i < data.size(); ++i) {
        QStringList excl_pos_list, pos_list;
        for (int j = 0; j < data[i].position_items.size(); ++j) {
            if (excludes.contains(data[i].position_items[j])) {
                excl_pos_list << data[i].position_items[j];
            } else {
                pos_list << data[i].position_items[j];
            }
        }
        data[i].position = get_positions_string(pos_list);
        data[i].count = data[i].count_from_bom - excl_pos_list.size();
        tw->item(data2tw[i], TW_COLUMN_COUNT)->setText(QString::number(data[i].count));
        tw->item(data2tw[i], TW_COLUMN_POSITION)->setText(data[i].position);
        tw->item(data2tw[i], TW_COLUMN_POSITION_EXCLUDED)->setText(get_positions_string(excl_pos_list));

    }
}

//=======================================================================================
//
//=======================================================================================
void BomAddDialog::accept()
{
  int i,j,last_id,last_id2;
  QString str;
  QSqlQuery query;

  QMap<QString,int> map;
  for( i=0; i<data.count(); i++ )
  { if (!data[i].is_enabled)
          continue;

    str = data[i].name_sklad;
    if( str.isEmpty() ) str = data[i].name_bom;
    j = map.value( str, -1 );
    if( j >= 0 )
    { if( data[i].type != data[j].type )
      { QMessageBox::critical( this, app_header,
        QString("ОШИБКА! Повторение позиции с разными типами: %1\n").arg(str) );
        return;
      }
      if( data[i].nominal != data[j].nominal )
      { QMessageBox::critical( this, app_header,
        QString("ОШИБКА! Повторение позиции с разными номиналами: %1\n").arg(str) );
        return;
      }
      data[i].count    += data[j].count;
      if( !data[j].position.isEmpty() )
      { data[i].position += ", " + data[j].position;
      }
      data.remove(j);
    }
    map.insert(str,i);
  }

  QProgressDialog progress("Copying files...", 0, 0, data.count(), this);

  if( le1->text().isEmpty() )
  {QMessageBox::critical(this, tr("Импорт BOM-файла"),
              tr("Поле \"Название\" не заполнено") );
    le1->setFocus();
    return;
  }

  // подготовка: чистка таблиц. Удаляем записи в kompl и zamena, не принадлежащие
  // ни одному составу

  query.prepare( "DELETE zamena,kompl FROM zamena,kompl "
                 " WHERE kompl.sostav NOT IN ( SELECT DISTINCT id FROM sostav ) "
                 "   AND zamena.kompl=kompl.id " );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  query.prepare( "INSERT INTO sostav ( izdelie, name, n1, date, status2 ) "
                 "VALUES ( ?,?,?,?,? ) " );
  query.addBindValue( izdelie_id );
  query.addBindValue( le1->text() );
  query.addBindValue( count );
  query.addBindValue( QDate::currentDate() );
  query.addBindValue( "Не утвержден" );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  last_id = query.lastInsertId().toInt();

  for( i=0; i<data.count(); i++ )
  { if (!data[i].is_enabled)
          continue;
    if (data[i].count == 0)
        continue;

    str = data[i].name_sklad;
    if( str.isEmpty() ) str = data[i].name_bom;
    query.prepare( "INSERT INTO kompl ( sostav, name, nominal, type, n ,position ) "
                   "VALUES ( ?,?,?,?,?,? ) " );
    query.addBindValue( last_id );
    query.addBindValue( str );
    query.addBindValue( data[i].nominal    );
    query.addBindValue( data[i].type       );
    query.addBindValue( data[i].count      );
    query.addBindValue( data[i].position   );
    if( !query.exec() )
    {  sql_error_message( query, this );
	     return;
    }

    last_id2 = query.lastInsertId().toInt();

    query.prepare( "INSERT INTO zamena ( kompl, name )  "
                   "VALUES ( ?,? ) " );
    query.addBindValue( last_id2 );
    query.addBindValue( str      );
    if( !query.exec() )
    {  sql_error_message( query, this );
	     return;
    }
    progress.setValue(i);
  }
  done(QDialog::Accepted);
}

//#######################################################################################
//
//#######################################################################################
BomAddDialog2::BomAddDialog2( BomAddDialog *parent_arg, int data_index_arg )
  : QDialog( parent_arg ), parent( parent_arg ), data_index( data_index_arg )
{
  setupUi( this );
  setWindowTitle( app_header );

  QString name_sklad = parent->data[data_index].name_sklad;
  le->setText( parent->data[data_index].name_bom );
  QStringList sl;
  sl << "Наименование СКЛАД"
     << "Наличие";
  tw->setColumnCount(2);
  tw->verticalHeader()->setDefaultSectionSize(font().pointSize()+11);
  tw->verticalHeader()->hide();
  tw->setHorizontalHeaderLabels( sl );
  tw->horizontalHeader()->resizeSection( 0 , 200 );
  tw->horizontalHeader()->resizeSection( 1 ,  70 );
  tw->horizontalHeader()->setStretchLastSection( true );
  tw->setSelectionBehavior( QAbstractItemView::SelectRows );
  tw->setSelectionMode( QAbstractItemView::SingleSelection );

  if( name_sklad.isEmpty() ) name_sklad = le->text();
  le2->setText( name_sklad );
  le2->setFocus();
}

//==============================================================================
//
//==============================================================================
void BomAddDialog2::accept()
{
  QSqlQuery query;
  int row,i;
  QString name_bom = le->text();
  QString name_sklad;
  QList<QTableWidgetSelectionRange> sr;
  sr = tw->selectedRanges();

  query.prepare( "SELECT id FROM bom2sklad WHERE name_bom = ? " );
  query.addBindValue( name_bom );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  i = query.size();

  if( sr.count() > 0 )
  { row = sr.at(0).topRow();
    name_sklad = tw->item(row,0)->text();
  }
  parent->data[data_index].name_sklad = name_sklad;
  if( !name_sklad.isEmpty() )
  { if( i ) { query.prepare( "UPDATE      bom2sklad SET name_bom = ?, name_sklad = ? " ); }
    else    { query.prepare( "INSERT INTO bom2sklad SET name_bom = ?, name_sklad = ? " ); }
    query.addBindValue( name_bom   );
    query.addBindValue( name_sklad );
    if( !query.exec() )
    {  sql_error_message( query, this );
	     return;
    }
  } else
  { if( i )
    { query.prepare( "DELETE FROM bom2sklad WHERE name_bom = ? " );
      query.addBindValue( name_bom );
      if( !query.exec() )
      {  sql_error_message( query, this );
	       return;
      }
    }
  }
  done(QDialog::Accepted);
}

//==============================================================================
//
//==============================================================================
void BomAddDialog2::on_le2_textChanged( const QString & text )
{
  int i;
  QTableWidgetItem *titem;
  QSqlQuery query;
  QString str;
  QString filtr = " "+text+" ";
  filtr.replace(" ","%");

  QList<QTableWidgetSelectionRange> sr = tw->selectedRanges();
  if( sr.count() > 0 )
  { str = tw->item( sr.at(0).topRow(), 0 )->text();
  }

  query.prepare(" SELECT name, SUM(ostatok) FROM prihod WHERE type = ? AND name LIKE ? GROUP BY name" );
  query.addBindValue( parent->data[data_index].type );
  query.addBindValue( filtr );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }
  tw->clearContents();
  tw->setRowCount( query.size() );
  i=0;
  while( query.next() )
  { titem = new QTableWidgetItem;
    titem -> setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
    titem ->setText( query.value(0).toString() );
    tw->setItem(i,0,titem);
    titem = new QTableWidgetItem;
    titem -> setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
    titem -> setTextAlignment( Qt::AlignRight|Qt::AlignVCenter );
    titem ->setText( query.value(1).toString() );
    tw->setItem(i,1,titem);
    if( tw->item(i,0)->text() == str ) tw->selectRow( i );
    i++;
  }
}

//==============================================================================
//
//==============================================================================
void BomAddDialog2::on_pb1_clicked()
{  tw->clearSelection();
}

//==============================================================================
//
//==============================================================================
void BomAddDialog2::on_tw_cellDoubleClicked()
{ accept();
}
