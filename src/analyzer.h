#ifndef __ANALYZER_H__
#define __ANALYZER_H__

#include <QAbstractTableModel>

//==============================================================================
//
//==============================================================================
class AnalyzerModel : public QAbstractTableModel
{
public:
    AnalyzerModel(QObject *parent)
        : QAbstractTableModel(parent)
    {
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

public:
    struct Item
    {
        QString name;
        size_t count;
    };

    using Items = QVector<Item>;

    struct ItemsDP
    {
        virtual Items data(int type, const QDate &d1, const QDate &d2, bool &ok)
            = 0;
        virtual QString title() = 0;
    };

    struct ItemsDpPrihod : public ItemsDP
    {
        Items data(int type, const QDate &d1, const QDate &d2,
                   bool &ok) override;
        virtual QString title() override;
    };

    struct ItemsDpRashod : public ItemsDP
    {
        Items data(int type, const QDate &d1, const QDate &d2,
                   bool &ok) override;
        virtual QString title() override;
    };


public:
    void load(ItemsDP *dp, int type);

private:
    void appendRow(const QString &name, const Items &d);

private:
    QStringList columnNames;
    QStringList names;
    QVector<QStringList> ddd;
};

#endif

