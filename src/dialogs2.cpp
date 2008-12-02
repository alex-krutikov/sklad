#include <QtGui>
#include <QtSql>

#include "dialogs2.h"
#include "main.h"

//#######################################################################################
//
//#######################################################################################
OtpravkaDialog::OtpravkaDialog( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  setWindowTitle( "��������" );
  resize(600,400);

  sostav_id=0;
}

//=======================================================================================
//
//=======================================================================================
void OtpravkaDialog::refresh()
{
  QTableWidgetItem *titem;
  QStringList sl;
  QString str;
  QSqlQuery query;
  t_data idata;
  int i;

  QColor color1("darkgray");
  QFont  font1 = font();
  font1.setBold( true );


  data.clear();
  tw->verticalHeader()->setDefaultSectionSize(font().pointSize()+14);
  tw->setSelectionBehavior( QAbstractItemView::SelectRows );

  query.prepare( " SELECT kompl.id, kompl.type, typename, kompl.name, "
                 "        kompl.nominal, zamena.name, kompl.need, zamena.snato, "
                 "        kompl.status, kompl.snato, kompl.position "
                 " FROM kompl "
                 "   LEFT JOIN types  ON kompl.type   = types.id     "
                 "   LEFT JOIN zamena ON kompl.id     = zamena.kompl "
                 " WHERE sostav = ? "
                 " ORDER BY typename, kompl.name, zamena.id " );
  query.addBindValue( sostav_id );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  i = -1;
  while( query.next() )
  { if( query.value(8).toInt() != 0 ) continue;
    if( query.value(1).toInt() != i )
    { // ����� ���
      idata.flag = 1;
      idata.name = query.value(2).toString();
      data << idata;
      i = query.value(1).toInt();
    }
    idata.flag          = 0;
    idata.defichit_flag = 0;
    idata.treb     = query.value(6).toInt();
    idata.snato    = query.value(7).toInt();
    idata.name     = query.value(5).toString();
    idata.nominal  = query.value(4).toString();
    idata.position = query.value(10).toString();
    //idata.position = "sdfsdf";

    if( query.value(3) != query.value(5) )
    { // ������
      idata.flag = 2;
      idata.nominal = "";
    } else
    { if( idata.treb > query.value(9).toInt() )
       idata.defichit_flag = 1;
    }

    data << idata;
  }
  //-----------------------------------------------------
  tw->clear();
  sl << "������������"
     << "�������"
     << "���������"
     << "�����"
     << "�������";

  tw->setColumnCount(5);
  tw->setHorizontalHeaderLabels( sl );
  tw->horizontalHeader()->resizeSection(  0 , 200 );
  tw->horizontalHeader()->resizeSection(  1 , 100 );
  tw->horizontalHeader()->resizeSection(  2 , 70  );
  tw->horizontalHeader()->resizeSection(  3 , 70  );
  tw->horizontalHeader()->setStretchLastSection( true );
  tw->verticalHeader()->hide();

  tw->setRowCount( data.count() );
  for( i=0; i<data.count(); i++ )
  { idata = data.at(i);
    if( idata.flag == 1 )
    { // �������� ����
      titem = new QTableWidgetItem;
      titem ->setText( idata.name );
      titem ->setBackgroundColor( color1 );
      titem ->setFont( font1 );
      tw->setItem( i, 0, titem );
      tw->setSpan( i,0,1,5);
      continue;
    }
    // ��������
    titem = new QTableWidgetItem;
    str = idata.name;
    titem ->setText( idata.name );
    if( idata.flag==2 ) str = "  ������: " + str;
    //if( idata.defichit_flag ) str += " (#)";
    titem ->setText( str );
    tw->setItem( i, 0, titem );
    // �������
    titem = new QTableWidgetItem;
    titem ->setText( idata.nominal );
    tw->setItem( i, 1, titem );
    // ���������
    titem = new QTableWidgetItem;
    if( idata.flag != 2 )
      titem ->setText( QString::number( idata.treb ) );
    tw->setItem( i, 2, titem );
    // �����
    titem = new QTableWidgetItem;
    titem ->setText( QString::number( idata.snato ) );
    tw->setItem( i, 3, titem );
    // �������
    titem = new QTableWidgetItem;
    titem ->setText( idata.position );
    tw->setItem( i, 4, titem );
  }
}

//=======================================================================================
//
//=======================================================================================
void OtpravkaDialog::on_pb_print_clicked()
{
  QRect rect;
  int nn=0;

  int i,j,w,h,x,cur_y,page;
  const int ww[6] = {3200,1500,1000,700,1700,1000};
  Qt::Alignment alig;
  QString str,str2;
  QVector<QRect> v;
  t_data idata;
  bool pos_line_flag = false;

  QString proekt,zakaz,izdelie,sostav;

  const int XSIZE = 10000;
  const int YSIZE = XSIZE*297/210;
  const int TABLE_Y = 800;

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

  QPrinter printer;
  QPrintDialog printDialog(&printer, this);
  if (printDialog.exec() == QDialog::Accepted)
  {  QPainter painter( &printer );
     painter.setWindow( 0,0,XSIZE,YSIZE );

     QBrush brush(QColor("gray"));
     QPen   pen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );

     QFont font("Arial");
     font.setBold(   false );
     font.setItalic( false );
     font.setPointSize(110);

     QFont font2("Tahoma");
     font2.setBold(   false );
     font2.setItalic( false );
     font2.setPointSize(110);

     QFont font3(font2);
     font3.setBold(   false );
     font3.setItalic( false );
     font3.setPointSize(110);
     font3.setBold( true );

     QFont font4(font2);
     font4.setBold(   false );
     font4.setItalic( false );
     font4.setPointSize(140);

     h = font.pointSize()+130;
     i = 0;
     page = 1;
     painter.setFont( font );

     while(1)
     {
       cur_y = TABLE_Y;
       if( page!=1 ) cur_y -= 1*h;

       painter.setFont( font3 );
       x=1100;
       w = ww[0];
       rect.setX( x ); x += w ;
       rect.setY( cur_y-40 );
       rect.setWidth( w  );
       rect.setHeight( h );
       v << rect;
       painter.drawText( rect, Qt::AlignVCenter|Qt::AlignLeft, tr("  ������������")  );
       w = ww[1];
       rect.setX( x ); x += w ;
       rect.setWidth( w  );
       v << rect;
       painter.drawText( rect, Qt::AlignCenter, tr("�������")  );
       w = ww[2];
       rect.setX( x ); x += w ;
       rect.setWidth( w  );
       v << rect;
       painter.drawText( rect, Qt::AlignCenter, tr("���������")  );
       w = ww[3];
       rect.setX( x ); x += w ;
       rect.setWidth( w  );
       v << rect;
       painter.drawText( rect, Qt::AlignCenter, tr("�����")  );
       w = ww[4];
       rect.setX( x ); x += w ;
       rect.setWidth( w  );
       v << rect;
       painter.drawText( rect, Qt::AlignCenter, tr("�������")  );

       while( i < data.count() )
       { if( pos_line_flag )
         { // ���������� �������
             x = 1100;
             cur_y += h;
             w = ww[0]+ww[1]+ww[2]+ww[3]+ww[4];
             rect.setX( x );
             rect.setY( cur_y );
             rect.setWidth( w  );
             rect.setHeight( h );
             v << rect;
             painter.setFont( font );
             painter.drawText( rect, alig | Qt::TextDontClip, "  �������: " + str  );
             pos_line_flag = false;
         }

         idata = data.at(i);
         x = 1100;
         cur_y += h;
         if( idata.flag == 1 )
         { painter.setFont( font3 );
           w = ww[0]+ww[1]+ww[2]+ww[3]+ww[4];
           rect.setX( x ); x += w;
           rect.setY( cur_y );
           rect.setWidth( w  );
           rect.setHeight( h );
           painter.save();
           painter.fillRect( rect, QBrush(QColor("gray")) );
           v << rect;
           painter.restore();
           alig = Qt::AlignVCenter|Qt::AlignLeft;
           painter.drawText( rect, alig | Qt::TextDontClip, "  " + idata.name );
           i++;
           continue;
         }
         painter.setFont( font );
         if( !pos_line_flag )
         { if( idata.defichit_flag  )
           { painter.drawText( x-150, cur_y+170, "X"  );
           }
           for(j=0;j<5;j++)
           { w = ww[j];
             rect.setX( x ); x += w;
             rect.setY( cur_y );
             rect.setWidth( w  );
             rect.setHeight( h );
             switch( j )
             { case(0): str = idata.name;                     break;
               case(1): str = idata.nominal;                  break;
               case(2): str = QString::number( idata.treb );  break;
               case(3): str = QString::number( idata.snato ); break;
               case(4):
               {  str.clear();
                  if( idata.flag!=2 )
                  {  str = idata.position;
                     if( str.length() > 21 )
                     { pos_line_flag = true;
                     }
                  }
                  break;
               }
             }
             if( idata.flag==2 )
             { if     ( j==0 ) str = "   ������: " + str;
               else if( j==2 ) str = "";
             }
             v << rect;
             alig = Qt::AlignVCenter;
             switch( j )
             { case(0):
               case(4):
                 alig |= Qt::AlignLeft;
                 str = "  "+str;
                 break;
               //case(4):
               //  alig |= Qt::AlignHCenter;
               //  break;
               case(1):
               case(2):
               case(3):
               case(5):
                 alig |= Qt::AlignRight;
                 str = str + "  ";
                 break;
              }
              if( !pos_line_flag )
              {  painter.drawText( rect, alig | Qt::TextDontClip, str  );
              }  else
              {  painter.drawText( rect, alig | Qt::TextDontClip, "  ��. ������� ����"  );
              }
            }
            i++;
          }
          if( cur_y >  YSIZE - 1200 ) break;
        }

       painter.setFont( font4 );
       str2 = tr("������������: %1 (%2 ��.)").arg( sostav ).arg( nn );
       if( page == 1 )
       { painter.setFont( font4 );
         rect.setRect(0,400,10000,300);
         painter.drawText( rect, Qt::AlignCenter, str2  );
       } else
       { painter.setFont( font2 );
         rect.setRect(0,400,10000,600);
         painter.drawText(2000,  400,  str2 );
       }

       painter.setFont( font2 );
       painter.drawText(500,  400,       tr("��� \"��������\"") );
       painter.drawText(500,  900,       QString("N: %1").arg( sostav_id ) );
       painter.drawText(9000, 400,       QDate::currentDate().toString("dd.MM.yy") );

       str2  = tr("������: ") + proekt;
       str2 += tr("; �����: ") + zakaz;
       str2 += tr("; �������: ") + izdelie;
       //painter.drawText(500,  YSIZE-300, str2 ); // �� ���������

       if(( i != data.count() ) || ( page != 1 ))
         painter.drawText(9000, YSIZE-300, tr("���. %1").arg(page) );

       //painter.setPen( pen );
       painter.drawRects( v );
       v.clear();

       if( i == data.count() ) break;
       printer.newPage();
       page++;
     }
     painter.end();
  }
  accept();
}

//=======================================================================================
//
//=======================================================================================
void OtpravkaDialog::on_pb_copy_clicked()
{
  QString str;
  int i,j;

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
  query.next();
  str = sql_get_string( query, 3 )+"\r\n";

  str += tr("������������\t�������\t�����\t�����\r\n");

  for(i=0; i < tw->rowCount(); i++ )
  { for(j=0; j < tw->columnCount(); j++ )
    { if( tw->item(i,j) )
        str += "\"" + tw->item(i,j)->text() + "\"\t";
    }
    str.chop(1);
    str += "\r\n" ;
  }
  str.chop(1);

  QApplication::clipboard()->setText( str );
}

//#######################################################################################
//
//#######################################################################################
DootpravkaDialog::DootpravkaDialog( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  setWindowTitle( "����������" );

  de1->setDate( QDate(2006,1,1) );
  de2->setDate( QDate::currentDate() );

  sostav_id=0;
  connect( de1, SIGNAL( dateChanged ( const QDate&  ) ),
           this, SLOT( refresh() )  );
  connect( de2, SIGNAL( dateChanged ( const QDate&  ) ),
           this, SLOT( refresh() )  );

}

//=======================================================================================
//
//=======================================================================================
void DootpravkaDialog::refresh()
{
  QString str, str2;
  QStringList sl;
  QSqlQuery query;
  QTableWidgetItem *titem;
  int i,j,k;

  QColor color1("darkgray");
  QFont  font1 = font();
  font1.setBold( true );

  tw->verticalHeader()->setDefaultSectionSize(font().pointSize()+14);
  tw->setSelectionBehavior( QAbstractItemView::SelectRows );

  tw->clear();
  sl << "������������"
     << "�������"
     << "���-��"
     << "����";

  tw->setColumnCount(4);
  tw->setRowCount(0);
  tw->setHorizontalHeaderLabels( sl );
  tw->horizontalHeader()->resizeSection(  0 , 200 );
  tw->horizontalHeader()->resizeSection(  1 , 100 );
  tw->horizontalHeader()->resizeSection(  2 , 70  );
  tw->horizontalHeader()->resizeSection(  3 , 70  );
  tw->horizontalHeader()->resizeSection(  4 , 70  );
  tw->horizontalHeader()->resizeSection(  5 , 70  );

  tw->verticalHeader()->hide();

  query.prepare(
  " SELECT type, typename, zamena.name, kompl.name, nominal, rashod.n, rashod.date "
  " FROM rashod "
  "   LEFT JOIN zamena ON rashod.zamena = zamena.id "
  "   LEFT JOIN kompl  ON zamena.kompl  = kompl.id "
  "   LEFT JOIN types  ON kompl.type    = types.id "
  " WHERE kompl.sostav = ? AND rashod.date BETWEEN ? and ? "
  " ORDER BY typename, kompl.name, zamena.id "  );
  query.addBindValue( sostav_id );
  query.addBindValue( de1->date() );
  query.addBindValue( de2->date() );
  if( !query.exec() )
  {  sql_error_message( query, this );
	   return;
  }

  j=-1;
  while( query.next() )
  {
    i = tw->rowCount();
    tw->setRowCount( i+1 );
    if( j != query.value(0).toInt() )
    { // �������� ����
      j = query.value(0).toInt();
      titem = new QTableWidgetItem;
      titem ->setText( query.value(1).toString() );
      titem ->setBackgroundColor( color1 );
      titem ->setFont( font1 );
      titem ->setData( Qt::UserRole, 1 );
      tw->setItem( i, 0, titem );
      for( k=1; k<6; k++ )
      { titem = new QTableWidgetItem;
        titem ->setBackgroundColor( color1 );
        tw->setItem( i, k, titem );
      }
      i = tw->rowCount();
      tw->setRowCount( i+1 );
    }
    if(  (  str2 != query.value(3).toString() )
       && ( query.value(2).toString() != query.value(3).toString() ) )
    { // �������� ����� �������
      titem = new QTableWidgetItem;
      titem ->setText( query.value(3).toString() );
      tw->setItem( i, 0, titem );
      // �������
      titem = new QTableWidgetItem;
      titem ->setText( query.value(4).toString() );
      tw->setItem( i, 1, titem );
      i = tw->rowCount();
      tw->setRowCount( i+1 );
    }
    // ��������
    titem = new QTableWidgetItem;
    str  = query.value(2).toString();
    str2 = query.value(3).toString();
    if( str != str2 ) str = "  ������: " + str;
    //if( idata.defichit_flag ) str += " (#)";
    titem ->setText( str );
    tw->setItem( i, 0, titem );
    if( str == str2 )
    { // �������
      titem = new QTableWidgetItem;
      titem ->setText( query.value(4).toString() );
      tw->setItem( i, 1, titem );
    }
    // ���-��
    titem = new QTableWidgetItem;
    titem ->setText( query.value(5).toString() );
    titem ->setTextAlignment( Qt::AlignRight|Qt::AlignVCenter );
    tw->setItem( i, 2, titem );
    // �����
    titem = new QTableWidgetItem;
    titem ->setText( query.value(6).toDate().toString("dd MMM yy") );
    titem->setTextAlignment( Qt::AlignCenter );
    tw->setItem( i, 3, titem );
  }
}

//=======================================================================================
//
//=======================================================================================
void DootpravkaDialog::on_pb_print_clicked()
{
  QRect rect;
  int nn=0;

  int i,j,w,h,x,cur_y,page;

  const int SCALE = 4;
  const int ww[6] = {SCALE*320,SCALE*150,SCALE*120};
  Qt::Alignment alig;
  QString str,str2;
  QVector<QRect> v;

  QString proekt,zakaz,izdelie,sostav;



  const int XSIZE = 1000;
  const int YSIZE = XSIZE*297/210;
  const int TABLE_Y = 80;

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

  QPrinter printer;
  QPrintDialog printDialog(&printer, this);
  if (printDialog.exec() == QDialog::Accepted)
  {  QPainter painter( &printer );
     painter.setWindow( 0,0,SCALE*XSIZE,SCALE*YSIZE );
     QBrush brush(QColor("gray"));
     QPen   pen(Qt::black, SCALE*5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );

     QFont font("Arial");
     font.setBold(   false );
     font.setItalic( false );
     font.setPointSize(SCALE*11);

     QFont font2("Tahoma");
     //QFont font2("Arial");
     font2.setBold(   false );
     font2.setItalic( false );
     font2.setPointSize(SCALE*11);

     QFont font3(font2);
     font3.setBold(   false );
     font3.setItalic( false );
     font3.setPointSize(SCALE*11);
     font3.setBold( true );

     QFont font4(font2);
     font4.setBold(   false );
     font4.setItalic( false );
     font4.setPointSize(SCALE*14);

     h = font.pointSize()+SCALE*13;
     i = 0;
     page = 1;
     painter.setFont( font );

     while(1)
     {
       cur_y = SCALE*TABLE_Y;
       if( page!=1 ) cur_y -= 1*h;

       painter.setFont( font3 );
       x=SCALE*110;
       w = ww[0];
       rect.setX( x ); x += w ;
       rect.setY( cur_y-SCALE*4 );
       rect.setWidth( w  );
       rect.setHeight( h );
       v << rect;
       painter.drawText( rect, Qt::AlignVCenter|Qt::AlignLeft, tr("  ������������")  );
       w = ww[1];
       rect.setX( x ); x += w ;
       rect.setWidth( w  );
       v << rect;
       painter.drawText( rect, Qt::AlignCenter, tr("�������")  );
       w = ww[2];
       rect.setX( x ); x += w ;
       rect.setWidth( w  );
       v << rect;
       painter.drawText( rect, Qt::AlignCenter, tr("����������")  );

       while( i < tw->rowCount() )
       { x = SCALE*110;
         cur_y += h;
         if( tw->item(i,0)->data(Qt::UserRole).toInt() == 1 )
         { painter.setFont( font3 );
           w = ww[0]+ww[1]+ww[2];
           rect.setX( x ); x += w;
           rect.setY( cur_y );
           rect.setWidth( w  );
           rect.setHeight( h );
           painter.save();
           //painter.setBrush(QBrush(QColor("gray")));
           painter.fillRect( rect, QBrush(QColor("gray")) );
           v << rect;
           painter.restore();
           alig = Qt::AlignVCenter|Qt::AlignLeft;
           painter.drawText( rect, alig | Qt::TextDontClip , "  " + tw->item(i,0)->text() );
           i++;
           continue;
         }
         painter.setFont( font );
         for(j=0;j<3;j++)
         { w = ww[j];
           rect.setX( x ); x += w;
           rect.setY( cur_y );
           rect.setWidth( w  );
           rect.setHeight( h );
           str.clear();
           if( tw->item(i,j) )  str = tw->item(i,j)->text();
           v << rect;
           alig = Qt::AlignVCenter;
           switch( j )
           { case(0):
             case(1):
               alig |= Qt::AlignLeft;
               str = "  "+str;
               break;
             //case(4):
               //alig |= Qt::AlignHCenter;
               //break;
             case(2):
               alig |= Qt::AlignRight;
               str = str + "  ";
               break;
            }
            painter.drawText( rect, alig | Qt::TextDontClip, str  );
          }
          i++;
          if( cur_y >  SCALE*YSIZE - SCALE*120 ) break;
        }

       painter.setFont( font4 );
       str2 = tr("����������: %1 (%2 ��.)").arg( sostav ).arg( nn );
       if( page == 1 )
       { painter.setFont( font4 );
         rect.setRect(0,SCALE*10,SCALE*1000,SCALE*60);
         painter.drawText( rect, Qt::AlignCenter, str2  );
       } else
       { painter.setFont( font2 );
         rect.setRect(0,SCALE*40,SCALE*1000,SCALE*60);
         painter.drawText(SCALE*200,  SCALE*40,  str2 );
       }

       painter.setFont( font2 );
       painter.drawText(SCALE*50,  SCALE*40,       tr("��� \"��������\"") );
       painter.drawText(SCALE*50,  SCALE*90,       QString("N: %1").arg( sostav_id ) );
       painter.drawText(SCALE*900, SCALE*40,       QDate::currentDate().toString("dd.MM.yy") );

       str2  = tr("������: ") + proekt;
       str2 += tr("; �����: ") + zakaz;
       str2 += tr("; �������: ") + izdelie;
       //painter.drawText(500,  YSIZE-300, str2 ); // �� ���������

       if(( i != tw->rowCount() ) || ( page != 1 ))
         painter.drawText(SCALE*900, SCALE*YSIZE-SCALE*30, tr("���. %1").arg(page) );

       //painter.setPen( pen );
       painter.drawRects( v );
       v.clear();

       //break;
       if( i == tw->rowCount() ) break;
       printer.newPage();
       page++;
     }
     painter.end();
  }
  accept();
}

//=======================================================================================
//
//=======================================================================================
void DootpravkaDialog::on_pb_copy_clicked()
{
}

//#######################################################################################
//
//#######################################################################################
MontagDialog::MontagDialog( QWidget *parent, QString name )
  : QDialog( parent )
{
  setupUi( this );
  setWindowTitle( "������" );
  le->setText( name );
  le->setFocus();
}

//=======================================================================================
//
//=======================================================================================
void MontagDialog::on_pb_date_clicked()
{
  le->insert( QDate::currentDate().toString("dd.MM.yy") );
}
//=======================================================================================
//
//=======================================================================================
void MontagDialog::on_pb_piter_clicked()
{
  le->insert( "�����" );
}
//=======================================================================================
//
//=======================================================================================
void MontagDialog::on_pb_ilya_clicked()
{
  le->insert( "����" );
}

