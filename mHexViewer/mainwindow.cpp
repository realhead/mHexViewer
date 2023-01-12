#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QAction>
#include <QFileDialog>
#include <QStatusBar>
#include <QMessageBox>
#include <QInputDialog>
#include <QVector>


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    this->setFixedSize(760, 520);
    ui->setupUi(this);
    setWindowTitle("mHexViewer");
    ui->statusbar->addWidget(label);
    //添加控件有先后顺序
    //在主widget内，需要先添加副widget，再添加layout
    hv->setFixedSize(740, 460);
    layout->addWidget(hv);
    QWidget *container = new QWidget;
    container->setLayout(layout);
    setCentralWidget(container);
    //设置快捷键
    ui->m_Open->setShortcut(tr("ctrl+o"));
    ui->m_Search->setShortcut(tr("ctrl+s"));
    //主窗口按钮事件信号槽推荐使用引用方式（&）
    connect(ui->m_Open, &QAction::triggered, this, &MainWindow::sOpen);
    connect(ui->m_Search, &QAction::triggered, this, &MainWindow::sSearch);
    connect(ui->m_About, &QAction::triggered, this, &MainWindow::sAbout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sOpen()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select file", ".", "*.*");
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
    {
        QFileInfo fileinfo = QFileInfo(filename);
        QString filetitle = fileinfo.fileName();
        setWindowTitle("mHexViewer - " + filetitle);
        //检测文件大小，大于100MB则警告，然后返回
        if(file.size() > 1024 * 102400 * 1)
        {
            QMessageBox::information(this, "WARNING", "<center>""<p style='font-family: Courier New'>"
                                     "This file is too large!"
                                     "<br>"
                                     " Please selecte a smaller file."
                                     "</p>""</center>");
            file.close();
            return;
        }
        QDataStream in(&file);
        QVector<quint8> contentdata;//将文件以QVector方式，存入内存
        contentdata.resize(file.size());
        //将数据读入，返回读取的单字节数量！重要！要从[0]开始！
        in.readRawData((char*)&contentdata[0], file.size()); //此时file指针指向文件末尾！
        hv->getdata(contentdata);
        label->setText("File size is " + QString::number(file.size() / 1024) + "KB");
        file.close();
        contentdata.clear();
    }
}

void MainWindow::sSearch()
{
    QString addrtext;
    addrtext = QInputDialog::getText(this, "Search", "Please input address:", QLineEdit::EchoMode());
    if(addrtext.toInt(NULL, 16) <= 0)
    {
        QMessageBox::information(this, "WARNING", "<center>""<p style='font-family: Courier New'>"
                                 "This address is improper!"
                                 "<br>"
                                 " Please try again."
                                 "</p>""</center>");
        return;
    }
    hv->searchdata(addrtext.toInt(NULL, 16));
}

void MainWindow::sAbout()
{
    QMessageBox::about(this, "ABOUT", "<center>""<p style='font-family: Courier New'>"
                       "mHexViewer"
                       "<br>"
                       "Hope you enjoy it!"
                       "<br>"
                       "https://github.com/realhead"
                       "</p>""</center>");
}

