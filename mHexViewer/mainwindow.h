#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "hview.h"
#include <QMainWindow>
#include <QLabel>
#include <QFile>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel *label = new QLabel(this);
    QVBoxLayout *layout = new QVBoxLayout;
    hview *hv = new hview;

private slots:
    void sOpen();
    void sAbout();
    void sSearch();
};
#endif // MAINWINDOW_H
