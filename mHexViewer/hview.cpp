#include "hview.h"
#include <QColor>
#include <QScrollBar>
#include <QMouseEvent>
#include <QMessageBox>

const int HEX_HEIGHT = 460;
const int HEX_WIDTH = 720;
const int HEX_ADDR_WIDTH = 105;
const int HEX_INIT_GAP = 5;
const int HEX_HEX_WIDTH = 410;
const int HEX_CONTENT_YTOP = 35;
const int HEX_CONTENT_YBOTTOM = 455;
const int HEX_WIN_TOP = 15;
const int HEX_BYTE_GAP = 25;
const int HEX_ASCII_GAP = 12;
const int HEX_ROW_GAP = 20;
const int HEX_ROW_NUM = 23;
const int HEX_ROW_HEIGHT = 20;
const int HEX_COLUMN_WIDTH = 25;

hview::hview() : QAbstractScrollArea()
{
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //类内使用的另一种信号槽方法
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(refreshView()));
}

void hview::getdata(QVector<quint8> qlistdata)
{
    if(qlistdata.size() == 0)
    {
        return;
    }
    indtoroffset = 0;
    indtoroffsetf = false;
    content = qlistdata;
    //一个页面为23行
    scrollnumrange = content.size() / 16 / 22;
    //检测最后一行是不是整行
    indtoroffset = content.size() % 16;
    if(indtoroffset == 0)
    {
        scrollnumrest = (content.size() / 16) % 22;
        qDebug() << "scrollnumrest is:" << scrollnumrest;
    }
    else
    {
        scrollnumrest = (content.size() / 16) % 22 + 1;
        indtoroffsetf = true;
        qDebug() << "scrollnumrest is:" << scrollnumrest;
    }
    verticalScrollBar()->setPageStep(1);
    verticalScrollBar()->setRange(0, scrollnumrange);
    verticalScrollBar()->setValue(0);
    indtor = false;
}

void hview::searchdata(quint32 data)
{
    if(content.size() == 0)
    {
        QMessageBox::information(this, "WARNING", "<center>""<p style='font-family: Courier New'>"
                                 "No file selected!"
                                 "<br>"
                                 " Please try again."
                                 "</p>""</center>");
        return;
    }
    verticalScrollBar()->setValue(0);
    indtorrow = (data / 16) % 22 + 1;
    indtorcolumn = data % 16;
    scrollnum = data / 16 / 22;
    qDebug() << "indtorrow is:" << indtorrow;
    qDebug() << "indtorcolumn is:" << indtorcolumn;
    qDebug() << "indtoroffset is:" << indtoroffset;
    qDebug() << "scrollnum is:" << scrollnum << Qt::endl;
    if((scrollnum == scrollnumrange && indtorrow > scrollnumrest) || scrollnum > scrollnumrange
            || (scrollnum == scrollnumrange && indtorrow == scrollnumrest && indtorcolumn > indtoroffset - 1 && indtoroffsetf))
    {
        indtor = false;
        verticalScrollBar()->setValue(0);
        scrollnum = 0;
        QMessageBox::information(this, "WARNING", "<center>""<p style='font-family: Courier New'>"
                                 "This address is unknow!"
                                 "<br>"
                                 " Please try again."
                                 "</p>""</center>");
        return;
    }
    verticalScrollBar()->setValue(scrollnum);
    indtor = true;//这段必须放在verticalScrollBar()的下边，否则会失效
    viewport()->update();
}

void hview::paintEvent(QPaintEvent *event)
{
    if(content.size() != 0)
    {
        //绘制纵向地址区域背景
        QPainter painter(viewport());//"viewport()" not "this"
        painter.fillRect(0, 0, HEX_ADDR_WIDTH, HEX_HEIGHT, QColor(QColorConstants::Gray));
        //绘制横向地址区域背景
        painter.fillRect(0, 0, HEX_WIDTH, HEX_ROW_HEIGHT, QColor(QColorConstants::Gray));
        painter.setFont(QFont("Consolas", 12));
        //ASCII部分
        for(int i = 0; i < 16; i++)
        {
            QString addrindexl = QString("%1").arg(i, 1, 16);
            QString addrindexu = addrindexl.toUpper();
            painter.drawText(HEX_ADDR_WIDTH + HEX_INIT_GAP + HEX_HEX_WIDTH + i * HEX_ASCII_GAP, HEX_WIN_TOP, addrindexu);
        }
        painter.drawText(0, HEX_WIN_TOP, "  OFFSET");
        //HEX部分
        for(int i = 0; i < 16; i++)
        {
            QString addrindexl = QString("%1").arg(i, 2, 16, QChar('0'));
            QString addrindexu = addrindexl.toUpper();
            painter.drawText(HEX_ADDR_WIDTH + HEX_INIT_GAP + i * HEX_BYTE_GAP, HEX_WIN_TOP, addrindexu);
        }
        //绘制HEX的ASCII分界线
        painter.setPen(QPen(Qt::black, 1));
        painter.drawLine(HEX_ADDR_WIDTH + HEX_HEX_WIDTH, 0, HEX_ADDR_WIDTH + HEX_HEX_WIDTH, HEX_HEIGHT);
        for(int j = 0 ; j < HEX_ROW_NUM; j++)
        {
            //绘制HEX地址
            QString addrl = QString("%1"":").arg((j + scrollnum) * 16, 10, 16, QChar('0'));
            QString addru = addrl.toUpper();
            painter.drawText(0, HEX_CONTENT_YTOP + HEX_ROW_GAP * j, addru);
            //绘制数据
            for(int i = 0; i < 16; i++)
            {
                if((i + (j + scrollnum) * 16) < content.size())//判断QList是否越界
                {
                    //绘制HEX数据
                    QString tmpcontentl = QString("%1").arg(content[i + (j + scrollnum) * 16], 2, 16, QChar('0'));
                    //qDebug() << "content location is:" << i + (j + scrollnum) * 16;
                    QString tmpcontentu = tmpcontentl.toUpper();
                    painter.drawText(HEX_ADDR_WIDTH + HEX_INIT_GAP + i * HEX_BYTE_GAP, j * HEX_ROW_HEIGHT + HEX_CONTENT_YTOP, tmpcontentu);
                    /**
                     * 启用以下方法后，第一次加载文件会卡顿三秒钟！
                     * 未解决！
                     */
                    //绘制ASCII数据
                    char tmpcontentascii = (char)content.at(i + (j + scrollnum) * 16);
                    painter.drawText(HEX_ADDR_WIDTH + HEX_INIT_GAP + HEX_HEX_WIDTH + i * HEX_ASCII_GAP, j * HEX_ROW_HEIGHT + HEX_CONTENT_YTOP, (QString)tmpcontentascii);
                }
            }
        }
        //绘制行列指示框
        if (indtor)
        {
            painter.setPen(QPen(Qt::red, 3));
            painter.drawRect(0, indtorrow * HEX_ROW_HEIGHT, HEX_WIDTH, HEX_ROW_HEIGHT);//使用顶点的相对位置！重要！
            painter.drawRect(indtorcolumn * HEX_COLUMN_WIDTH + HEX_ADDR_WIDTH, 0, HEX_COLUMN_WIDTH, HEX_HEIGHT);
//            qDebug() << "painted indtor!!" << Qt::endl;
        }
    }
}

void hview::refreshView()
{
    indtor = false;
    //一次鼠标滚动为23行
    scrollnum = verticalScrollBar()->value() * 22;
    viewport()->update();
}

//绘制行列指示框
void hview::mousePressEvent(QMouseEvent *event)
{
    if(content.size() != 0)
    {
        indtorrow = event->pos().ry() / HEX_ROW_HEIGHT;//先赋值，后检测，不然会出现最后一页头次点击检测不到的BUG！
        indtorcolumn = (event->pos().rx() - HEX_ADDR_WIDTH) / HEX_COLUMN_WIDTH;
        /**
         * 以下判断条件
         * 1.x轴大于HEX区域
         * 2.x轴小于HEX区域
         * 3.y轴高于HEX区域
         * 4.最后一页超出数据区域
         * 5.最后一行不完整的情况下，超出HEX区域
         */
        if((event->pos().rx() > 505 || event->pos().rx() < HEX_ADDR_WIDTH) || indtorrow == 0
                || (verticalScrollBar()->value() == scrollnumrange && indtorrow > scrollnumrest)
                || (verticalScrollBar()->value() == scrollnumrange && indtorrow == scrollnumrest && indtorcolumn > indtoroffset - 1 && indtoroffsetf))
        {
//            qDebug() << "indtoroffset is:" << indtoroffset;
//            qDebug() << "indtorrow is:" << indtorrow;
//            qDebug() << "indtorcolumn is:" << indtorcolumn << Qt::endl;
            indtor = false;
            return;
        }
        indtor = true;
        viewport()->update();
    }
}
