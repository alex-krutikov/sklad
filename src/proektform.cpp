#include <QtGui>

#include "main.h"
#include "price.h"
#include "proektform.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlQuery>

//#######################################################################################
//
//#######################################################################################
ProektForm::ProektForm(QWidget *parent)
    : QWidget(parent)
{
    QStringList sl;

    current_proekt_id = -1;
    current_zakaz_id = -1;
    current_izdelie_id = -1;
    current_sostav_id = -1;

    setupUi(this);
    pfsf = new ProektFormSostavFindWidget(this);

    QPalette pal = palette();
    pal.setColor(QPalette::Inactive, QPalette::Highlight,
                 pal.color(QPalette::Active, QPalette::Highlight));
    pal.setColor(QPalette::Inactive, QPalette::HighlightedText,
                 pal.color(QPalette::Active, QPalette::HighlightedText));
    tw_proekt->setPalette(pal);
    tw_zakaz->setPalette(pal);
    tw_izdelie->setPalette(pal);
    tw_sostav->setPalette(pal);
    tw_kompl->setPalette(pal);

    sl << "Название";
    tw_proekt->setColumnCount(1);
    tw_proekt->verticalHeader()->setDefaultSectionSize(font().pointSize() + 13);
    tw_proekt->verticalHeader()->hide();
    tw_proekt->setHorizontalHeaderLabels(sl);
    tw_proekt->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw_proekt->setSelectionMode(QAbstractItemView::SingleSelection);
    tw_proekt->horizontalHeader()->setSectionResizeMode(0,
                                                        QHeaderView::Stretch);
    tw_proekt->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    //------------------------------------------------------------------------------
    sl.clear();
    sl << "Название"
       << "Дата"
       << "Пользователь";
    tw_zakaz->setColumnCount(1);
    tw_zakaz->verticalHeader()->setDefaultSectionSize(font().pointSize() + 13);
    tw_zakaz->verticalHeader()->hide();
    tw_zakaz->setHorizontalHeaderLabels(sl);
    tw_zakaz->horizontalHeader()->resizeSection(0, 150);
    tw_zakaz->horizontalHeader()->resizeSection(1, 70);
    tw_zakaz->horizontalHeader()->resizeSection(2, 50);
    tw_zakaz->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw_zakaz->setSelectionMode(QAbstractItemView::SingleSelection);
    tw_zakaz->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tw_zakaz->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    //------------------------------------------------------------------------------
    sl.clear();
    sl << "Название";
    tw_izdelie->setColumnCount(1);
    tw_izdelie->verticalHeader()->setDefaultSectionSize(font().pointSize()
                                                        + 13);
    tw_izdelie->verticalHeader()->hide();
    tw_izdelie->setHorizontalHeaderLabels(sl);
    tw_izdelie->horizontalHeader()->resizeSection(0, 150);
    tw_izdelie->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw_izdelie->setSelectionMode(QAbstractItemView::SingleSelection);
    tw_izdelie->horizontalHeader()->setSectionResizeMode(0,
                                                         QHeaderView::Stretch);
    tw_izdelie->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    //------------------------------------------------------------------------------
    sl.clear();
    sl << "N"
       << "Название"
       << "Кол-во"
       << "Пр."
       << "Статус"
       << "Монтаж"
       << "Дата"
       << "Цена\nкомпл."
       << "Цена\nплата"
       << "Цена\nмонтаж";
    tw_sostav->setColumnCount(10);
    tw_sostav->verticalHeader()->setDefaultSectionSize(font().pointSize() + 13);
    tw_sostav->verticalHeader()->hide();
    tw_sostav->setHorizontalHeaderLabels(sl);
    tw_sostav->horizontalHeader()->resizeSection(0, 30);
    tw_sostav->horizontalHeader()->resizeSection(1, 150);
    tw_sostav->horizontalHeader()->resizeSection(2, 50);
    tw_sostav->horizontalHeader()->resizeSection(3, 40);
    tw_sostav->horizontalHeader()->resizeSection(4, 100);
    tw_sostav->horizontalHeader()->resizeSection(5, 100);
    tw_sostav->horizontalHeader()->resizeSection(6, 70);
    tw_sostav->horizontalHeader()->resizeSection(7, 70);
    tw_sostav->horizontalHeader()->resizeSection(8, 70);
    tw_sostav->horizontalHeader()->resizeSection(9, 70);
    tw_sostav->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw_sostav->setSelectionMode(QAbstractItemView::SingleSelection);
    // tw_sostav->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
    tw_sostav->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    //------------------------------------------------------------------------------
    sl.clear();
    sl << "Наименование"
       << "Номинал"
       << "Требуется"
       << "Снято"
       << "В наличии"
       << "Статус"
       << "Позиция";
    tw_kompl->setColumnCount(7);
    tw_kompl->verticalHeader()->setDefaultSectionSize(font().pointSize() + 13);
    tw_kompl->verticalHeader()->hide();
    tw_kompl->setHorizontalHeaderLabels(sl);
    tw_kompl->horizontalHeader()->resizeSection(0, 150);
    tw_kompl->horizontalHeader()->resizeSection(1, 100);
    tw_kompl->horizontalHeader()->resizeSection(2, 70);
    tw_kompl->horizontalHeader()->resizeSection(3, 70);
    tw_kompl->horizontalHeader()->resizeSection(4, 70);
    tw_kompl->horizontalHeader()->resizeSection(5, 70);
    tw_kompl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tw_kompl->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw_kompl->setSelectionMode(QAbstractItemView::NoSelection);
    // tw_kompl->horizontalHeader()->setResizeMode(6,QHeaderView::Stretch);
    tw_kompl->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    //------------------------------------------------------------------------------
    splitter->setStretchFactor(0, 8);
    splitter->setStretchFactor(1, 12);
    splitter->setStretchFactor(2, 10);
    splitter->setStretchFactor(3, 30);
    splitter->setStretchFactor(4, 10);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setCollapsible(2, false);
    splitter->setCollapsible(3, false);
    splitter->setCollapsible(4, false);
    //------------------------------------------------------------------------------
    pb_zakaz_add->setEnabled(false);
    pb_izdelie_add->setEnabled(false);
    pb_sostav_add->setEnabled(false);
    pb_kompl_price->setEnabled(false);
    pb_kompl_export->setEnabled(false);
    //------------------------------------------------------------------------------
    proekt_refresh();
}

//=======================================================================================
// УДАЛЕНИЕ
//=======================================================================================
void ProektForm::delete_item()
{
    QSqlQuery query;
    QString str;
    // int id;

    if (tw_proekt->hasFocus())
    { //================== УДАЛЕНИЕ ПРОЕКТ
      //========================================================
        query.prepare(" SELECT name FROM proekt WHERE id = ? ");
        query.addBindValue(current_proekt_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        while (query.next())
        {
            str = query.value(0).toString();
        }
        str = "Удалить проект\n\n" + str + " ?";
        query.prepare(" SELECT id FROM zakaz WHERE proekt = ? ");
        query.addBindValue(current_proekt_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        if (query.size())
        {
            QMessageBox::critical(this, app_header,
                                  "Нельзя удалить проект:\n проект не пустой.");
            return;
        }
        QMessageBox mb(app_header, str, QMessageBox::Warning, QMessageBox::Yes,
                       QMessageBox::No | QMessageBox::Default
                           | QMessageBox::Escape,
                       QMessageBox::NoButton);
        mb.setButtonText(QMessageBox::Yes, tr("Удалить"));
        mb.setButtonText(QMessageBox::No, tr("Отмена"));
        if (mb.exec() == QMessageBox::Yes)
        {
            query.prepare(" DELETE FROM proekt WHERE proekt.id = ? ");
            query.addBindValue(current_proekt_id);
            if (!query.exec())
            {
                sql_error_message(query, this);
                return;
            }
            proekt_refresh();
        }
    } else if (tw_zakaz->hasFocus())
    { //================== УДАЛЕНИЕ ЗАКАЗ
      //========================================================
        query.prepare(" SELECT name FROM zakaz WHERE id = ? ");
        query.addBindValue(current_zakaz_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        while (query.next())
        {
            str = query.value(0).toString();
        }
        str = "Удалить заказ\n\n" + str + " ?";
        query.prepare(" SELECT id FROM izdelie WHERE zakaz = ? ");
        query.addBindValue(current_zakaz_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        if (query.size())
        {
            QMessageBox::critical(
                this, app_header,
                tr("Нельзя удалить заказ:\n заказ не пустой."));
            return;
        }
        QMessageBox mb(app_header, str, QMessageBox::Warning, QMessageBox::Yes,
                       QMessageBox::No | QMessageBox::Default
                           | QMessageBox::Escape,
                       QMessageBox::NoButton);
        mb.setButtonText(QMessageBox::Yes, tr("Удалить"));
        mb.setButtonText(QMessageBox::No, tr("Отмена"));
        if (mb.exec() == QMessageBox::Yes)
        {
            query.prepare(" DELETE FROM zakaz WHERE zakaz.id = ? ");
            query.addBindValue(current_zakaz_id);
            if (!query.exec())
            {
                sql_error_message(query, this);
                return;
            }
            zakaz_refresh();
        }
    } else if (tw_izdelie->hasFocus())
    { //================== УДАЛЕНИЕ ИЗДЕЛИЯ
      //========================================================
        query.prepare(" SELECT name FROM izdelie WHERE id = ? ");
        query.addBindValue(current_izdelie_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        while (query.next())
        {
            str = query.value(0).toString();
        }
        str = "Удалить изделие\n\n" + str + " ?";
        query.prepare(" SELECT id FROM sostav WHERE izdelie = ? ");
        query.addBindValue(current_izdelie_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        if (query.size())
        {
            QMessageBox::critical(
                this, app_header,
                tr("Нельзя удалить изделие:\n изделие не пусто."));
            return;
        }
        QMessageBox mb(app_header, str, QMessageBox::Warning, QMessageBox::Yes,
                       QMessageBox::No | QMessageBox::Default
                           | QMessageBox::Escape,
                       QMessageBox::NoButton);
        mb.setButtonText(QMessageBox::Yes, tr("Удалить"));
        mb.setButtonText(QMessageBox::No, tr("Отмена"));
        if (mb.exec() == QMessageBox::Yes)
        {
            query.prepare(" DELETE FROM izdelie WHERE izdelie.id = ? ");
            query.addBindValue(current_izdelie_id);
            if (!query.exec())
            {
                sql_error_message(query, this);
                return;
            }
            izdelie_refresh();
        }
    } else if (tw_sostav->hasFocus())
    { //================== УДАЛЕНИЕ СОСТАВ
      //========================================================
        query.prepare(" SELECT name FROM sostav WHERE id = ? ");
        query.addBindValue(current_sostav_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        while (query.next())
        {
            str = query.value(0).toString();
        }
        str = "Удалить спецификацию\n\n" + str + " ?";
        query.prepare(" SELECT SUM(snato) FROM kompl WHERE sostav = ? ");
        query.addBindValue(current_sostav_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        while (query.next())
        {
            if (query.value(0).toInt() > 0)
            {
                QMessageBox::critical(this, app_header,
                                      "Нельзя удалить спецификацию:\nесть "
                                      "снятые со склада компоненты.");
                return;
            }
        }

        QMessageBox mb(app_header, str, QMessageBox::Warning, QMessageBox::Yes,
                       QMessageBox::No | QMessageBox::Default
                           | QMessageBox::Escape,
                       QMessageBox::NoButton);
        mb.setButtonText(QMessageBox::Yes, tr("Удалить"));
        mb.setButtonText(QMessageBox::No, tr("Отмена"));
        if (mb.exec() == QMessageBox::Yes)
        {
            query.prepare(" DELETE sostav,kompl,zamena "
                          " FROM sostav "
                          " LEFT JOIN kompl  ON kompl.sostav=sostav.id "
                          " LEFT JOIN zamena on zamena.kompl=kompl.id "
                          " WHERE sostav.id = ? ");
            query.addBindValue(current_sostav_id);
            if (!query.exec())
            {
                sql_error_message(query, this);
                return;
            }
            sostav_refresh();
            emit pereraschet_needed();
        }
    }
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::proekt_refresh()
{
    int i;
    QTableWidgetItem *titem;
    QSqlQuery query;
    int a = current_proekt_id;

    query.prepare(" SELECT id,name FROM proekt ");
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    tw_proekt->clearSelection();
    tw_proekt->clearContents();
    tw_proekt->setRowCount(query.size());
    i = 0;
    while (query.next())
    {
        titem = new QTableWidgetItem(query.value(1).toString());
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setData(Qt::UserRole, query.value(0).toString());
        tw_proekt->setItem(i, 0, titem);
        if (query.value(0).toInt() == a) { tw_proekt->selectRow(i); }
        i++;
    }
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::on_tw_proekt_itemSelectionChanged()
{
    QList<QTableWidgetSelectionRange> sr = tw_proekt->selectedRanges();
    if (sr.count())
    {
        current_proekt_id
            = tw_proekt->item(sr.at(0).topRow(), 0)->data(Qt::UserRole).toInt();
    } else
    {
        current_proekt_id = -1;
    }
    zakaz_refresh();
    tw_zakaz->selectRow(0);
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::on_tw_proekt_itemDoubleClicked(QTableWidgetItem *titem)
{
    Q_UNUSED(titem);

    QSqlQuery query;
    QString str;
    bool ok;

    query.prepare(" SELECT name FROM proekt WHERE id = ? ");
    query.addBindValue(current_proekt_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    while (query.next())
    {
        str = query.value(0).toString();
    }

    str = QInputDialog::getText(this, "Переименование",
                                "Новое название проекта:", QLineEdit::Normal,
                                str, &ok);
    if ((!ok) || (str.isEmpty())) return;
    query.prepare(" UPDATE proekt SET name = ? WHERE id = ? ");
    query.addBindValue(str);
    query.addBindValue(current_proekt_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    proekt_refresh();
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::zakaz_refresh()
{
    int i;
    QTableWidgetItem *titem;
    QSqlQuery query;
    int a = current_zakaz_id;

    if (current_proekt_id < 0)
    {
        tw_zakaz->setRowCount(0);
        pb_zakaz_add->setEnabled(false);
        return;
    }
    pb_zakaz_add->setEnabled(true);

    query.prepare(" SELECT id,name,date FROM zakaz WHERE proekt = ? ");
    query.addBindValue(current_proekt_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    tw_zakaz->clearSelection();
    tw_zakaz->clearContents();
    tw_zakaz->setRowCount(query.size());
    i = 0;
    while (query.next())
    {
        titem = new QTableWidgetItem(query.value(1).toString());
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setData(Qt::UserRole, query.value(0).toString());
        tw_zakaz->setItem(i, 0, titem);
        titem = new QTableWidgetItem(query.value(2).toString());
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        tw_zakaz->setItem(i, 1, titem);
        if (query.value(0).toInt() == a) { tw_zakaz->selectRow(i); }
        i++;
    }
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::on_tw_zakaz_itemSelectionChanged()
{
    QList<QTableWidgetSelectionRange> sr = tw_zakaz->selectedRanges();
    if (sr.count())
    {
        current_zakaz_id
            = tw_zakaz->item(sr.at(0).topRow(), 0)->data(Qt::UserRole).toInt();
    } else
    {
        current_zakaz_id = -1;
    }
    izdelie_refresh();
    tw_izdelie->selectRow(0);
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::on_tw_zakaz_itemDoubleClicked(QTableWidgetItem *titem)
{
    Q_UNUSED(titem);

    QSqlQuery query;
    QString str;
    bool ok;

    query.prepare(" SELECT name FROM zakaz WHERE id = ? ");
    query.addBindValue(current_zakaz_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    while (query.next())
    {
        str = query.value(0).toString();
    }

    str = QInputDialog::getText(this, "Переименование",
                                "Новое название заказа:", QLineEdit::Normal,
                                str, &ok);
    if ((!ok) || (str.isEmpty())) return;
    query.prepare(" UPDATE zakaz SET name = ? WHERE id = ? ");
    query.addBindValue(str);
    query.addBindValue(current_zakaz_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    zakaz_refresh();
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::izdelie_refresh()
{
    int i;
    QTableWidgetItem *titem;
    QSqlQuery query;
    int a = current_izdelie_id;

    if (current_zakaz_id < 0)
    {
        tw_izdelie->setRowCount(0);
        pb_izdelie_add->setEnabled(false);
        return;
    }
    pb_izdelie_add->setEnabled(true);

    query.prepare(" SELECT id,name FROM izdelie WHERE zakaz = ? ");
    query.addBindValue(current_zakaz_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    tw_izdelie->clearSelection();
    tw_izdelie->clearContents();
    tw_izdelie->setRowCount(query.size());
    i = 0;
    while (query.next())
    {
        titem = new QTableWidgetItem(query.value(1).toString());
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setData(Qt::UserRole, query.value(0).toString());
        tw_izdelie->setItem(i, 0, titem);
        if (query.value(0).toInt() == a) { tw_izdelie->selectRow(i); }
        i++;
    }
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::on_tw_izdelie_itemSelectionChanged()
{
    QList<QTableWidgetSelectionRange> sr = tw_izdelie->selectedRanges();
    if (sr.count())
    {
        current_izdelie_id = tw_izdelie->item(sr.at(0).topRow(), 0)
                                 ->data(Qt::UserRole)
                                 .toInt();
    } else
    {
        current_izdelie_id = -1;
    }
    sostav_refresh();
    tw_sostav->selectRow(0);
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::on_tw_izdelie_itemDoubleClicked(QTableWidgetItem *titem)
{
    Q_UNUSED(titem);

    QSqlQuery query;
    QString str;
    bool ok;

    query.prepare(" SELECT name FROM izdelie WHERE id = ? ");
    query.addBindValue(current_izdelie_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    while (query.next())
    {
        str = query.value(0).toString();
    }

    str = QInputDialog::getText(this, "Переименование",
                                "Новое название изделия:", QLineEdit::Normal,
                                str, &ok);
    if ((!ok) || (str.isEmpty())) return;
    query.prepare(" UPDATE izdelie SET name = ? WHERE id = ? ");
    query.addBindValue(str);
    query.addBindValue(current_izdelie_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    izdelie_refresh();
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::sostav_refresh()
{
    int i;
    QTableWidgetItem *titem;
    QSqlQuery query;
    int a = current_sostav_id;
    QColor color;

    bool b = (current_izdelie_id < 0);
    pb_sostav_add->setEnabled(!b);
    if (b)
    {
        tw_sostav->setRowCount(0);
        return;
    }

    query.prepare(" SELECT id,name,n1,prioritet,status2,date,color,notes, "
                  "cost_kompl, cost_plata, cost_montag "
                  " FROM sostav WHERE izdelie = ? ");
    query.addBindValue(current_izdelie_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    tw_sostav->clearSelection();
    tw_sostav->clearContents();
    tw_sostav->setRowCount(query.size());
    i = 0;
    while (query.next())
    {
        color = int2color(query.value(6).toInt());
        titem = new QTableWidgetItem(query.value(0).toString());
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
        titem->setData(Qt::UserRole, query.value(0).toString());
        titem->setBackground(color);
        tw_sostav->setItem(i, 0, titem);
        titem = new QTableWidgetItem(query.value(1).toString());
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setBackground(color);
        tw_sostav->setItem(i, 1, titem);
        titem = new QTableWidgetItem(query.value(2).toString());
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
        titem->setBackground(color);
        tw_sostav->setItem(i, 2, titem);
        titem = new QTableWidgetItem;
        titem->setTextAlignment(Qt::AlignCenter);
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        if (!query.value(3).isNull())
        {
            titem->setText(query.value(3).toString());
        }
        titem->setBackground(color);
        tw_sostav->setItem(i, 3, titem);
        titem = new QTableWidgetItem(query.value(4).toString());
        titem->setTextAlignment(Qt::AlignCenter);
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setBackground(color);
        tw_sostav->setItem(i, 4, titem);
        titem = new QTableWidgetItem(query.value(7).toString());
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setBackground(color);
        tw_sostav->setItem(i, 5, titem);
        titem = new QTableWidgetItem(
            query.value(5).toDate().toString("dd.MM.yy"));
        titem->setTextAlignment(Qt::AlignCenter);
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setBackground(color);
        tw_sostav->setItem(i, 6, titem);
        if (query.value(0).toInt() == a) { tw_sostav->selectRow(i); }
        titem = new QTableWidgetItem(query.value(8).toString());
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setBackground(color);
        titem->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
        tw_sostav->setItem(i, 7, titem);
        titem = new QTableWidgetItem(query.value(9).toString());
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setBackground(color);
        titem->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
        tw_sostav->setItem(i, 8, titem);
        titem = new QTableWidgetItem(query.value(10).toString());
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setBackground(color);
        titem->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
        tw_sostav->setItem(i, 9, titem);
        i++;
    }
}
//=======================================================================================
//
//=======================================================================================
void ProektForm::on_tw_sostav_itemSelectionChanged()
{
    QList<QTableWidgetSelectionRange> sr = tw_sostav->selectedRanges();
    if (sr.count())
    {
        current_sostav_id
            = tw_sostav->item(sr.at(0).topRow(), 0)->data(Qt::UserRole).toInt();
    } else
    {
        current_sostav_id = -1;
    }
    if (current_sostav_id > 0)
    {
        le_sostav->setText(QString::number(current_sostav_id));
    } else
    {
        le_sostav->setText("");
    }
    kompl_refresh();
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::on_tw_sostav_cellDoubleClicked(int row, int column)
{
    QSqlQuery query;
    QString str;
    bool ok;
    int i;

    switch (column)
    {
    case (1): // название
        query.prepare(" SELECT name FROM sostav WHERE id = ? ");
        query.addBindValue(current_sostav_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        while (query.next())
        {
            str = query.value(0).toString();
        }

        str = QInputDialog::getText(
            this, "Переименование",
            "Новое название спецификации:", QLineEdit::Normal, str, &ok);
        if ((!ok) || (str.isEmpty())) return;
        query.prepare(" UPDATE sostav SET name = ? WHERE id = ? ");
        query.addBindValue(str);
        query.addBindValue(current_sostav_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        break;
    case (2): // количество
        query.prepare(" SELECT status2,n1 FROM sostav WHERE id = ? ");
        query.addBindValue(current_sostav_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        if (query.size() != 1)
        {
            QMessageBox::critical(this, app_header,
                                  "Ошибка выборки из базы данных.");
            return;
        }
        query.next();
        str = query.value(0).toString();
        if (str.toLower() != "не утвержден")
        {
            QMessageBox::critical(
                this, app_header,
                "Изменять количество можно только\nв спецификациях со статусом "
                "\"Не Утвержден\".");
            return;
        }
        i = query.value(1).toInt();
        i = QInputDialog::getInt(this, app_header, "Количество:", i, 1, 1000, 1,
                                 &ok);
        if (!ok) return;
        query.prepare(" UPDATE sostav SET n1 = ? WHERE id = ? ");
        query.addBindValue(i);
        query.addBindValue(current_sostav_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        emit pereraschet_needed();
        break;
    case (3): // приоритет
        query.prepare(" SELECT prioritet FROM sostav WHERE id = ? ");
        query.addBindValue(current_sostav_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        while (query.next())
        {
            str = query.value(0).toString();
        }
        str = QInputDialog::getText(this, "Приоритет",
                                    "Приоритет:", QLineEdit::Normal, str, &ok);
        i = str.toInt(&ok);
        query.prepare(" UPDATE sostav SET prioritet = ? WHERE id = ? ");
        if (i > 0)
        {
            query.addBindValue(i);
        } else
        {
            query.addBindValue(QVariant());
        }
        query.addBindValue(current_sostav_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        break;
    case (4): // статус
    {
        QMenu menu;
        QMap<QAction *, int> map;
        if (tw_sostav->item(row, 4)) str = tw_sostav->item(row, 4)->text();
        for (i = 0; i < sostav_status.count(); i++)
        {
            QAction *act = menu.addAction(sostav_status.at(i));
            if (act->text() == str)
            {
                act->setCheckable(true);
                act->setChecked(true);
            }
            map[act] = i;
        }
        QAction *act = menu.exec(QCursor::pos());
        if (map.contains(act))
        {
            str = sostav_status.value(map[act]);
            QSqlQuery query;
            query.prepare("UPDATE sostav SET status2 = ? WHERE id = ?");
            query.addBindValue(str);
            query.addBindValue(current_sostav_id);
            if (!query.exec())
            {
                sql_error_message(query, this);
                return;
            }
        }
    }
    break;
    case (8): // цена плата
        query.prepare(" SELECT cost_plata FROM sostav WHERE id = ? ");
        query.addBindValue(current_sostav_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        while (query.next())
        {
            str = query.value(0).toString();
        }
        str = QInputDialog::getText(this, app_header,
                                    "Цена платы:", QLineEdit::Normal, str, &ok);
        query.prepare(" UPDATE sostav SET cost_plata = ? WHERE id = ? ");
        if (!str.isEmpty())
        {
            query.addBindValue(str);
        } else
        {
            query.addBindValue(QVariant());
        }
        query.addBindValue(current_sostav_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        break;
    case (9): // цена монтаж
        query.prepare(" SELECT cost_montag FROM sostav WHERE id = ? ");
        query.addBindValue(current_sostav_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        while (query.next())
        {
            str = query.value(0).toString();
        }
        str = QInputDialog::getText(
            this, app_header, "Цена монтажа:", QLineEdit::Normal, str, &ok);
        query.prepare(" UPDATE sostav SET cost_montag = ? WHERE id = ? ");
        if (!str.isEmpty())
        {
            query.addBindValue(str);
        } else
        {
            query.addBindValue(QVariant());
        }
        query.addBindValue(current_sostav_id);
        if (!query.exec())
        {
            sql_error_message(query, this);
            return;
        }
        break;
    }
    sostav_refresh();
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::kompl_refresh()
{
    int i, j, t;
    QTableWidgetItem *titem;
    QSqlQuery query;
    bool is_zamena;
    QColor color;

    bool b = (current_sostav_id < 0);
    pb_kompl_price->setEnabled(!b);
    pb_kompl_export->setEnabled(!b);
    if (b)
    {
        tw_kompl->setRowCount(0);
        return;
    }

    QFont fnt = font();
    fnt.setBold(true);

    query.prepare(
        " SELECT kompl.id, kompl.color, kompl.type, typename, kompl.name, "
        "kompl.need, "
        "        kompl.snato, zamena.name, zamena.id, prihod2.sostatok, "
        "        zamena.snato, kompl.nominal, kompl.status, kompl.position"
        " FROM kompl "
        "    LEFT JOIN types  ON kompl.type   = types.id     "
        "    LEFT JOIN zamena ON kompl.id     = zamena.kompl "
        "    LEFT JOIN  ( SELECT name,sum(ostatok) AS sostatok  "
        "                   FROM prihod GROUP BY name    ) "
        "         AS prihod2  ON zamena.name  = prihod2.name "
        " WHERE sostav = ? "
        " ORDER BY typename, kompl.name, zamena.id ");

    query.addBindValue(current_sostav_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    tw_kompl->clearContents();
    tw_kompl->setRowCount(1);
    i = 0;
    t = -1;
    while (query.next())
    { // тип
        j = query.value(2).toInt();
        if (j != t)
        { // новый тип
            t = j;
            tw_kompl->setRowCount(i + 1);
            titem = new QTableWidgetItem(query.value(3).toString());
            titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            titem->setBackground(QColor {"gray"});
            titem->setFont(fnt);
            titem->setData(Qt::UserRole, j);
            tw_kompl->setItem(i, 0, titem);
            tw_kompl->setSpan(i, 0, 1, 7);
            i++;
        }
        tw_kompl->setRowCount(i + 1);
        // наименование
        is_zamena = (query.value(4) != query.value(7));
        color = int2color(query.value(1).toInt());
        if (is_zamena) // замена
        {
            titem = new QTableWidgetItem("  ЗАМЕНА: "
                                         + query.value(7).toString());
        } else
        {
            titem = new QTableWidgetItem(query.value(4).toString());
        }
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setBackground(color);
        tw_kompl->setItem(i, 0, titem);
        tw_kompl->setSpan(i, 0, 0, 0);
        // номинал
        titem = new QTableWidgetItem(query.value(11).toString());
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setBackground(color);
        tw_kompl->setItem(i, 1, titem);
        // требуется
        if (is_zamena) // замена
        {
            titem = new QTableWidgetItem;
        } else
        {
            titem = new QTableWidgetItem(query.value(5).toString());
        }
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setBackground(color);
        tw_kompl->setItem(i, 2, titem);
        // снято
        titem = new QTableWidgetItem(query.value(10).toString());
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setBackground(color);
        tw_kompl->setItem(i, 3, titem);
        // наличие
        titem = new QTableWidgetItem(query.value(9).toString());
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setBackground(color);
        tw_kompl->setItem(i, 4, titem);
        // статус
        titem
            = new QTableWidgetItem(kompl_status.value(query.value(12).toInt()));
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setBackground(color);
        tw_kompl->setItem(i, 5, titem);
        // позиция
        if (is_zamena) // замена
        {
            titem = new QTableWidgetItem;
        } else
        {
            titem = new QTableWidgetItem(query.value(13).toString());
        }
        titem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        titem->setBackground(color);
        tw_kompl->setItem(i, 6, titem);

        i++;
    }
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::on_pb_proekt_add_clicked()
{
    QSqlQuery query;
    QString str;
    bool ok;

    str = QInputDialog::getText(this, "Добавить", tr("Название проекта:"),
                                QLineEdit::Normal, "", &ok);
    if ((!ok) || str.isEmpty()) return;
    query.prepare(" INSERT INTO proekt ( name ) VALUES ( ? ) ");
    query.addBindValue(str.toUtf8());
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    proekt_refresh();
}
//=======================================================================================
//
//=======================================================================================
void ProektForm::on_pb_zakaz_add_clicked()
{
    QSqlQuery query;
    QString str;
    bool ok;

    str = QInputDialog::getText(this, "Добавить",
                                "Название заказа:", QLineEdit::Normal, "", &ok);
    if ((!ok) || str.isEmpty()) return;
    query.prepare(
        " INSERT INTO zakaz ( proekt,name,date,user ) VALUES ( ?,?,?,? ) ");
    query.addBindValue(current_proekt_id);
    query.addBindValue(str);
    query.addBindValue(QDate::currentDate());
    query.addBindValue(user_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    zakaz_refresh();
}
//=======================================================================================
//
//=======================================================================================
void ProektForm::on_pb_izdelie_add_clicked()
{
    QSqlQuery query;
    QString str;
    bool ok;

    str = QInputDialog::getText(
        this, "Добавить", "Название изделия:", QLineEdit::Normal, "", &ok);
    if ((!ok) || str.isEmpty()) return;
    query.prepare(" INSERT INTO izdelie ( zakaz,name ) VALUES ( ?,? ) ");
    query.addBindValue(current_zakaz_id);
    query.addBindValue(str);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    izdelie_refresh();
}
//=======================================================================================
//
//=======================================================================================
void ProektForm::on_pb_sostav_add_clicked()
{
    SostavAddDialog dialog(this, current_izdelie_id);
    if (dialog.exec() == QDialog::Accepted) { emit pereraschet_needed(); }
    sostav_refresh();
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::on_pb_sostav_export_to_file_clicked()
{
    QString str;
    QString fn;
    static QString fileName;

    int nr = tw_sostav->rowCount();
    int nc = tw_sostav->columnCount();
    int i, j;

    for (i = 0; i < nc; i++)
    {
        str += tw_sostav->horizontalHeaderItem(i)->text().simplified() + ";";
    }
    str.chop(1);
    str += "\n";

    for (j = 0; j < nr; j++)
    {
        for (i = 0; i < nc; i++)
        {
            QTableWidgetItem *titem = tw_sostav->item(j, i);
            if (titem)
                str += titem->text().simplified() + ";";
            else
                str += ";";
        }
        str.chop(1);
        str += "\n";
    }

    fn = QFileDialog::getSaveFileName(this, "Экспорт в текстовый файл",
                                      fileName, "Text Files (*.txt)");

    if (fn.isEmpty()) return;
    fileName = fn;

    QFile file(fileName);
    bool ok = file.open(QIODevice::WriteOnly | QIODevice::Text
                        | QIODevice::Truncate);
    if (!ok) return;
    file.write(str.toLocal8Bit());
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::on_pb_kompl_price_clicked()
{
    PriceDialog *dialog = new PriceDialog(0, current_sostav_id);
    connect(dialog, SIGNAL(updated()), this, SLOT(sostav_refresh()));
    dialog->show();
}
//=======================================================================================
//
//=======================================================================================
void ProektForm::on_pb_kompl_export_clicked()
{
    QString str;
    int i, j;

    QSqlQuery query;
    query.prepare(
        " SELECT proekt.name, zakaz.name ,izdelie.name ,sostav.name, sostav.n1 "
        " FROM sostav "
        " LEFT JOIN izdelie ON izdelie.id = sostav.izdelie "
        " LEFT JOIN zakaz   ON zakaz.id = izdelie.zakaz "
        " LEFT JOIN proekt  ON proekt.id = zakaz.proekt "
        " WHERE sostav.id = ? ");
    query.addBindValue(current_sostav_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    query.next();

    str = "Комплектация N=" + QString::number(current_sostav_id) + "\r\n";
    str += "Спецификация: " + sql_get_string(query, 3) + " ( "
         + query.value(4).toString() + " шт.)\r\n";
    str += "Изделие: " + sql_get_string(query, 2) + "\r\n";
    str += "Заказ: " + sql_get_string(query, 1) + "\r\n";
    str += "Проект: " + sql_get_string(query, 0) + "\r\n";
    str += "\r\n";

    str += "Наименование\tНоминал\tТребуется\tСнято\tВ "
           "наличии\tСтатус\tПозиция\r\n";

    for (i = 0; i < tw_kompl->rowCount(); i++)
    {
        for (j = 0; j < tw_kompl->columnCount(); j++)
        {
            if (tw_kompl->item(i, j))
                str += "\"" + tw_kompl->item(i, j)->text() + "\"\t";
        }
        str.chop(1);
        str += "\r\n";
    }
    str.chop(1);

    QApplication::clipboard()->setText(str);

    QMessageBox::information(this, app_header,
                             "Комплектация скопирована в буфер обмена.");
}
//=======================================================================================
//
//=======================================================================================
void ProektForm::on_le_sostav_textChanged(const QString string)
{
    bool ok;
    int id = string.toInt(&ok);
    if ((!ok) || (id <= 0)) id = -1;
    if (id == current_sostav_id) return;
    QSqlQuery query;
    query.prepare("SELECT izdelie FROM sostav WHERE id = ? ");
    query.addBindValue(string.toInt());
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    if (query.size())
    {
        current_sostav_id = id;
        query.next();
        current_izdelie_id = query.value(0).toInt();
    } else
    {
        current_sostav_id = -1;
        current_izdelie_id = -1;
    }

    query.prepare("SELECT zakaz FROM izdelie WHERE id = ? ");
    query.addBindValue(current_izdelie_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    if (query.size())
    {
        query.next();
        current_zakaz_id = query.value(0).toInt();
    } else
    {
        current_zakaz_id = -1;
    }

    query.prepare("SELECT proekt FROM zakaz WHERE id = ? ");
    query.addBindValue(current_zakaz_id);
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    if (query.size())
    {
        query.next();
        current_proekt_id = query.value(0).toInt();
    } else
    {
        current_proekt_id = -1;
    }

    refresh();
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::on_pb_sostav_find_clicked()
{
    pfsf->move(QCursor::pos() + QPoint(10, 10));
    pfsf->show();
    pfsf->activateWindow();
    pfsf->le->clear();
    pfsf->tw->clearSelection();
    pfsf->le->setFocus();
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::refresh()
{
    tw_proekt->blockSignals(true);
    tw_zakaz->blockSignals(true);
    tw_izdelie->blockSignals(true);
    tw_sostav->blockSignals(true);
    proekt_refresh();
    zakaz_refresh();
    izdelie_refresh();
    sostav_refresh();
    kompl_refresh();
    tw_proekt->blockSignals(false);
    tw_zakaz->blockSignals(false);
    tw_izdelie->blockSignals(false);
    tw_sostav->blockSignals(false);
}

//=======================================================================================
//
//=======================================================================================
void ProektForm::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    pfsf->le->clear();
    pfsf->tw->clearSelection();
    pfsf->hide();
}

//#######################################################################################
//
//#######################################################################################
ProektFormSostavFindWidget::ProektFormSostavFindWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    setWindowTitle("Поиск спецификации");
    setWindowFlags(Qt::Tool);
    connect(le, SIGNAL(textChanged(const QString)), this, SLOT(refresh()));

    tw->setColumnCount(1);
    tw->verticalHeader()->setDefaultSectionSize(font().pointSize() + 13);
    tw->verticalHeader()->hide();
    tw->setHorizontalHeaderLabels(QStringList() << "Название спецификации");
    tw->horizontalHeader()->setSectionsClickable(false);
    tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tw->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw->setSelectionMode(QAbstractItemView::SingleSelection);
    // tw->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
    tw->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    refresh();
}

//=======================================================================================
//
//=======================================================================================
void ProektFormSostavFindWidget::refresh()
{
    QTableWidgetItem *titem;
    QSqlQuery query;
    int i, a;

    query.prepare(
        "SELECT id,name FROM sostav WHERE name LIKE ? ORDER BY name ASC");
    query.addBindValue("%" + le->text().replace(' ', '%') + "%");
    if (!query.exec())
    {
        sql_error_message(query, this);
        return;
    }
    a = 0;
    QList<QTableWidgetSelectionRange> sr = tw->selectedRanges();
    if (sr.count())
        a = tw->item(sr.at(0).topRow(), 0)->data(Qt::UserRole).toInt();
    tw->clearSelection();
    tw->clearContents();
    tw->setRowCount(query.size());
    i = 0;
    while (query.next())
    {
        titem = new QTableWidgetItem(query.value(1).toString());
        titem->setData(Qt::UserRole, query.value(0).toString());
        tw->setItem(i, 0, titem);
        if (query.value(0).toInt() == a) { tw->selectRow(i); }
        i++;
    }
}

//=======================================================================================
//
//=======================================================================================
void ProektFormSostavFindWidget::on_tw_itemSelectionChanged()
{
    int id = 0;
    QList<QTableWidgetSelectionRange> sr = tw->selectedRanges();
    if (sr.count())
        id = tw->item(sr.at(0).topRow(), 0)->data(Qt::UserRole).toInt();
    if (id == 0) return;

    ProektForm *pf = qobject_cast<ProektForm *>(parentWidget());
    if (pf == 0) return;

    pf->le_sostav->setText(QString::number(id));
}
