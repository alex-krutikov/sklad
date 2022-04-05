#ifndef __PRICE_H__
#define __PRICE_H__

#include <QtGui>

//=======================================================================================
//
//=======================================================================================
#include "ui/ui_pricedialog.h"
class PriceDialog
    : public QDialog
    , public Ui::PriceDialog
{
    struct t_data
    {
        QString name;
        QString type_name;
        int need;
        double price_real;
        double price_est;
        int snato;
        bool zamena_flag;
        double sum;
        int row_index;
    };

    Q_OBJECT
public:
    PriceDialog(QWidget *parent = 0, int sostav_id = 0);
private slots:
    void tw_cellChanged(int, int);
    void on_tw_cellDoubleClicked(int, int);
    void price_settings_changed();
    void on_pb_export_clicked();
    void on_pb_ok_clicked();
signals:
    void updated();

private:
    void recalc();
    void queryPrices();
    int sostav_id;
    QVector<t_data> data;
    QMap<int, int> tw2data;

    QMap<QString, double> price;
    QMap<QString, int> price_est;
};

#endif
