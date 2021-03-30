#ifndef __PROEKTFORM_H__
#define __PROEKTFORM_H__

#include <QtGui>

#include "ui/ui_proektform.h"
#include "ui/ui_proektformsostavfind.h"

class ProektFormSostavFindWidget;

class ProektForm
    : public QWidget
    , public Ui::ProektForm
{
    Q_OBJECT
public:
    ProektForm(QWidget *parent = 0);
private slots:
    void proekt_refresh();
    void zakaz_refresh();
    void izdelie_refresh();
    void sostav_refresh();
    void kompl_refresh();

private:
    int current_proekt_id;
    int current_zakaz_id;
    int current_izdelie_id;
    int current_sostav_id;
    ProektFormSostavFindWidget *pfsf;
private slots:
    void on_tw_proekt_itemSelectionChanged();
    void on_tw_zakaz_itemSelectionChanged();
    void on_tw_izdelie_itemSelectionChanged();
    void on_tw_sostav_itemSelectionChanged();

    void on_tw_proekt_itemDoubleClicked(QTableWidgetItem *);
    void on_tw_zakaz_itemDoubleClicked(QTableWidgetItem *);
    void on_tw_izdelie_itemDoubleClicked(QTableWidgetItem *);
    void on_tw_sostav_cellDoubleClicked(int, int);
    void on_pb_sostav_export_to_file_clicked();

    void on_pb_proekt_add_clicked();
    void on_pb_zakaz_add_clicked();
    void on_pb_izdelie_add_clicked();
    void on_pb_sostav_add_clicked();
    void on_pb_kompl_price_clicked();
    void on_pb_kompl_export_clicked();
    void on_le_sostav_textChanged(const QString);
    void on_pb_sostav_find_clicked();
public slots:
    void delete_item();
    void refresh();
signals:
    void pereraschet_needed();

protected:
    void hideEvent(QHideEvent *event);
};

class ProektFormSostavFindWidget
    : public QWidget
    , public Ui::ProektFormSostavFindWidget
{
    Q_OBJECT
public:
    ProektFormSostavFindWidget(QWidget *parent);
public slots:
    void refresh();
    void on_tw_itemSelectionChanged();
};

#endif
