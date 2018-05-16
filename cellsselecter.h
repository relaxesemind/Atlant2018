#ifndef CELLSSELECTER_H
#define CELLSSELECTER_H

#include <QObject>
#include <QImage>
#include <QVector>
#include <single_area.h>

class CellsSelecter : public QObject
{
    Q_OBJECT
    CellsSelecter() = default;
    CellsSelecter(const CellsSelecter&);
    CellsSelecter& operator=(CellsSelecter&);

public:

    static CellsSelecter& sharedSelecter() {
        static CellsSelecter shared;
        return shared;
    }

    using Labels = QVector<QVector<quint64>>;
    using Labels_row = QVector<quint64>;
    using Objects = QVector<S_area>;

signals:
    void selectingIsFinished();

public slots: 
    Objects select(const QImage& img);

private:
   bool isWorking;
};

#endif // CELLSSELECTER_H
