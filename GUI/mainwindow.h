/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>

#include <QMainWindow>

#include <QtSerialPort/QSerialPort>
#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <QVector>

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class Console;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void readData();

    void handleError(QSerialPort::SerialPortError error);

    void on_Choose_Servo_Position_clicked();
    void on_Choose_Pot_clicked();
    void on_Choose_Stepper_Position_clicked();
    void on_Choose_Stepper_Velocity_clicked();
    void on_Choose_IR_clicked();
    void on_Choose_DC_Position_clicked();
    void on_Choose_DC_Velocity_clicked();
    void on_Choose_Encoder_clicked();

    void on_Servo_Position_In_returnPressed();
    void on_Stepper_Position_In_returnPressed();
    void on_Stepper_Velocity_In_returnPressed();
    void on_DC_Position_In_returnPressed();
    void on_DC_Velocity_In_returnPressed();

    QString outputString(const QString &data);

private:
    void initActionsConnections();
    void initPlot();

private:
    Ui::MainWindow *ui;
    Console *console;
    SettingsDialog *settings;
    QSerialPort *serial;
    QSerialPort *arduino;

    QwtPlotCurve *sPosCurve;
    QwtPlot *servoPositionPlot;
    QVector<double> *servoPositionDataX;
    QVector<double> *servoPositionDataY;

    QwtPlotCurve *potCurve;
    QwtPlot *potPlot;
    QVector<double> *potDataX;
    QVector<double> *potDataY;

    QwtPlotCurve *IRCurve;
    QwtPlot *IRPlot;
    QVector<double> *IRDataX;
    QVector<double> *IRDataY;

    QwtPlotCurve *stPosCurve;
    QwtPlot *stepperPositionPlot;
    QVector<double> *stepperPositionDataX;
    QVector<double> *stepperPositionDataY;

    QwtPlotCurve *stVelCurve;
    QwtPlot *stepperVelocityPlot;
    QVector<double> *stepperVelocityDataX;
    QVector<double> *stepperVelocityDataY;

    QwtPlotCurve *encoderCurve;
    QwtPlot *encoderPlot;
    QVector<double> *encoderDataX;
    QVector<double> *encoderDataY;

    QwtPlotCurve *DCPosCurve;
    QwtPlot *DCPositionPlot;
    QVector<double> *DCPositionDataX;
    QVector<double> *DCPositionDataY;

    QwtPlotCurve *DCVelCurve;
    QwtPlot *DCVelocityPlot;
    QVector<double> *DCVelocityDataX;
    QVector<double> *DCVelocityDataY;

    char sysState;
    int point;
    int count;

};

#endif // MAINWINDOW_H
