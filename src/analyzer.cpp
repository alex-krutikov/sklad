#include "analyzer.h"

#include "main.h"

#include <QSqlQuery>

//==============================================================================
//
//==============================================================================
int AnalyzerModel::rowCount(const QModelIndex &) const
{
    return names.size();
}

//==============================================================================
//
//==============================================================================
int AnalyzerModel::columnCount(const QModelIndex &) const
{
    return ddd.size() + 1;
}

//==============================================================================
//
//==============================================================================
QVariant AnalyzerModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    const int column = index.column();

    switch (role)
    {
    case Qt::DisplayRole:
        if (column == 0) return names[row];
        return ddd[column - 1][row];

    case Qt::TextAlignmentRole:
        if (column == 0) return (int)(Qt::AlignLeft | Qt::AlignVCenter);
        return Qt::AlignCenter;
    }

    return QVariant{};
}

//==============================================================================
//
//==============================================================================
QVariant AnalyzerModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
    if (orientation != Qt::Horizontal) return QVariant{};

    switch (role)
    {
    case Qt::DisplayRole:
        return columnNames.value(section);
    }

    return QVariant{};
}

//==============================================================================
//
//==============================================================================
void AnalyzerModel::appendRow(const QString &name, const Items &d)
{
    columnNames << name;

    QStringList sl;
    sl.reserve(names.size());
    for (int i = 0; i < names.size(); ++i)
        sl << QString{};
    ddd.push_back(sl);

    foreach (auto const &e, d)
    {
        auto it = std::lower_bound(std::begin(names), std::end(names), e.name);
        int pos = it - names.begin();
        if ((it == names.end()) || (*it != e.name))
        {
            names.insert(pos, e.name);
            for (int i = 0; i < ddd.size(); ++i)
                ddd[i].insert(pos, QString{});
        }
        ddd[ddd.size() - 1][pos] = QString::number(e.count);
    }
}

//==============================================================================
//
//==============================================================================
AnalyzerModel::Items AnalyzerModel::ItemsDpPrihod::data(int type,
                                                        const QDate &d1,
                                                        const QDate &d2,
                                                        bool &ok)
{
    AnalyzerModel::Items ret;
    ok = true;

    QSqlQuery query;
    query.prepare("SELECT `name`, SUM(`kolichestvo`) "
                  "FROM `prihod` "
                  "WHERE `type` = ? AND `date` >= ? AND `date` < ? "
                  "GROUP BY `name`");
    query.addBindValue(type);
    query.addBindValue(d1);
    query.addBindValue(d2);
    if (!query.exec())
    {
        sql_error_message(query);
        ok = false;
        return ret;
    }

    while (query.next())
    {
        ret.push_back(Item{query.value(0).toString(), query.value(1).toUInt()});
    }

    return ret;
}

//==============================================================================
//
//==============================================================================
QString AnalyzerModel::ItemsDpPrihod::title()
{
    return "Приход";
}

//==============================================================================
//
//==============================================================================
AnalyzerModel::Items AnalyzerModel::ItemsDpRashod::data(int type,
                                                        const QDate &d1,
                                                        const QDate &d2,
                                                        bool &ok)
{
    AnalyzerModel::Items ret;
    ok = true;

    QSqlQuery query;
    query.prepare(
        "SELECT `name`, SUM(`rashod`.`n`) "
        "FROM `rashod` LEFT JOIN `prihod` ON `rashod`.`prihod` = `prihod`.`id` "
        "WHERE `type` = ? AND `rashod`.`date` >= ? AND `rashod`.`date` < ? "
        "GROUP BY `name`");
    query.addBindValue(type);
    query.addBindValue(d1);
    query.addBindValue(d2);
    if (!query.exec())
    {
        sql_error_message(query);
        ok = false;
        return ret;
    }

    while (query.next())
    {
        ret.push_back(Item{query.value(0).toString(), query.value(1).toUInt()});
    }

    return ret;
}

//==============================================================================
//
//==============================================================================
QString AnalyzerModel::ItemsDpRashod::title()
{
    return "Расход";
}

//==============================================================================
//
//==============================================================================
void AnalyzerModel::load(ItemsDP *dp, int type)
{
    class ResetModelLocker
    {
    public:
        ResetModelLocker(AnalyzerModel *model)
            : model{model}
        {
            model->beginResetModel();
        }
        ~ResetModelLocker() { model->endResetModel(); }

    private:
        AnalyzerModel *model;
    };

    ResetModelLocker locker{this};

    names.clear();
    ddd.clear();
    columnNames.clear();
    columnNames << "Наименование";

    int y = QDate::currentDate().year();
    QDate d1;
    QDate d2;
    AnalyzerModel::Items v;

    for (int j = 0; j < 12; ++j)
    {
        d1 = QDate{y, 1, 1};
        d2 = QDate{y + 1, 1, 1};
        QString title = QString{"%1\n%2"}.arg(y).arg(dp->title());

        bool ok = false;
        v = dp->data(type, d1, d2, ok);
        if (!ok) { return; }
        appendRow(title, v);

        y -= 1;
    }

    ///////

    d1 = QDate{1900, 1, 1};
    d2 = QDate{y + 1, 1, 1};
    QString title = QString{"Ранее\n%1"}.arg(dp->title());

    bool ok = false;
    v = dp->data(type, d1, d2, ok);
    if (!ok) { return; }
    appendRow(title, v);

    ////////

    d1 = QDate{1900, 1, 1};
    d2 = QDate{2500, 1, 1};
    title = QString{"Всего\n%1"}.arg(dp->title());

    ok = false;
    v = dp->data(type, d1, d2, ok);
    if (!ok) { return; }
    appendRow(title, v);
}
