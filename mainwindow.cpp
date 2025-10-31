#include "mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QProgressBar>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>

struct DiffEntry {
    qint64 offset;
    QString hex1;
    QString hex2;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    buildUi();
}

void MainWindow::buildUi()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);


    m_btnOpen1 = new QPushButton("Open File 1", this);
    m_btnOpen2 = new QPushButton("Open File 2", this);
    m_btnCompare = new QPushButton("Compare", this);
    m_progress = new QProgressBar(this);
    m_progress->setVisible(false);
    m_progress->setMinimum(0);
    m_progress->setMaximum(100);


    m_labelCount = new QLabel("Differences: 0", this);


    m_table = new QTableWidget(this);
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels({"Offset", "File1", "File2"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionMode(QAbstractItemView::NoSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);


    connect(m_btnOpen1, &QPushButton::clicked, this, &MainWindow::openFile1);
    connect(m_btnOpen2, &QPushButton::clicked, this, &MainWindow::openFile2);
    connect(m_btnCompare, &QPushButton::clicked, this, &MainWindow::compareFiles);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_btnOpen1);
    btnLayout->addWidget(m_btnOpen2);
    btnLayout->addWidget(m_btnCompare);
    btnLayout->addWidget(m_progress);
    btnLayout->addStretch();
    btnLayout->addWidget(m_labelCount);

    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(m_table);


    setFixedSize(1100, 700);
    setWindowTitle("File Comparison - Hex - By SeharH");
}

void MainWindow::openFile1()
{
    QString path = QFileDialog::getOpenFileName(this, "Open File 1");
    if (!path.isEmpty()) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, "Error", "Cannot open file 1");
            return;
        }
        m_data1 = file.readAll();
        file.close();
    }
}

void MainWindow::openFile2()
{
    QString path = QFileDialog::getOpenFileName(this, "Open File 2");
    if (!path.isEmpty()) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, "Error", "Cannot open file 2");
            return;
        }
        m_data2 = file.readAll();
        file.close();
    }
}

void MainWindow::compareFiles()
{
    if (m_data1.isEmpty() || m_data2.isEmpty()) {
        QMessageBox::information(this, "Info", "Please open both files before comparing.");
        return;
    }

    m_table->setRowCount(0);
    QVector<DiffEntry> diffs;

    const qint64 maxLen = qMax(m_data1.size(), m_data2.size());
    m_progress->setVisible(true);
    m_progress->setValue(0);

    for (qint64 i = 0; i < maxLen; ++i) {
        QByteArray b1 = (i < m_data1.size()) ? QByteArray(1, m_data1.at(i)) : QByteArray(1, 0);
        QByteArray b2 = (i < m_data2.size()) ? QByteArray(1, m_data2.at(i)) : QByteArray(1, 0);

        if (b1 != b2) {
            DiffEntry entry;
            entry.offset = i;
            entry.hex1 = QString("%1").arg(static_cast<unsigned char>(b1.at(0)), 2, 16, QLatin1Char('0')).toUpper();
            entry.hex2 = QString("%1").arg(static_cast<unsigned char>(b2.at(0)), 2, 16, QLatin1Char('0')).toUpper();
            diffs.append(entry);
        }

        if ((i & 0xFFFF) == 0) {
            int percent = int(double(i) / double(maxLen) * 100.0);
            m_progress->setValue(percent);
            QApplication::processEvents();
        }
    }
    m_progress->setValue(100);


    m_table->setRowCount(diffs.size());
    for (int row = 0; row < diffs.size(); ++row) {
        const DiffEntry &d = diffs[row];

        QTableWidgetItem *itemOffset = new QTableWidgetItem(QString("%1").arg(d.offset, 8, 16, QLatin1Char('0')).toUpper());
        QTableWidgetItem *item1 = new QTableWidgetItem(d.hex1);
        QTableWidgetItem *item2 = new QTableWidgetItem(d.hex2);

        item1->setBackground(Qt::red);
        item2->setBackground(Qt::green);

        m_table->setItem(row, 0, itemOffset);
        m_table->setItem(row, 1, item1);
        m_table->setItem(row, 2, item2);
    }

    m_labelCount->setText(QString("Differences: %1").arg(diffs.size()));
    m_progress->setVisible(false);
}
