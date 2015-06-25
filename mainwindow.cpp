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
        ts << "=== Stream " << n << " ===\n";
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
        ts << "=== Stream " << n << " ===\n";
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
    path << "0" << "DeserialTest.TestClass" << "singleint";

    for(int i=0; i<path.length(); i++) {
        ts << path.at(i) << "/";
    }
    ts << ": ";

    TSearchType* result = deserial->getObject(path);
    if(result == NULL) {
        ts << "Not found";
    }
    else {
        qint64 singleint;
        bool ok = result->getInt(singleint);
        if(ok) {
            ts << singleint;
        }
        else ts << "not got";
    }
    ui->textEdit->append(str);


    str = "";
    path.clear();

    // class list
    path << "0" << "DeserialTest.TestClass" << "classlist" << "_items" << "1" << "smallint";

    for(int i=0; i<path.length(); i++) {
        ts << path.at(i) << "/";
    }
    ts << ": ";

    result = deserial->getObject(path);
    if(result == NULL) {
        ts << "Not found";
    }
    else {
        qint64 int1;
        bool ok = result->getInt(int1);
        if(ok) {
            ts << int1;
        }
        else ts << "not got";
    }
    ui->textEdit->append(str);

    str = "";
    path.clear();

    // int array
    path << "0" << "DeserialTest.TestClass" << "intarray";

    for(int i=0; i<path.length(); i++) {
        ts << path.at(i) << "/";
    }
    ts << ": ";

    result = deserial->getObject(path);
    if(result == NULL) {
        ts << "Not found";
    }
    else {
        qint32* a;
        int len;
        bool ok = result->getInt32Array(&a, len);
        if(ok) {
            ts << "length:" << len << " values: ";
            for(int p=0; p<len; p++) {
                ts << a[p] << ", ";
            }
        }
        else ts << "nope";
        ts << "\n";
    }

    ui->textEdit->append(str);
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
