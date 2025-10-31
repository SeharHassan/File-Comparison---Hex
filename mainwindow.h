#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QVector>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>



class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void openFile1();
    void openFile2();
    void compareFiles();

private:
    void buildUi();

    QByteArray m_data1;
    QByteArray m_data2;

    QPushButton *m_btnOpen1;
    QPushButton *m_btnOpen2;
    QPushButton *m_btnCompare;
    QProgressBar *m_progress;
    QLabel *m_labelCount;

    QTableWidget *m_table;
};

#endif
