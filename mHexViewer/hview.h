#ifndef HVIEW_H
#define HVIEW_H
#include <QObject>
#include <QPainter>
#include <QAbstractScrollArea>
#include <QVector>

class hview : public QAbstractScrollArea
{
    Q_OBJECT
public:
    hview();
    void getdata(QVector<quint8> qlistdata);
    void searchdata(quint32 data);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QVector<quint8> content;
    int scrollnum = 0;//当前行号
    int scrollnumrange = 0;//总行数
    short scrollnumrest = 0;//最后一页剩余行数
    int targetdata = 0;
    //行列指示框
    bool indtor = false;
    short indtorrow = 0;
    short indtorcolumn = 0;
    short indtoroffset = 0;
    bool indtoroffsetf = false;
    virtual void mousePressEvent(QMouseEvent *event);

public slots:
    void refreshView();
};

#endif // HVIEW_H
