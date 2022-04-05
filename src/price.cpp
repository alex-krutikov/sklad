#include <QtGui>

#include "main.h"
#include "price.h"

#include <QMessageBox>
#include <QSqlQuery>

//#######################################################################################
//
//#######################################################################################
PriceDialog::PriceDialog(QWidget *parent, int sostav_id_arg)
    : QDialog(parent)
    , sostav_id(sostav_id_arg)
{
    setupUi(this);
    int nn;

    QFont fnt = font();
    fnt.setBold(true);
    le1->setFont(fnt);
    //------------------------------------------------------------------------------------
    cb_price_type->addItem("Последняя актуальная", "last_actual");
    cb_price_type->addItem("Усредненная", "avr");
    //------------------------------------------------------------------------------------
    sb_price_months->setMinimum(0);
    sb_price_months->setMaximum(100 * 12);
    sb_price_months->setSingleStep(6);
    //------------------------------------------------------------------------------------
    label_2->setEnabled(false);
    label_3->setEnabled(false);
    sb_price_months->setEnabled(false);
    //------------------------------------------------------------------------------------
    int i, j;
    QString str;
    QString str_typename;
    QTableWidgetItem *titem;
    QSqlQuery query;
    //------------------------------------------------------------------------------------
    query.prepare(
        "SELECT sostav.n1, sostav.name, izdelie.name FROM sostav,izdelie "
        "  WHERE izdelie.id = sostav.izdelie AND sostav.id = ? ");
    query.addBindValue(sostav_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    if (query.size() != 1)
    {
        QMessageBox::critical(this, app_header, "ОШИБКА выборки из базы!");
        return;
    }
    query.next();
    nn = query.value(0).toInt();
    setWindowTitle("Оценка стоимости: " + query.value(1).toString() + " | "
                   + query.value(2).toString());
    //------------------------------------------------------------------------------------
    queryPrices();
    //------------------------------------------------------------------------------------
    QSqlQuery query_price;
    query_price.prepare("SELECT name, price_est FROM price_est");
    if (!query_price.exec())
    {
        sql_error_message(query_price, this);
        return;
    }
    i = 0;
    while (query_price.next())
    {
        price_est[query_price.value(0).toString()] = i;
        i++;
    }
    //------------------------------------------------------------------------------------
    t_data ss;
    query.prepare(" SELECT zamena.name, typename, kompl.need, kompl.name "
                  " FROM kompl "
                  "    LEFT JOIN types  ON kompl.type   = types.id     "
                  "    LEFT JOIN zamena ON kompl.id     = zamena.kompl "
                  " WHERE sostav = ? "
                  " ORDER BY typename, kompl.name, zamena.id ");
    query.addBindValue(sostav_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    while (query.next())
    {
        ss.name = query.value(0).toString();      // zamena.name
        ss.type_name = query.value(1).toString(); // kompl.typename
        ss.need = query.value(2).toInt();         // kompl.need
        if (ss.need % nn)
        {
            QMessageBox::critical(this, app_header,
                                  "ОШИБКА! Кол-во не кратно!");
            return;
        }
        ss.need /= nn;
        ss.snato = ss.need;
        ss.price_real = price.value(ss.name, -1);
        ss.price_est = -1;
        ss.sum = 0;
        if (price_est.contains(ss.name))
        {
            query_price.seek(price_est[ss.name]);
            ss.price_est = query_price.value(1).toDouble();
        }
        ss.zamena_flag
            = query.value(3) != query.value(0); // kompl.name != zamena.name
        if (ss.zamena_flag)
        {
            ss.need = -1;
            ss.snato = -1;
        }
        data << ss;
    }
    //------------------------------------------------------------------------------------
    // смещение кол-ва на первую замену
    for (i = 1; i < data.count(); i++)
    {
        if ((!data[i - 1].zamena_flag) && (data[i].zamena_flag))
        {
            data[i].snato = data[i - 1].snato;
            data[i - 1].snato = -1;
        }
    }
    //------------------------------------------------------------------------------------
    QStringList sl;
    sl << "Наименование"
       << "Кол-во\n(треб.)"
       << "Кол-во\n(факт)"
       << "Цена\n(закуп.)"
       << "Цена\n(оценка)"
       << "Стоимость";

    QColor color("bisque");

    tw->setColumnCount(6);
    tw->verticalHeader()->setDefaultSectionSize(font().pointSize() + 11);
    tw->verticalHeader()->hide();
    tw->setHorizontalHeaderLabels(sl);
    tw->horizontalHeader()->resizeSection(0, 200);
    tw->horizontalHeader()->resizeSection(1, 70);
    tw->horizontalHeader()->resizeSection(2, 70);
    tw->horizontalHeader()->resizeSection(3, 70);
    tw->horizontalHeader()->resizeSection(4, 70);
    tw->horizontalHeader()->resizeSection(5, 70);
    tw->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw->setSelectionMode(QAbstractItemView::SingleSelection);

    j = 0;
    for (i = 0; i < data.count(); i++)
    {
        if (str_typename != data[i].type_name)
        {
            QFont boldfont = font();
            boldfont.setBold(true);
            str_typename = data[i].type_name;
            tw->setRowCount(j + 1);
            titem = new QTableWidgetItem;
            titem->setText(str_typename);
            titem->setFlags(Qt::ItemIsEnabled);
            titem->setBackground(QColor {"gray"});
            titem->setFont(boldfont);
            tw->setItem(j, 0, titem);
            tw->setSpan(j, 0, 1, 7);
            j++;
        }
        tw->setRowCount(j + 1);
        tw2data[j] = i;
        data[i].row_index = j;
        //--------------------- наименование ---
        titem = new QTableWidgetItem;
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        str = data[i].name;
        if (data[i].zamena_flag) str = "  ЗАМЕНА: " + str;
        // titem->setBackgroundColor( color );
        titem->setBackground(QColor {"antiquewhite"});
        titem->setText(str);
        tw->setItem(j, 0, titem);
        //--------------------- требуется ---
        titem = new QTableWidgetItem;
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        if (!data[i].zamena_flag) titem->setText(QString::number(data[i].need));
        titem->setBackground(color);
        tw->setItem(j, 1, titem);
        //--------------------- кол-во ---
        titem = new QTableWidgetItem;
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable
                        | Qt::ItemIsEditable);
        titem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        if (data[i].snato > 0) titem->setText(QString::number(data[i].snato));
        tw->setItem(j, 2, titem);
        //--------------------- цена ---
        titem = new QTableWidgetItem;
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        if (data[i].price_real > 0)
            titem->setText(QString::number(data[i].price_real, 'f', 2));
        titem->setBackground(color);
        tw->setItem(j, 3, titem);
        //--------------------- цена оценка ---
        titem = new QTableWidgetItem;
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable
                        | Qt::ItemIsEditable);
        titem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        if (data[i].price_real <= 0)
        {
            if (data[i].price_est > 0)
                titem->setText(QString::number(data[i].price_est));
        } else
        {
            titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            titem->setBackground(color);
        }
        tw->setItem(j, 4, titem);
        //--------------------- Стоимость ---
        titem = new QTableWidgetItem;
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        titem->setFont(fnt);
        tw->setItem(j, 5, titem);
        j++;
    }

    recalc();

    connect(tw, SIGNAL(cellChanged(int, int)), SLOT(tw_cellChanged(int, int)));

    connect(cb_price_type, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PriceDialog::price_settings_changed);
    connect(sb_price_months, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &PriceDialog::price_settings_changed);
}

//=======================================================================================
//
//=======================================================================================
void PriceDialog::recalc()
{
    int i, j;
    double f, sum;
    QString str;
    QColor color;
    bool b;

    sum = 0;
    for (i = 0; i < data.count(); i++)
    {
        f = data[i].price_real;
        if (f < 0) f = data[i].price_est;
        j = data[i].snato;
        if ((f < 0) || (j < 0))
        {
            str.clear();
            data[i].sum = 0;
        } else
        {
            data[i].sum = f * j;
            str = QString::number(data[i].sum, 'f', 2);
        }
        b = str.isEmpty();
        if (data[i].snato < 0) b = false;
        color = b ? QColor("salmon") : QColor("seagreen");
        tw->item(data[i].row_index, 5)->setBackground(color);
        tw->item(data[i].row_index, 5)->setText(str);
        sum += data[i].sum;
    }
    le1->setText(QString::number(sum, 'f', 2));
}

//=======================================================================================
//
//=======================================================================================
void PriceDialog::queryPrices()
{
    const QColor color1 = QColor{"bisque"};
    const Qt::ItemFlags f1
        = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    const Qt::ItemFlags f2 = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    price.clear();

    int months = 0;
    if (cb_price_type->currentData() == "avr")
        months = sb_price_months->value();

    QSqlQuery query;
    // Запрос для последней актуальной цены
    query.prepare("SELECT name, price FROM prihod WHERE id IN "
                  "( SELECT MAX(id) FROM prihod "
                  " GROUP BY name) ");
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    QMap<QString, double> last_price;
    while (query.next())
    {
        last_price.insert(query.value(0).toString(), query.value(1).toDouble());
    }

    if (cb_price_type->currentData() == "avr")
    {
        // Запрос для средней цены
        query.prepare(
            "SELECT name,SUM(price*kolichestvo)/SUM(kolichestvo) AS avr_price "
            "FROM prihod "
            "WHERE date >= DATE_SUB(NOW(), INTERVAL :period MONTH) "
            "GROUP BY name");
        query.bindValue(":period", months);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        while (query.next())
        {
            if (query.isNull(1)) continue;
            price[query.value(0).toString()] = query.value(1).toDouble();
        }

        // Дополнение средних цен последними актуальными
        // для позиций где нет средней цены

        for (auto it = last_price.constBegin(); it != last_price.constEnd();
             ++it)
        {
            if (!price.contains(it.key())) price.insert(it.key(), it.value());
        }
    } else
    {
        // последняя актуальная цена
        price = std::move(last_price);
    }

    // Обновление таблицы

    for (int i = 0; i < data.count(); i++)
    {
        data[i].price_real = price.value(data[i].name, -1);

        //----------------------------------------------------------------------
        QTableWidgetItem *titem = tw->item(data[i].row_index, 3);
        if (!titem) continue;
        if (data[i].price_real > 0)
            titem->setText(QString::number(data[i].price_real, 'f', 2));
        else
            titem->setText("");

        //----------------------------------------------------------------------
        titem = tw->item(data[i].row_index, 4);
        if (!titem) continue;
        if (data[i].price_real <= 0)
        {
            titem->setBackground(QBrush{});
            if (titem->flags() != f1) titem->setFlags(f1);
            if (data[i].price_est > 0)
                titem->setText(QString::number(data[i].price_est));
        } else
        {
            if (titem->flags() != f2) titem->setFlags(f2);
            titem->setBackground(color1);
            titem->setText("");
        }
    }
}

//=======================================================================================
//
//=======================================================================================
void PriceDialog::tw_cellChanged(int row, int column)
{
    bool ok;
    double f;
    int i;

    if (column == 5) return;

    QString text;
    if (tw->item(row, column))
        text = tw->item(row, column)->text();
    else
        return;

    QSqlQuery query;
    int index = tw2data.value(row, -1);
    if ((index < 0) || (index >= data.count()))
    {
        QMessageBox::critical(this, app_header,
                              "ОШИБКА! Выход за границы массива.");
        return;
    }

    switch (column)
    {
    case (2): // кол-во
        i = text.toInt(&ok);
        if (!ok) i = -1;
        data[index].snato = i;
        recalc();
        break;
    case (4): // цена оценка
        f = text.toDouble(&ok);
        if (!ok) f = -1;
        if (f > 0)
        {
            data[index].price_est = f;

            query.prepare("REPLACE price_est SET price_est = ?, name = ?");
            query.addBindValue(f);
            query.addBindValue(data[index].name);
            if (!query.exec())
            {
                sql_error_message(query, this);
                return;
            }
        }
        recalc();
        break;
    }
}

//=======================================================================================
//
//=======================================================================================
void PriceDialog::on_tw_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column);

    QString str = tw->item(row, 0)->text();
    QApplication::clipboard()->setText(str);
}

//=======================================================================================
//
//=======================================================================================
void PriceDialog::price_settings_changed()
{
    const bool flag = cb_price_type->currentData() == "avr";
    label_2->setEnabled(flag);
    label_3->setEnabled(flag);
    sb_price_months->setEnabled(flag);

    queryPrices();
    recalc();
}

//=======================================================================================
//
//=======================================================================================
void PriceDialog::on_pb_export_clicked()
{
    const QString str = tablewidget2str(tw);
    QApplication::clipboard()->setText(str);
    QMessageBox::information(this, app_header,
                             "Таблица скопирована в буфер обмена.");
}

//=======================================================================================
//
//=======================================================================================
void PriceDialog::on_pb_ok_clicked()
{
    QSqlQuery query;
    query.prepare("UPDATE sostav SET cost_kompl = ? WHERE id = ?");
    query.addBindValue(le1->text());
    query.addBindValue(sostav_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    emit updated();
    close();
}
