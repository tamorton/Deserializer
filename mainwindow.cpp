#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    deserial = new TDeserializer();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete deserial;
}

void MainWindow::on_actionOpen_triggered()
{
    // open binary
    QString inName = QFileDialog::getOpenFileName(this);
    if (inName.isEmpty()) return;

    ui->textEdit->append(inName);

    QString logname = inName + ".log";

    TFileInput fileinput(inName);
    if(!fileinput.open()) {
        ui->textEdit->append("Failed to open in file");
        return;
    }

    QString msg;

    int result = deserial->read(fileinput);

    if(result != DESERIAL_OK) {
        deserial->getErrorString(result, msg);
        msg = "Read failed: " + msg;
        ui->textEdit->append(msg);
        return;
    }

    msg.sprintf("Read %d streams", deserial->streamCount());
    ui->textEdit->append(msg);

    deserial->associateReferences();
    ui->textEdit->append("Associated records");

    ui->textEdit->append("Ok");
}

void MainWindow::on_actionDisplay_triggered()
{
    for(int n=0; n<deserial->streamCount(); n++) {
        QString str;
        QTextStream ts(&str);
        ts << "=== Stream " << n+1 << " ===\n";
        deserial->getStream(n)->write(ts);
        ui->textEdit->append(str);
    }
}

void MainWindow::on_actionSave_triggered()
{
    // save text
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList outfiles;
    if (dialog.exec()) outfiles = dialog.selectedFiles();
    else return;

    QFile outfile(outfiles[0]);
    if(!outfile.open(QIODevice::WriteOnly)) {
        ui->textEdit->append("Failed to open file");
        return;
    }

    QTextStream ts(&outfile);

    for(int n=0; n<deserial->streamCount(); n++) {
        ts << "=== Stream " << n+1 << " ===\n";
        deserial->getStream(n)->write(ts);
    }

    outfile.close();

    ui->textEdit->append("Saved to:");
    ui->textEdit->append(outfiles[0]);
}

void MainWindow::on_actionMembers_triggered()
{
    QStringList path;
    QString str;
    QTextStream ts(&str);

    // int
    path << "0" << "Storage.Stor+AssayAllDataHash" << "AllData" << "Header" << "ChanCount";

    qint64 chancount;
    for(int i=0; i<path.length(); i++) {
        ts << path.at(i) << "/";
    }
    ts << ": ";

    TSearchType* result = deserial->getObject(path);
    if(result == NULL) {
        ts << "Not found";
    }
    else {
        bool ok = result->getInt(chancount);
        if(ok) {
            ts << chancount;
        }
        else ts << "not got";
    }
    ui->textEdit->append(str);
    str = "";
    path.clear();

    // string list
    path << "0" << "Storage.Stor+AssayAllDataHash" << "AllData" << "Header" << "ChanNames" << "_items";
    for(int i=0; i<path.length(); i++) {
        ts << path.at(i) << "/";
    }
    ts << ": ";

    result = deserial->getObject(path);
    if(result == NULL) {
        ts << "Not found";
    }
    else {
        TBinaryObject** d;
        int len;
        bool ok = result->getObjectArray(&d, len);
        if(ok) {
            for(int i=0; i<len; i++) {
                QString s;
                if(d[i]->getString(s)) {
                    ts << s;
                }
                else ts << "not got";
                ts << ", ";
            }
        }
        else ts << "not got";
    }
    ui->textEdit->append(str);
    str = "";
    path.clear();

    // int
    qint64 itemcount;
    path << "0" << "Storage.Stor+AssayAllDataHash" << "AllData" << "Data" << "SampVals" << "_size";
    for(int i=0; i<path.length(); i++) {
        ts << path.at(i) << "/";
    }
    ts << ": ";

    result = deserial->getObject(path);
    if(result == NULL) {
        ts << "Not found";
    }
    else {
        bool ok = result->getInt(itemcount);
        if(ok) {
            ts << itemcount;
        }
        else ts << "not got";
        ts << "\n";
    }

    ui->textEdit->append(str);
    str = "";
    path.clear();

    // objectarray
    path << "0" << "Storage.Stor+AssayAllDataHash" << "AllData" << "Data" << "SampVals" << "_items";

    for(qint64 item=0; item<5; item++) {

        QStringList itempath = path;
        QString itemstr;
        itemstr.sprintf("%d", item);
        itempath << itemstr << "_items";

        for(int i=0; i<itempath.length(); i++) {
            ts << itempath.at(i) << "/";
        }
        ts << ": ";

        result = deserial->getObject(itempath);
        if(result == NULL) {
            ts << itemstr << "Not found\n";
        }
        else {
            double* d;
            int len;
            bool ok = result->getDoubleArray(&d, len);
            if(ok) {
                ts << "length:" << len << " values:" << d[0];
            }
            else ts << "nope";
            ts << "\n";
        }
    }

    ui->textEdit->append(str);
    str = "";
    path.clear();
}

void MainWindow::on_actionLog_triggered()
{
    // open binary
    // save hex in logfile
    QString inName = QFileDialog::getOpenFileName(this);
    if (inName.isEmpty()) return;

    QString logname = inName + ".log";

    ui->textEdit->append(inName);
    ui->textEdit->append(logname);

    TFileInputLog fileinput(inName, logname);
    if(!fileinput.open()) {
        ui->textEdit->append("Failed to open in file");
        return;
    }

    QString msg;

    int result = deserial->read(fileinput);

    if(result != DESERIAL_OK) {
        deserial->getErrorString(result, msg);
        msg = "Read failed: " + msg;
        ui->textEdit->append(msg);
        return;
    }

    msg.sprintf("Read %d streams", deserial->streamCount());
    ui->textEdit->append(msg);

    ui->textEdit->append("Ok");

}
