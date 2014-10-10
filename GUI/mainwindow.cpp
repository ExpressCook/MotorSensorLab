#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QDebug>

#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QTextStream>
#include <windows.h>
#include <QString>
#include <QList>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <QVector>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
    settings = new SettingsDialog;
    potDataX = new QVector<double>;
    potDataY = new QVector<double>;
    servoPositionDataX = new QVector<double>;
    servoPositionDataY = new QVector<double>;
    IRDataX = new QVector<double>;
    IRDataY = new QVector<double>;
    DCPositionDataX = new QVector<double>;
    DCPositionDataY = new QVector<double>;
    DCVelocityDataX = new QVector<double>;
    DCVelocityDataY = new QVector<double>;
    encoderDataX = new QVector<double>;
    encoderDataY = new QVector<double>;
    stepperPositionDataX = new QVector<double>;
    stepperPositionDataY = new QVector<double>;
    stepperVelocityDataX = new QVector<double>;
    stepperVelocityDataY = new QVector<double>;

    ui->actionConnect->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    initActionsConnections();
    initPlot();

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

MainWindow::~MainWindow()
{
    delete settings;
    delete ui;
}

void MainWindow::openSerialPort()
{
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
            ui->actionConnect->setEnabled(false);
            ui->actionConfigure->setEnabled(false);
            ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                       .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                       .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());
        ui->statusBar->showMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    serial->close();
    ui->actionConnect->setEnabled(true);
    ui->actionConfigure->setEnabled(true);
    ui->statusBar->showMessage(tr("Disconnected"));
}

void MainWindow::readData()
{
    //Sleep(90);
    QString data;
   //QIODevice::bytesAvailable();
   int temp=serial->bytesAvailable();
   if(temp>1)
    {data = serial->readAll();
    //   qDebug()<<data.toLocal8Bit();
   //data = data.trimmed();
   }
//   serial->flush();
   //if(data.isEmpty()==false&&count>10){
     //  if(data.isEmpty()==false){

    point = data.toInt();
    count = 0;
    switch(sysState)
    {
        case 'a':
        {
            servoPositionDataX->push_back(servoPositionDataX->size());
            servoPositionDataY->push_back(point);
            sPosCurve->setSamples(*servoPositionDataX,*servoPositionDataY);
            servoPositionPlot->replot();
            qDebug() << "Received from Arduino: " << point;
            break;
        }
        case 'b':
        {
            potDataX->push_back(potDataX->size());
            potDataY->push_back(point);
            potCurve->setSamples(*potDataX,*potDataY);
            potPlot->replot();
            qDebug() << "Received from pot: " << point;

            QByteArray data2 = serial->readLine();
            int point2 = data2.toInt();

            servoPositionDataX->push_back(servoPositionDataX->size());
            servoPositionDataY->push_back(point2);
            sPosCurve->setSamples(*servoPositionDataX,*servoPositionDataY);
            servoPositionPlot->replot();
            qDebug() << "Received from servo: " << point2;
            break;
        }
        case 'c':
        {
            stepperPositionDataX->push_back(stepperPositionDataX->size());
            stepperPositionDataY->push_back(point);
            stPosCurve->setSamples(*stepperPositionDataX,*stepperPositionDataY);
            stepperPositionPlot->replot();
            qDebug() << "Received from Arduino: " << point;
            break;
        }
        case 'd':
        {
            stepperVelocityDataX->push_back(stepperVelocityDataX->size());
            stepperVelocityDataY->push_back(point);
            stVelCurve->setSamples(*stepperVelocityDataX,*stepperVelocityDataY);
            stepperVelocityPlot->replot();
            qDebug() << "Received from Arduino: " << point;
            break;
        }
        case 'e':
        {
            IRDataX->push_back(IRDataX->size());
            IRDataY->push_back(point);
            IRCurve->setSamples(*IRDataX,*IRDataY);
            IRPlot->replot();
            qDebug() << "Received from Arduino: " << point;

            QByteArray data2 = serial->readLine();
            int point2 = data2.toInt();

            stepperVelocityDataX->push_back(stepperVelocityDataX->size());
            stepperVelocityDataY->push_back(point2);
            stVelCurve->setSamples(*stepperVelocityDataX,*stepperVelocityDataY);
            stepperVelocityPlot->replot();
            qDebug() << "Received from Arduino: " << point2;
            break;
        }
        case 'f':
        {
            //qDebug()<<data;
            DCPositionDataX->push_back(DCPositionDataX->size());
            DCPositionDataY->push_back(point);
            DCPosCurve->setSamples(*DCPositionDataX,*DCPositionDataY);
            DCPositionPlot->replot();
            //qDebug() << "Received from Arduino: " << point;
            break;
        }
        case 'g':
        {
            DCVelocityDataX->push_back(DCVelocityDataX->size());
            DCVelocityDataY->push_back(point);
            DCVelCurve->setSamples(*DCVelocityDataX,*DCVelocityDataY);
            DCVelocityPlot->replot();
            qDebug() << "Received from Arduino: " << point;
            break;
        }
        case 'h':
        {
            encoderDataX->push_back(encoderDataX->size());
            encoderDataY->push_back(point);
            encoderCurve->setSamples(*encoderDataX,*encoderDataY);
            encoderPlot->replot();
            qDebug() << "Received from Arduino: " << point;

            QByteArray data2 = serial->readLine();
            int point2 = data2.toInt();

            DCVelocityDataX->push_back(DCVelocityDataX->size());
            DCVelocityDataY->push_back(point2);
            DCVelCurve->setSamples(*DCVelocityDataX,*DCVelocityDataY);
            DCVelocityPlot->replot();
            qDebug() << "Received from Arduino: " << point2;
            break;
        }
    }
  // }
   // Sleep(10);
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::initActionsConnections()
{
    connect(ui->actionConnect, SIGNAL(clicked()), this, SLOT(openSerialPort()));
    connect(ui->actionConfigure, SIGNAL(clicked()), settings, SLOT(show()));

    ui->Servo_Position_In->setValidator(new QIntValidator(0, 179, this));
    ui->Stepper_Position_In->setValidator(new QIntValidator(-360, 360, this));
    ui->Stepper_Velocity_In->setValidator(new QIntValidator(0, 100, this));
    ui->DC_Position_In->setValidator(new QIntValidator(0, 360, this));
    ui->DC_Velocity_In->setValidator(new QIntValidator(30, 400, this));
}

void MainWindow::initPlot()
{
    //Potentiometer Plot
    potPlot = new QwtPlot(ui->Pot_Out); //creation of the plot
    potPlot->setMinimumSize(390,230); //we set the minimum size of the plot
    QwtText xpot("t");
    QwtText ypot("value");
    xpot.setFont(QFont("MS Shell Dlg2", 8));
    ypot.setFont(QFont("MS Shell Dlg2", 8));
    potPlot->setAxisTitle(QwtPlot::xBottom, xpot );
    potPlot->setAxisFont(QwtPlot::xBottom,QFont("MS Shell Dlg2",6));
    potPlot->setAxisTitle(QwtPlot::yLeft, ypot);
    potPlot->setAxisFont(QwtPlot::yLeft,QFont("MS Shell Dlg2",6));
    potCurve = new QwtPlotCurve;
    potCurve->setPen(QPen(Qt::green));
    potCurve->attach(potPlot);

    //Servo Position Plot
    servoPositionPlot = new QwtPlot(ui->Servo_Position_Out); //creation of the plot
    servoPositionPlot->setMinimumSize(390,230); //we set the minimum size of the plot
    QwtText xspos("t");
    QwtText yspos("x(deg)");
    xspos.setFont(QFont("MS Shell Dlg2", 8));
    yspos.setFont(QFont("MS Shell Dlg2", 8));
    servoPositionPlot->setAxisTitle(QwtPlot::xBottom, xspos );
    servoPositionPlot->setAxisFont(QwtPlot::xBottom,QFont("MS Shell Dlg2",6));
    servoPositionPlot->setAxisTitle(QwtPlot::yLeft, yspos);
    servoPositionPlot->setAxisFont(QwtPlot::yLeft,QFont("MS Shell Dlg2",6));
    servoPositionPlot->setAxisScale(QwtPlot::yLeft,0,400);
    sPosCurve = new QwtPlotCurve;
    sPosCurve->setPen(QPen(Qt::red));
    sPosCurve->attach(servoPositionPlot);

    //IR Plot
    IRPlot = new QwtPlot(ui->IR_Out); //creation of the plot
    IRPlot->setMinimumSize(390,230); //we set the minimum size of the plot
    QwtText xir("t");
    QwtText yir("value");
    xir.setFont(QFont("MS Shell Dlg2", 8));
    yir.setFont(QFont("MS Shell Dlg2", 8));
    IRPlot->setAxisTitle(QwtPlot::xBottom, xir );
    IRPlot->setAxisFont(QwtPlot::xBottom,QFont("MS Shell Dlg2",6));
    IRPlot->setAxisTitle(QwtPlot::yLeft, yir);
    IRPlot->setAxisFont(QwtPlot::yLeft,QFont("MS Shell Dlg2",6));
    IRCurve = new QwtPlotCurve;
    IRCurve->setPen(QPen(Qt::green));
    IRCurve->attach(IRPlot);

    //Stepper Position Plot
    stepperPositionPlot = new QwtPlot(ui->Stepper_Position_Out); //creation of the plot
    stepperPositionPlot->setMinimumSize(390,230); //we set the minimum size of the plot
    QwtText xstpos("t");
    QwtText ystpos("incremental value(deg)");
    xstpos.setFont(QFont("MS Shell Dlg2", 8));
    ystpos.setFont(QFont("MS Shell Dlg2", 8));
    stepperPositionPlot->setAxisTitle(QwtPlot::xBottom, xstpos );
    stepperPositionPlot->setAxisFont(QwtPlot::xBottom,QFont("MS Shell Dlg2",6));
    stepperPositionPlot->setAxisTitle(QwtPlot::yLeft, ystpos);
    stepperPositionPlot->setAxisFont(QwtPlot::yLeft,QFont("MS Shell Dlg2",6));
    stPosCurve = new QwtPlotCurve;
    stPosCurve->setPen(QPen(Qt::red));
    stPosCurve->attach(stepperPositionPlot);

    //Stepper Velocity Plot
    stepperVelocityPlot = new QwtPlot(ui->Stepper_Velocity_Out); //creation of the plot
    stepperVelocityPlot->setMinimumSize(390,230); //we set the minimum size of the plot
    QwtText xstvel("t");
    QwtText ystvel("v");
    xstvel.setFont(QFont("MS Shell Dlg2", 8));
    ystvel.setFont(QFont("MS Shell Dlg2", 8));
    stepperVelocityPlot->setAxisTitle(QwtPlot::xBottom, xstvel );
    stepperVelocityPlot->setAxisFont(QwtPlot::xBottom,QFont("MS Shell Dlg2",6));
    stepperVelocityPlot->setAxisTitle(QwtPlot::yLeft, ystvel);
    stepperVelocityPlot->setAxisFont(QwtPlot::yLeft,QFont("MS Shell Dlg2",6));
    stepperVelocityPlot->setAxisScale(QwtPlot::yLeft,0,100);
    stVelCurve = new QwtPlotCurve;
    stVelCurve->setPen(QPen(Qt::blue));
    stVelCurve->attach(stepperVelocityPlot);

    //Encoder Plot
    encoderPlot = new QwtPlot(ui->Encoder_Out); //creation of the plot
    encoderPlot->setMinimumSize(390,230); //we set the minimum size of the plot
    QwtText xenc("t");
    QwtText yenc("value");
    xenc.setFont(QFont("MS Shell Dlg2", 8));
    yenc.setFont(QFont("MS Shell Dlg2", 8));
    encoderPlot->setAxisTitle(QwtPlot::xBottom, xenc );
    encoderPlot->setAxisFont(QwtPlot::xBottom,QFont("MS Shell Dlg2",6));
    encoderPlot->setAxisTitle(QwtPlot::yLeft, yenc);
    encoderPlot->setAxisFont(QwtPlot::yLeft,QFont("MS Shell Dlg2",6));
    encoderCurve = new QwtPlotCurve;
    encoderCurve->setPen(QPen(Qt::green));
    encoderCurve->attach(encoderPlot);

    //DC Position Plot
    DCPositionPlot = new QwtPlot(ui->DC_Position_Out); //creation of the plot
    DCPositionPlot->setMinimumSize(390,230); //we set the minimum size of the plot
    QwtText xdcpos("t");
    QwtText ydcpos("x(deg)");
    xdcpos.setFont(QFont("MS Shell Dlg2", 8));
    ydcpos.setFont(QFont("MS Shell Dlg2", 8));
    DCPositionPlot->setAxisTitle(QwtPlot::xBottom, xdcpos );
    DCPositionPlot->setAxisFont(QwtPlot::xBottom,QFont("MS Shell Dlg2",6));
    DCPositionPlot->setAxisTitle(QwtPlot::yLeft, ydcpos);
    DCPositionPlot->setAxisFont(QwtPlot::yLeft,QFont("MS Shell Dlg2",6));
    DCPosCurve = new QwtPlotCurve;
    DCPosCurve->setPen(QPen(Qt::red));
    DCPosCurve->attach(DCPositionPlot);

    //DC Velocity Plot
    DCVelocityPlot = new QwtPlot(ui->DC_Velocity_Out); //creation of the plot
    DCVelocityPlot->setMinimumSize(390,230); //we set the minimum size of the plot
    QwtText xdcvel("t");
    QwtText ydcvel("v");
    xdcvel.setFont(QFont("MS Shell Dlg2", 8));
    ydcvel.setFont(QFont("MS Shell Dlg2", 8));
    DCVelocityPlot->setAxisTitle(QwtPlot::xBottom, xdcvel );
    DCVelocityPlot->setAxisFont(QwtPlot::xBottom,QFont("MS Shell Dlg2",6));
    DCVelocityPlot->setAxisTitle(QwtPlot::yLeft, ydcvel);
    DCVelocityPlot->setAxisFont(QwtPlot::yLeft,QFont("MS Shell Dlg2",6));
    DCVelCurve = new QwtPlotCurve;
    DCVelCurve->setPen(QPen(Qt::blue));
    DCVelCurve->attach(DCVelocityPlot);
}

void MainWindow::on_Choose_Pot_clicked()
{
    ui->Servo_Position_In->setEnabled(false);
    ui->Stepper_Position_In->setEnabled(false);
    ui->Stepper_Velocity_In->setEnabled(false);
    ui->DC_Position_In->setEnabled(false);
    ui->DC_Velocity_In->setEnabled(false);
    serial->write("b");
    sysState = 'b';
    qDebug() << "Sent to Arduino: " << sysState;
}

void MainWindow::on_Choose_Servo_Position_clicked()
{
    ui->Servo_Position_In->setEnabled(true);
    ui->Stepper_Position_In->setEnabled(false);
    ui->Stepper_Velocity_In->setEnabled(false);
    ui->DC_Position_In->setEnabled(false);
    ui->DC_Velocity_In->setEnabled(false);
}

void MainWindow::on_Choose_IR_clicked()
{
    ui->Servo_Position_In->setEnabled(false);
    ui->Stepper_Position_In->setEnabled(false);
    ui->Stepper_Velocity_In->setEnabled(false);
    ui->DC_Position_In->setEnabled(false);
    ui->DC_Velocity_In->setEnabled(false);
    serial->write("e");
    sysState = 'e';
    qDebug() << "Sent to Arduino: " << sysState;
}

void MainWindow::on_Choose_Stepper_Position_clicked()
{
    ui->Servo_Position_In->setEnabled(false);
    ui->Stepper_Position_In->setEnabled(true);
    ui->Stepper_Velocity_In->setEnabled(false);
    ui->DC_Position_In->setEnabled(false);
    ui->DC_Velocity_In->setEnabled(false);
}

void MainWindow::on_Choose_Stepper_Velocity_clicked()
{
    ui->Servo_Position_In->setEnabled(false);
    ui->Stepper_Position_In->setEnabled(false);
    ui->Stepper_Velocity_In->setEnabled(true);
    ui->DC_Position_In->setEnabled(false);
    ui->DC_Velocity_In->setEnabled(false);
}

void MainWindow::on_Choose_Encoder_clicked()
{
    ui->Servo_Position_In->setEnabled(false);
    ui->Stepper_Position_In->setEnabled(false);
    ui->Stepper_Velocity_In->setEnabled(false);
    ui->DC_Position_In->setEnabled(false);
    ui->DC_Velocity_In->setEnabled(false);
    serial->write("h");
    sysState = 'h';
    qDebug() << "Sent to Arduino: " << sysState;
}

void MainWindow::on_Choose_DC_Position_clicked()
{
    ui->Servo_Position_In->setEnabled(false);
    ui->Stepper_Position_In->setEnabled(false);
    ui->Stepper_Velocity_In->setEnabled(false);
    ui->DC_Position_In->setEnabled(true);
    ui->DC_Velocity_In->setEnabled(false);
}

void MainWindow::on_Choose_DC_Velocity_clicked()
{
    ui->Servo_Position_In->setEnabled(false);
    ui->Stepper_Position_In->setEnabled(false);
    ui->Stepper_Velocity_In->setEnabled(false);
    ui->DC_Position_In->setEnabled(false);
    ui->DC_Velocity_In->setEnabled(true);
}

void MainWindow::on_Servo_Position_In_returnPressed()
{
    sysState = 'a';
    QString out = outputString(ui->Servo_Position_In->text());
    qDebug() << "Sent to Arduino:" << out;
}

void MainWindow::on_Stepper_Position_In_returnPressed()
{
    sysState = 'c';
    QString out = outputString(ui->Stepper_Position_In->text());
    qDebug() << "Sent to Arduino:" << out;
}

void MainWindow::on_Stepper_Velocity_In_returnPressed()
{
    sysState = 'd';
    QString out = outputString(ui->Stepper_Velocity_In->text());
    qDebug() << "Sent to Arduino:" << out;
}

void MainWindow::on_DC_Position_In_returnPressed()
{
    sysState = 'f';
    QString out = outputString(ui->DC_Position_In->text());
    qDebug() << "Sent to Arduino:" << out;
}

void MainWindow::on_DC_Velocity_In_returnPressed()
{
    sysState = 'g';
    QString out = outputString(ui->DC_Velocity_In->text());
    qDebug() << "Sent to Arduino:" << out;
}

QString MainWindow::outputString(const QString &data)
{
    serial->write(sysState+data.toLatin1());
    return sysState+data.toLatin1();
}
