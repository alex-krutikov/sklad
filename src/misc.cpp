#include "misc.h"
#include "dialogs.h"
#include "dialogs2.h"
#include "main.h"
#include "price.h"

#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>

//#######################################################################################
//
//#######################################################################################
SqlTable::SqlTable(QWidget *parent)
    : QTableWidget(parent)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Inactive, QPalette::Highlight,
                 pal.color(QPalette::Active, QPalette::Highlight));
    pal.setColor(QPalette::Inactive, QPalette::HighlightedText,
                 pal.color(QPalette::Active, QPalette::HighlightedText));
    setPalette(pal);


    rows_highlighting = false;
    row_selection_mode = false;
    horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(horizontalHeader(),
            SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(my1(const QPoint &)));
    connect(horizontalHeader(), SIGNAL(sectionClicked(int)), this,
            SLOT(my2(int)));
}

//=======================================================================================
/// Очистить настройки таблицы
//=======================================================================================
void SqlTable::clear_query_fields()
{
    header_labels.clear();
    header_sizes.clear();
    query_fields.clear();
    header_show_flag.clear();
}

//=======================================================================================
/// Добавить коронку к таблице
//=======================================================================================
void SqlTable::add_query_field(QString fieldname, int size, QString sql_field,
                               int alignment_arg)
{
    header_labels << fieldname;
    header_sizes << size;
    query_fields << sql_field;
    alignment << alignment_arg;
}


//=======================================================================================
//
//=======================================================================================
void SqlTable::setShownFields(QString str)
{
    int i, j, k;
    QStringList sl;

    k = header_labels.size();
    header_show_flag.resize(k);
    header_show_flag.fill(1);
    sl = str.split(",");
    j = sl.size();
    if (j == 1) return;
    if (j >= k) j = k;
    for (i = 0; i < j; i++)
    {
        header_show_flag[i] = sl[i].toInt();
    }
}

//=======================================================================================
//
//=======================================================================================
QString SqlTable::getShownFields()
{
    int i;
    QString str;
    for (i = 0; i < header_show_flag.size(); i++)
    {
        str = str + QVariant(header_show_flag[i]).toString() + ",";
    }
    str.chop(1);
    return str;
}

//=======================================================================================
//
//=======================================================================================
void SqlTable::refresh()
{
    int i, k, cc, q, qq, sel_id, a;
    QTableWidgetItem *newItem;
    QDate date;
    QVector<int> v;
    QStringList strl;
    QString str;
    QString str2;

    // запаминаем текущую выбранную строчку
    sel_id = get_selected_id();

    clear();
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode((row_selection_mode)
                         ? (QAbstractItemView::ExtendedSelection)
                         : (QAbstractItemView::SingleSelection));
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    //==============================================================================
    cc = header_labels.size();
    header_show_flag.resize(cc);

    i = 0;
    for (k = 0; k < cc; k++)
    {
        if (header_show_flag[k])
        {
            strl << header_labels[k];
            v << header_sizes[k];
        }
    }

    if (columnCount() != strl.size())
    {
        setColumnCount(strl.size());
        for (k = 0; k < v.size(); k++)
        {
            horizontalHeader()->resizeSection(k, v[k]);
        }
    }
    setHorizontalHeaderLabels(strl);
    //==============================================================================

    verticalHeader()->setDefaultSectionSize(font().pointSize() + 14);
    verticalHeader()->hide();

    QSqlQuery query;
    QSqlRecord query_rec;
    QColor color;
    QColor color_1 = QColor("white");
    QColor color_2 = QColor("wheat");

    str2 = query_str_order_by;
    q = 0;
    str = "SELECT " + query_str_pk_field + ",";
    if (!query_str_color_field.isEmpty()) str += query_str_color_field + ",";
    for (i = 0; i < cc; i++)
    {
        if (header_show_flag[i])
        {
            str += query_fields[i] + ",";
            if ((horizontalHeader()->sortIndicatorSection() == q)
                && horizontalHeader()->isSortIndicatorShown())
            {
                str2 = " ORDER BY " + query_fields[i];
                if (horizontalHeader()->sortIndicatorOrder()
                    == Qt::DescendingOrder)
                {
                    str2 += " DESC ";
                } else
                {
                    str2 += " ASC ";
                }
            }
            q++;
        }
    }
    str.chop(1);
    str += " ";

    if (!query.exec(QString(str + query_str_main + query_str_where
                            + query_str_group_by + str2 + query_str_limit)))
    {
        sql_error_message(query, this);
        return;
    }

    setRowCount(query.size());
    pk_values.resize(query.size());

    i = 0;
    qq = 1;
    if (!query_str_color_field.isEmpty()) { qq = 2; }
    query_rec = query.record();
    v.clear();
    for (k = 0; k < q; k++)
    {
        v << query_rec.field(k + qq).type();
    }
    while (query.next())
    {
        pk_values[i] = query.value(0).toInt();
        color = color_1;
        if (rows_highlighting && (i & 1)) color = color_2;
        if (qq >= 2)
        {
            color = int2color(query.value(1).toInt());
            // switch( query.value(1).toInt() )
            //{ case( 10 ): color.setNamedColor("lightpink");   break;
            //  case( 20 ): color.setNamedColor("yellow");   break;
            //  case( 30 ): color.setNamedColor("white");   break;
            //  case( 40 ): color.setNamedColor("lightgreen");   break;
            //}
        }
        for (k = 0; k < q; k++)
        {
            switch (v[k])
            {
            case (QVariant::Int):
            case (QVariant::Double):
            case (QVariant::LongLong):
                newItem = new QTableWidgetItem;
                newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                if (!query.value(k + qq).isNull())
                {
                    newItem->setText(query.value(k + qq).toString());
                }
                a = alignment.value(k);
                if (a == 0) a = Qt::AlignRight | Qt::AlignVCenter;
                newItem->setTextAlignment(a);
                newItem->setBackground(color);
                setItem(i, k, newItem);
                break;
            case (QVariant::String):
            case (QVariant::ByteArray):
                newItem = new QTableWidgetItem;
                newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                if (!query.value(k + qq).isNull())
                {
                    newItem->setText(
                        query.value(k + qq).toString().replace("\n", " "));
                }
                a = alignment.value(k);
                if (a == 0) a = Qt::AlignLeft | Qt::AlignVCenter;
                newItem->setTextAlignment(a);
                newItem->setBackground(color);
                setItem(i, k, newItem);
                break;
            case (QVariant::Date):
                newItem = new QTableWidgetItem;
                newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                date = QDate::fromString(query.value(k + qq).toString(),
                                         "yyyy-MM-dd");
                if (!query.value(k + qq).isNull())
                    newItem->setText(date.toString("dd.MM.yy"));
                newItem->setTextAlignment(Qt::AlignCenter);
                newItem->setBackground(color);
                setItem(i, k, newItem);
                break;
            default:
                QMessageBox::critical(this, QObject::tr("Неизвестный тип"),
                                      QString::number(v[k]));
                break;
            }
        }
        i++;
    }

    // восстановление выбора текущей строчки
    if (sel_id) { setCurrentCell(pk_values.indexOf(sel_id), 0); }
}

//=======================================================================================
//
//=======================================================================================
int SqlTable::get_selected_id()
{
    QList<QTableWidgetSelectionRange> sr = selectedRanges();
    if (sr.count() == 0) return -1;
    return pk_values.value(sr.at(0).topRow(), -1);
}

//=======================================================================================
//
//=======================================================================================
QVector<int> SqlTable::get_selected_id_vect()
{
    QVector<int> v;
    QList<QTableWidgetSelectionRange> sr = selectedRanges();

    int i, j, k;
    for (i = 0; i < sr.count(); i++)
    {
        for (j = sr.at(i).topRow(); j <= sr.at(i).bottomRow(); j++)
        {
            k = pk_values.value(j, -1);
            if (k >= 0) v << k;
        }
    }
    return v;
}

//=======================================================================================
//
//=======================================================================================
int SqlTable::set_selected_id(int id)
{
    int i;
    i = pk_values.indexOf(id);
    if (i < 0) return 0;
    selectRow(i);
    return 1;
}

//=======================================================================================
//
//=======================================================================================
void SqlTable::my1(const QPoint &pos)
{
    int i;

    QAction *item;
    QVector<QAction *> v;

    QMenu *menu = new QMenu(this);

    for (i = 0; i < header_show_flag.size(); i++)
    {
        item = menu->addAction(header_labels[i]);
        item->setCheckable(true);
        if (header_show_flag[i]) item->setChecked(true);
        v << item;
    }
    menu->addSeparator();
    item = menu->addAction(tr("Отключить все"));

    QAction *item_selected = menu->exec(mapToGlobal(pos));
    if (item_selected == item)
    {
        header_show_flag.fill(0);
        header_show_flag[0] = 1;
        refresh();
        return;
    }

    for (i = 0; i < v.size(); i++)
    {
        if (v[i] == item_selected)
        {
            header_show_flag[i] = v[i]->isChecked();
            refresh();
            break;
        }
    }
    // если выключены все поля -- включаем все поля
    for (i = 0; i < v.size(); i++)
        if (header_show_flag[i]) break;
    if (i == v.size())
    {
        header_show_flag.fill(1);
        refresh();
    }
}

//=======================================================================================
//
//=======================================================================================
void SqlTable::my2(int i)
{
    QHeaderView *hw = horizontalHeader();

    if ((hw->sortIndicatorSection() == i) && (hw->isSortIndicatorShown()))
    {
        if (hw->sortIndicatorOrder() == Qt::DescendingOrder)
        {
            hw->setSortIndicatorShown(false);
        } else
        {
            hw->setSortIndicator(i, Qt::AscendingOrder);
        }
    } else
    {
        hw->setSortIndicatorShown(true);
        hw->setSortIndicator(i, Qt::DescendingOrder);
    }

    refresh();
}

//#######################################################################################
//
//#######################################################################################
KomplTable::KomplTable(QWidget *parent)
    : QTableWidget(parent)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Inactive, QPalette::Highlight,
                 pal.color(QPalette::Active, QPalette::Highlight));
    pal.setColor(QPalette::Inactive, QPalette::HighlightedText,
                 pal.color(QPalette::Active, QPalette::HighlightedText));
    setPalette(pal);
}

//=======================================================================================
//
//=======================================================================================
void KomplTable::refresh(int id)
{
    QStringList sl;
    QTableWidgetItem *newitem;
    int i, ii, j, k;
    QColor color;
    QColor color2;
    QColor color3;
    QColor color4;
    QString str, str1, str2;

    QFont bold_font = qApp->font();
    bold_font.setBold(true);

    // int current_row = currentRow();
    if (id < 1) return;

    current_id = id;
    i = columnCount();
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    sl << tr("Наименование") // 0
       << tr("Номинал")      // 1
       << tr("Требуется")    // 2
       << tr("Снято")        // 3
       << tr("Место")        // 4
       << tr("В наличии")    // 5
       << tr("Статус");      // 6
    setColumnCount(7);
    setHorizontalHeaderLabels(sl);

    if (i == 0)
    {
        horizontalHeader()->resizeSection(0, 200);
        horizontalHeader()->resizeSection(1, 100);
        horizontalHeader()->resizeSection(2, 70);
        horizontalHeader()->resizeSection(3, 70);
        horizontalHeader()->resizeSection(4, 70);
        horizontalHeader()->resizeSection(5, 70);
        horizontalHeader()->resizeSection(6, 120);
    }

    // int vs_value = verticalScrollBar()->value();
    verticalHeader()->setDefaultSectionSize(font().pointSize() + 14);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    // setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
    setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    verticalHeader()->hide();

    QSqlQuery query;
    query.prepare(
        " SELECT kompl.id, kompl.color, kompl.type, typename, kompl.name, "
        "kompl.need, "
        "        kompl.snato, zamena.name, zamena.id, prihod2.sostatok, "
        "        zamena.snato, prihod2.mesto, kompl.nominal, kompl.status"
        " FROM kompl "
        "    LEFT JOIN types  ON kompl.type   = types.id     "
        "    LEFT JOIN zamena ON kompl.id     = zamena.kompl "
        "    LEFT JOIN  ( SELECT name, mesto, sum(ostatok) AS sostatok "
        "                   FROM prihod GROUP BY name,mesto ) "
        "         AS prihod2  ON zamena.name  = prihod2.name "
        " WHERE sostav = ? "
        " ORDER BY typename, kompl.name, zamena.id, prihod2.mesto");
    query.addBindValue(id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }

    v_id.resize(query.size());
    i = 0;
    ii = 0;
    j = -1;
    while (query.next())
    {
        if (rowCount() < (i + 1)) setRowCount(i + 1);
        v_id[ii] = query.value(0).toInt();
        color.setNamedColor("white");
        color3.setNamedColor("peru");
        color4.setNamedColor("darkgray");

        color = int2color(query.value(1).toInt());

        //    switch( query.value(1).toInt() )
        //    { case( 10 ): color.setNamedColor("lightpink");    break;
        //      case( 20 ): color.setNamedColor("yellow");       break;
        //      case( 30 ): color.setNamedColor("white");        break;
        //      case( 40 ): color.setNamedColor("lightgreen");   break;
        //    }

        if ((ii == 0) || (v_id[ii] != v_id[ii - 1]))
        {
            if (j != query.value(2).toInt())
            {
                j = query.value(2).toInt();
                newitem = new QTableWidgetItem;
                newitem->setFlags(Qt::ItemIsEnabled);
                newitem->setData(Qt::UserRole + 3, 10);
                newitem->setText(sql_get_string(query, 3));
                newitem->setFont(bold_font);
                newitem->setBackground(color4);
                setItem(i, 0, newitem);
                for (k = 1; k < 7; k++)
                {
                    newitem = new QTableWidgetItem;
                    newitem->setFlags(Qt::ItemIsEnabled);
                    newitem->setBackground(color4);
                    setItem(i, k, newitem);
                }
                i++;
                if (rowCount() < (i + 1)) setRowCount(i + 1);
            }

            newitem = new QTableWidgetItem; // наименование
            newitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            str1 = sql_get_string(query, 4);
            newitem->setText(str1);
            newitem->setData(Qt::UserRole, query.value(0));
            newitem->setData(Qt::UserRole + 1, str1);
            newitem->setData(Qt::UserRole + 2, query.value(8).toInt());
            newitem->setData(Qt::UserRole + 3, 1);
            newitem->setData(Qt::UserRole + 4, query.value(1).toInt() == 10);
            newitem->setBackground(color);
            setItem(i, 0, newitem);

            newitem = new QTableWidgetItem; // номинал
            newitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            newitem->setText(sql_get_string(query, 12));
            newitem->setBackground(color);
            setItem(i, 1, newitem);

            newitem = new QTableWidgetItem; // требуется
            newitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            newitem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            newitem->setText(query.value(5).toString());
            newitem->setBackground(color);
            setItem(i, 2, newitem);

            newitem = new QTableWidgetItem; // место
            newitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            newitem->setText(sql_get_string(query, 11));
            newitem->setData(Qt::UserRole, query.value(0));
            newitem->setBackground(color);
            setItem(i, 4, newitem);

            newitem = new QTableWidgetItem; // снято
            newitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            newitem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            newitem->setText(query.value(6).toString());
            newitem->setBackground(color);
            setItem(i, 3, newitem);
        } else
        {
            newitem = new QTableWidgetItem; // наименование
            newitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            str2 = sql_get_string(query, 7);
            if (str1 != str2)
            {
                newitem->setData(Qt::UserRole + 3, 2);
                newitem->setText(tr("    ЗАМЕНА: ") + str2);
            } else
            {
                newitem->setData(Qt::UserRole + 3, 3);
                newitem->setText(tr("----- // -----"));
            }
            str1 = str2;
            newitem->setData(Qt::UserRole + 1, str1);
            newitem->setData(Qt::UserRole + 2, query.value(8).toInt());
            newitem->setBackground(color);
            setItem(i, 0, newitem);


            newitem = new QTableWidgetItem; // номинал
            newitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            newitem->setBackground(color);
            setItem(i, 1, newitem);

            newitem = new QTableWidgetItem; // нужно
            newitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            newitem->setBackground(color);
            setItem(i, 2, newitem);

            newitem = new QTableWidgetItem; // место
            newitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            newitem->setText(sql_get_string(query, 11));
            newitem->setData(Qt::UserRole, query.value(0));
            newitem->setBackground(color);
            setItem(i, 4, newitem);

            newitem = new QTableWidgetItem; // снято
            newitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            newitem->setBackground(color);
            setItem(i, 3, newitem);
        }

        newitem = new QTableWidgetItem; // в наличии
        newitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        newitem->setText(sql_get_string(query, 9));
        newitem->setBackground(color);
        setItem(i, 5, newitem);

        newitem = new QTableWidgetItem; // статус
        newitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        str = kompl_status.value(query.value(13).toInt());
        newitem->setText(str);
        newitem->setBackground(color);
        setItem(i, 6, newitem);

        i++;
        ii++;
    }
    setRowCount(i);

    for (i = 0; i < rowCount(); i++)
    {
        if (item(i, 0)->text() != "----- // -----") continue;
        if (item(i, 5)->text().toInt() > 0) continue;
        removeRow(i);
        i--;
    }
}


//=======================================================================================
//
//=======================================================================================
void KomplTable::add_zamena()
{
    int i = currentRow();
    if (i < 0) return;

    if (item(i, 0)->data(Qt::UserRole + 3).toInt() != 1) return;

    ZamenaAddDialog dialog(this, item(i, 0)->data(Qt::UserRole).toInt());
    if (dialog.exec() == QDialog::Accepted)
        mainwindow_ptr->on_action_Pereraschet_triggered();
}

//=======================================================================================
//
//=======================================================================================
void KomplTable::status()
{
    int j;
    int i = currentRow();
    if (i < 0) return;
    if (item(i, 0)->data(Qt::UserRole + 3).toInt() != 1) return;
    i = item(i, 0)->data(Qt::UserRole).toInt();

    QMenu *menu = new QMenu(this);

    QAction *item1 = menu->addAction("Не комплектовать");
    QAction *item2 = menu->addAction("Не отправлять");
    menu->addSeparator();
    QAction *item3 = menu->addAction("Очистить");
    QAction *item_selected = menu->exec(QCursor::pos());
    j = 0;
    if (item_selected == item1)
        j = 1;
    else if (item_selected == item2)
        j = 2;
    else if (item_selected == item3)
        j = 0;
    else
        return;

    QSqlQuery query;
    query.prepare(" UPDATE kompl SET status = ? WHERE id = ? ");
    query.addBindValue(j);
    query.addBindValue(i);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }

    refresh(current_id);
}

//#######################################################################################
//
//#######################################################################################
Defichit2::Defichit2(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(le, SIGNAL(returnPressed()), this, SLOT(refresh()));
    connect(pb_exec, SIGNAL(clicked()), this, SLOT(refresh()));
}

//=======================================================================================
//
//=======================================================================================
void Defichit2::refresh()
{
    int i, j, k;
    QString str;
    QStringList sl;
    QTableWidgetItem *titem;

    QFont bold_font = qApp->font();
    bold_font.setBold(true);

    tw->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw->setSelectionMode(QAbstractItemView::SingleSelection);

    sl << "Наименование"
       << "Нужно"
       << "В наличии"
       << "В закупке"
       << "Номера заданий";
    tw->setColumnCount(5);
    tw->setHorizontalHeaderLabels(sl);

    tw->horizontalHeader()->resizeSection(0, 200);
    tw->horizontalHeader()->resizeSection(1, 100);
    tw->horizontalHeader()->resizeSection(2, 100);
    tw->horizontalHeader()->resizeSection(3, 100);
    tw->horizontalHeader()->resizeSection(4, 100);
    // tw->horizontalHeader()->setStretchLastSection( true );

    tw->verticalHeader()->setDefaultSectionSize(font().pointSize() + 14);
    tw->verticalHeader()->hide();

    str = le->text();
    if (str == 0) return;

    QRegExp rx("(\\d+)");
    i = 0;
    sl.clear();
    while ((i = rx.indexIn(str, i)) != -1)
    {
        sl << rx.cap(1);
        i += rx.matchedLength();
    }
    QString sostav_id = sl.join(",");

    QSqlQuery query;

    query.prepare(
        " SELECT t1.type, t1.typename, t1.name, t1.treb, t2.nalichie, "
        "t1.sost_id, zakupka "
        " FROM ( "
        "   SELECT MAX(kompl.type) AS type, MAX(typename) AS typename, "
        "zamena.name,SUM(kompl.need-kompl.snato) AS treb, "
        "          GROUP_CONCAT( kompl.sostav ) AS sost_id "
        "   FROM kompl "
        "   LEFT JOIN zamena ON zamena.kompl=kompl.id "
        "   LEFT JOIN types ON types.id=kompl.type "
        "   WHERE kompl.sostav IN ( "
        + sostav_id
        + " ) "
          "   GROUP BY zamena.name "
          "   ORDER BY MAX(typename) ASC ) AS t1 "
          " LEFT JOIN ( "
          "   SELECT name,SUM(ostatok) AS nalichie FROM prihod GROUP BY name "
          "   ) AS t2 ON t1.name = t2.name "
          " LEFT JOIN ( "
          "   SELECT name,SUM(n-polucheno) AS zakupka FROM zakupki GROUP BY "
          "name "
          "   ) AS t3 ON t1.name = t3.name "
          " WHERE typename != \'Модуль\' ");
    query.exec();
    i = 0, j = -1;
    while (query.next())
    {
        if (tw->rowCount() < i + 1) tw->setRowCount(i + 1);
        if (query.value(0).toInt() != j)
        {
            j = query.value(0).toInt();
            titem = new QTableWidgetItem;
            titem->setFlags(Qt::ItemIsEnabled);
            titem->setFont(bold_font);
            titem->setText(query.value(1).toString());
            titem->setBackground(QColor("darkgrey"));
            titem->setData(Qt::UserRole, 1);
            tw->setItem(i, 0, titem);
            for (k = 1; k < 5; k++)
            {
                titem = new QTableWidgetItem;
                titem->setFlags(Qt::ItemIsEnabled);
                titem->setBackground(QColor("darkgrey"));
                tw->setItem(i, k, titem);
            }
            i++;
            if (tw->rowCount() < i + 1) tw->setRowCount(i + 1);
        }
        // наименование
        titem = new QTableWidgetItem;
        titem->setFlags(Qt::ItemIsEnabled);
        titem->setText(query.value(2).toString());
        titem->setData(Qt::UserRole + 1, query.value(0));
        tw->setItem(i, 0, titem);
        // нужно
        titem = new QTableWidgetItem;
        titem->setFlags(Qt::ItemIsEnabled);
        titem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        titem->setText(query.value(3).toString());
        tw->setItem(i, 1, titem);
        // в наличии
        titem = new QTableWidgetItem;
        titem->setFlags(Qt::ItemIsEnabled);
        titem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        titem->setText(query.value(4).toString());
        tw->setItem(i, 2, titem);
        // в закупке
        titem = new QTableWidgetItem;
        titem->setFlags(Qt::ItemIsEnabled);
        titem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        titem->setText(query.value(6).toString());
        tw->setItem(i, 3, titem);
        // номера заданий
        titem = new QTableWidgetItem;
        titem->setFlags(Qt::ItemIsEnabled);
        titem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        titem->setText(query.value(5).toString());
        tw->setItem(i, 4, titem);

        if (query.value(4).toInt() < query.value(3).toInt())
        {
            for (k = 0; k < 5; k++)
            {
                tw->item(i, k)->setBackground(QColor("pink"));
                tw->item(i, k)->setData(Qt::UserRole, 2);
            }
        }
        i++;
    }
    tw->setRowCount(i);
}

//=======================================================================================
//
//=======================================================================================
void Defichit2::on_pb_print_clicked()
{
    int i, k;
    QString str;
    QStringList sl;

    str = le->text();
    QRegExp rx("(\\d+)");
    i = 0;
    sl.clear();
    while ((i = rx.indexIn(str, i)) != -1)
    {
        sl << rx.cap(1);
        i += rx.matchedLength();
    }
    QString sostav_id = sl.join(",");
    if (sostav_id.length() == 0) return;
    str.clear();

    QSqlQuery query;
    query.prepare(
        " SELECT t1.type, t1.typename, t1.name, t1.treb, t2.nalichie, "
        "t1.sost_id "
        " FROM ( "
        "   SELECT MAX(kompl.type) AS type, MAX(typename) AS typename, "
        "zamena.name,SUM(kompl.need-kompl.snato) AS treb, "
        "          GROUP_CONCAT( kompl.sostav ) AS sost_id "
        "   FROM kompl "
        "   LEFT JOIN zamena ON zamena.kompl=kompl.id "
        "   LEFT JOIN types ON types.id=kompl.type "
        "   WHERE kompl.sostav IN ( "
        + sostav_id
        + " ) "
          "   GROUP BY zamena.name "
          "   ORDER BY MAX(typename) ASC ) AS t1 "
          " LEFT JOIN ( "
          "   SELECT name,SUM(ostatok) AS nalichie FROM prihod GROUP BY name "
          "   ) AS t2 ON t1.name = t2.name "
          " WHERE t1.treb > IFNULL(t2.nalichie,0) AND typename != \'Модуль\'");
    query.exec();

    str += "<HTML>";
    str += " <style type=\"text/css\"> body {  font-family: \"Arial\"; "
           "font-size: 10pt  } </style> ";
    str += " <BODY> ";
    str += "<H2 align=center>Анализ дефицита</H2>";
    str += "<TABLE>";
    str += "<TR> <TD>Дата: <TD>"
         + QDate::currentDate().toString("dd MMMM yyyy (dddd)");
    str += "<TR> <TD>Номера заданий: <TD>" + sostav_id;
    str += "</TABLE>";

    str += " <TABLE style=\"border-style: solid;border-color: black\" "
           "border=0.5 cellpadding=2 cellspacing=0 width=100%> ";
    str += " <THEAD style=\"font-family:Tahoma\" ><TR bgcolor=gray >"
           "<TD width=40%>Наименование<TD>Требуется<TD>Наличие<TD "
           "width=40%>Номера заданий</THEAD>";
    k = -1;
    while (query.next())
    {
        if (query.value(0).toInt() != k)
        {
            k = query.value(0).toInt();
            str += "<TR>";
            str += " <TD style=\"font-family:Tahoma\" bgcolor=lightgray "
                   "colspan=4 > <B>"
                 + query.value(1).toString() + "</B> </TD>";
            str += "</TR>";
        }
        str += "<TR>";
        str += "<TD>" + query.value(2).toString() + "</TD>";
        str += "<TD>" + QString::number(query.value(3).toInt()) + "</TD>";
        str += "<TD>" + QString::number(query.value(4).toInt()) + "</TD>";
        str += "<TD>" + query.value(5).toString() + "</TD>";
        str += "</TR>";
    }
    str += "</TABLE> ";
    str += "</BODY> ";
    str += "</HTML>";

    QPrinter printer;
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle("Печать");
    if (dialog->exec() != QDialog::Accepted) return;

    QTextDocument td;
    td.setHtml(str);
    td.print(&printer);
}

//=======================================================================================
//
//=======================================================================================
void Defichit2::on_pb_copy_clicked()
{
    int i;
    QString str;
    int n = tw->rowCount();

    for (i = 0; i < n; i++)
    {
        if (tw->item(i, 0)->data(Qt::UserRole).toInt() > 0)
        {
            str += tw->item(i, 0)->text() + "\n";
        }
    }

    QApplication::clipboard()->setText(str);
    QMessageBox::information(this, app_header,
                             "Наименования дефицитных позиций "
                             "скопированны в буфер обмена");
}

//=======================================================================================
//
//=======================================================================================
void Defichit2::on_tw_cellDoubleClicked(int row, int col)
{
    Q_UNUSED(col);

    if (tw->item(row, 0)->data(Qt::UserRole).toInt() == 1) return;

    Defichit2RenameDialog dialog(
        this, tw->item(row, 0)->text(),
        tw->item(row, 0)->data(Qt::UserRole + 1).toInt(),
        tw->item(row, 4)->text());

    if (dialog.exec() == QDialog::Accepted) { refresh(); }
}

//=======================================================================================
//
//=======================================================================================
void Defichit2::on_pb_price_clicked()
{
    refresh();
    /*
      int i,a;
      QString str;
      int n = tw->rowCount();
      QStringList sl;

      for( i=0; i<n;  i++ )
      { if( tw->item(i,0)->data(Qt::UserRole).toInt() > 0 )
        { if( !tw->item(i,1)->text().isEmpty() )
          { a =  tw->item(i,1)->text().toInt() - tw->item(i,2)->text().toInt();
            if( a > 0 )
            { sl << tw->item(i,0)->text()+";" + QString::number(a);
            }
          } else
          { sl << tw->item(i,0)->text()+";";
          }
        }
      }
    */
    DefichitPriceDialog dialog(this, le->text());
    dialog.exec();
}

//#######################################################################################
//
//#######################################################################################
Defichit2RenameDialog::Defichit2RenameDialog(QWidget *parent,
                                             const QString &name, int type,
                                             const QString &kompl)
    : QDialog(parent)
{
    setupUi(this);
    setWindowTitle("Переименование");

    this->name = name;
    this->type = type;
    this->kompl = kompl;

    le_name->setText(name);
    le_kompl->setText(kompl);
    le_filtr->setText(name);

    tw->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw->setSelectionMode(QAbstractItemView::SingleSelection);
    tw->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList sl;
    sl << "Наименование"
       << "Наличие"
       << "В закупке";
    tw->setRowCount(0);
    tw->setColumnCount(3);
    tw->setHorizontalHeaderLabels(sl);

    tw->horizontalHeader()->resizeSection(0, 250);
    tw->horizontalHeader()->resizeSection(1, 70);
    tw->horizontalHeader()->resizeSection(2, 70);
    tw->horizontalHeader()->setStretchLastSection(true);

    tw->verticalHeader()->setDefaultSectionSize(font().pointSize() + 14);
    tw->verticalHeader()->hide();

    QSqlQuery query;
    query.prepare("SELECT typename FROM types WHERE id = ?");
    query.addBindValue(type);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    query.next();
    le_type->setText(query.value(0).toString());

    on_le_filtr_textChanged(le_name->text());
}

//=======================================================================================
//
//=======================================================================================
void Defichit2RenameDialog::on_le_filtr_textChanged(const QString &str_arg)
{
    int i;
    QString str = " " + str_arg + " ";
    str.replace(" ", "%");
    QSqlQuery query;
    query.prepare(" SELECT DISTINCT t1.name,t2.s1, t3.s2 FROM "
                  "           ( SELECT name,type FROM prihod UNION SELECT "
                  "name,type FROM zakupki )  AS t1 "
                  " LEFT JOIN ( SELECT name,sum(ostatok) AS s1 FROM prihod "
                  "GROUP BY name )      as t2 ON t1.name = t2.name "
                  " LEFT JOIN ( SELECT name,sum(n-polucheno) AS s2 FROM "
                  "zakupki GROUP BY name ) as t3 ON t1.name = t3.name "
                  " WHERE type = ? AND t1.name LIKE ? ORDER BY t1.name ASC ");
    query.addBindValue(type);
    query.addBindValue(str);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    tw->setRowCount(query.size());
    i = 0;
    while (query.next())
    {
        tw->setItem(i, 0, new QTableWidgetItem(query.value(0).toString()));
        tw->setItem(i, 1, new QTableWidgetItem(query.value(1).toString()));
        tw->setItem(i, 2, new QTableWidgetItem(query.value(2).toString()));
        i++;
    }
}

//=======================================================================================
//
//=======================================================================================
void Defichit2RenameDialog::on_pb_rename_clicked()
{
    QList<QTableWidgetSelectionRange> sr = tw->QTableWidget::selectedRanges();
    if (sr.size() != 1) return;
    QString str = tw->item(sr.at(0).topRow(), 0)->text();

    if (str == name) return;

    QSqlQuery query;
    QString query_str
        = " DELETE zamena FROM  kompl,zamena"
          " WHERE kompl.sostav IN( "
        + kompl
        + " ) "
          " AND kompl.name=? AND zamena.name=? AND zamena.kompl=kompl.id ";
    query.prepare(query_str);
    query.addBindValue(name);
    query.addBindValue(str);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }

    query_str = " UPDATE kompl,zamena  SET kompl.name=?, zamena.name=? "
                " WHERE kompl.sostav IN( "
              + kompl
              + " ) "
                "       AND kompl.name=? AND zamena.name=? AND "
                "zamena.kompl=kompl.id ";
    query.prepare(query_str);
    query.addBindValue(str);
    query.addBindValue(str);
    query.addBindValue(name);
    query.addBindValue(name);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    accept();
}

//#######################################################################################
// Диалог анализа закупки
//#######################################################################################
struct DefichitPriceDialogStruct
{
    QString type;
    QString kompl_name;
    QString zamena_name;
    int need;
};

static inline void map_insert(QMap<QString, int> &map, const QString &str,
                              const QString &str_type, int i)
{
    QString s = str_type + ";" + str;
    map.insert(s, map.value(s, 0) + i);
}

DefichitPriceDialog::DefichitPriceDialog(QWidget *parent,
                                         const QString &items_str)
    : QDialog(parent)
{
    setupUi(this);
    setWindowTitle("Анализ стоимости дефицита");
    resize(600, 600);

    QSqlQuery query;
    QStringList sl;
    int i, n;
    QString s;
    QMap<QString, int> map;
    QMap<QString, int> ost_map;
    QMap<QString, double> price_est_map;
    QMap<QString, double> price_map;

    QFont bold_font = qApp->font();
    bold_font.setBold(true);

    //------------------------------------------------------------------------------------
    // подготовка таблицы

    sl << "Наименование"    // 0
       << "Кол-во"          // 1
       << "Цена\n(оценоч.)" // 2
       << "Цена\n(закуп.)"  // 2
       << "Стоимость";      // 3
    tw->setColumnCount(5);
    tw->setHorizontalHeaderLabels(sl);

    tw->horizontalHeader()->resizeSection(0, 200);
    tw->horizontalHeader()->resizeSection(1, 70);
    tw->horizontalHeader()->resizeSection(2, 70);
    tw->horizontalHeader()->resizeSection(3, 70);
    tw->horizontalHeader()->resizeSection(4, 70);

    tw->verticalHeader()->setDefaultSectionSize(font().pointSize() + 14);
    tw->verticalHeader()->hide();
    tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tw->setSelectionMode(QAbstractItemView::NoSelection);


    //------------------------------------------------------------------------------------
    // текущий остаток на складе
    query.prepare(" SELECT MAX(typename), name, SUM( ostatok ) "
                  " FROM prihod "
                  " LEFT JOIN types ON prihod.type=types.id "
                  " GROUP BY prihod.name ");
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    while (query.next())
    {
        ost_map.insert(query.value(0).toString() + ";"
                           + query.value(1).toString(),
                       query.value(2).toInt());
    }

    //------------------------------------------------------------------------------------
    // получение оценочных цен
    query.prepare("SELECT name,price_est FROM price_est ");
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    while (query.next())
    {
        price_est_map.insert(query.value(0).toString(),
                             query.value(1).toDouble());
    }

    //------------------------------------------------------------------------------------
    // получение закупочных цен
    query.prepare(
        "SELECT name,SUM(price*kolichestvo)/SUM(kolichestvo) AS avr_price "
        " FROM prihod GROUP BY name");
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    while (query.next())
    {
        if (query.isNull(1)) continue;
        price_map.insert(query.value(0).toString(), query.value(1).toDouble());
    }

    //------------------------------------------------------------------------------------
    // получение списка необходимых компонентов
    QRegExp rx("(\\d+)");
    i = 0;
    sl.clear();
    while ((i = rx.indexIn(items_str, i)) != -1)
    {
        sl << rx.cap(1);
        i += rx.matchedLength();
    }

    QStringList items;
    QVector<DefichitPriceDialogStruct> v;
    DefichitPriceDialogStruct vi;

    foreach (QString s, sl)
    {
        query.prepare(" SELECT typename,kompl.name,zamena.name,kompl.need "
                      " FROM kompl "
                      " LEFT JOIN zamena ON zamena.kompl=kompl.id "
                      " LEFT JOIN types ON kompl.type=types.id "
                      " WHERE sostav = ? "
                      " ORDER BY typename ASC,kompl.name ASC ,zamena.id ASC ");
        query.addBindValue(s);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        v.clear();
        while (query.next())
        {
            vi.type = query.value(0).toString();
            vi.kompl_name = query.value(1).toString();
            vi.zamena_name = query.value(2).toString();
            vi.need = query.value(3).toInt();
            v << vi;
        }
        v << vi;

        // анализ замен: при наличии замен используем первую замену
        n = v.count();
        for (i = 0; i < n - 1; i++)
        {
            if (v[i].kompl_name != v[i + 1].kompl_name)
            {
                map_insert(map, v[i].kompl_name, v[i].type, v[i].need);
            } else
            {
                map_insert(map, v[i + 1].zamena_name, v[i].type, v[i].need);
                for (; i < n - 1; i++)
                {
                    if (v[i].kompl_name != v[i + 1].kompl_name) break;
                }
            }
        }
    }

    //---------------------------------------------------------------------
    // сравнение с текущими остатками
    foreach (QString k, map.keys())
    {
        int a = map.value(k) - ost_map.value(k);
        if (a > 0)
        {
            map.insert(k, a);
        } else
        {
            map.remove(k);
        }
    }

    //---------------------------------------------------------------------
    // заполнение таблицы
    foreach (QString k, map.keys())
    {
        QString s0 = k.section(";", 0, 0);
        if (s != s0)
        {
            items << s0;
            s = s0;
        }
        items << (k.section(";", 1, 1) + ";" + QString::number(map.value(k)));
    }
    //---------------------------------------------------------------------
    // расчет и вывод таблицы
    n = items.count();
    tw->setRowCount(n);

    for (i = 0; i < n; i++)
    {
        tw->setItem(i, 0, new QTableWidgetItem(items.at(i).section(";", 0, 0)));
        tw->setItem(i, 1, new QTableWidgetItem(items.at(i).section(";", 1, 1)));
    }

    for (i = 0; i < n; i++)
    {
        if (tw->item(i, 1)->text().isEmpty())
        {
            tw->setSpan(i, 0, 0, 5);
            tw->item(i, 0)->setBackground(QColor("darkgrey"));
            tw->item(i, 0)->setFont(bold_font);
        }
    }

    for (i = 0; i < n; i++)
    {
        QString s = tw->item(i, 0)->text();
        if (price_est_map.contains(s) && !price_map.contains(s))
        {
            tw->setItem(i, 2,
                        new QTableWidgetItem(
                            QString::number(price_est_map.value(s), 'f', 2)));
        } else
        {
            tw->setItem(i, 2, new QTableWidgetItem(""));
        }
        if (price_map.contains(s))
        {
            tw->setItem(i, 3,
                        new QTableWidgetItem(
                            QString::number(price_map.value(s), 'f', 2)));
        } else
        {
            tw->setItem(i, 3, new QTableWidgetItem(""));
        }
    }

    for (i = 0; i < n; i++)
    {
        QString p1 = tw->item(i, 2)->text();
        QString p2 = tw->item(i, 3)->text();
        double p = 0;
        int n = 0;
        if (!p1.isEmpty())
            p = p1.toDouble();
        else if (!p2.isEmpty())
            p = p2.toDouble();
        n = tw->item(i, 1)->text().toInt();
        if (n)
        {
            tw->setItem(i, 4,
                        new QTableWidgetItem(QString::number(p * n, 'f', 2)));
            if (p < 0.01) { tw->item(i, 4)->setBackground(QColor("pink")); }
        } else
        {
            tw->setItem(i, 4, new QTableWidgetItem(""));
        }
    }

    for (i = 0; i < n; i++)
    {
        tw->item(i, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        tw->item(i, 1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tw->item(i, 2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tw->item(i, 3)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tw->item(i, 4)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    //---------------------------------------------------------------------
    // общая стомиость
    double sp = 0;
    for (i = 0; i < n; i++)
    {
        QString s = tw->item(i, 4)->text();
        if (s.isEmpty()) continue;
        sp += s.toDouble();
    }
    le->setText(QString::number(sp, 'f', 2));
}

//=======================================================================================
// Копирование таблицы в буфер обмена
//=======================================================================================
void DefichitPriceDialog::on_pb_copy_clicked()
{
    QString str = tablewidget2str(tw);
    QApplication::clipboard()->setText(str);
    QMessageBox::information(this, app_header,
                             "Таблица скопирована в буфер обмена.");
}

//#######################################################################################
// Копирование содержания таблицы в строку для буфера обмена (типа CSV)
//#######################################################################################
QString tablewidget2str(const QTableWidget *tw)
{
    QString str;
    QString s;

    QTableWidgetItem *titem;
    int i, j;
    int ni = tw->rowCount();
    int nj = tw->columnCount();

    for (j = 0; j < nj; j++)
    {
        titem = tw->horizontalHeaderItem(j);
        s = titem ? titem->text() : "";
        str += "\"" + s + "\"\t";
    }
    str += "\r\n";

    for (i = 0; i < ni; i++)
    {
        for (j = 0; j < nj; j++)
        {
            titem = tw->item(i, j);
            s = titem ? titem->text() : "";
            str += "\"" + s + "\"\t";
        }
        str.chop(1);
        str += "\r\n";
    }
    str.chop(1);
    return str;
}
