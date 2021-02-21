#include "noiseControl.h"
#include "ui_noiseControl.h"
#include "channelcontrol.h"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include <QVector>
#include <QDebug>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <QFile>
#include <QTime>

#define VID_DEVICE 0x0483
#define PID_DEVICE 0x5711

noiseControl::noiseControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::noiseControl)
{
    ui->setupUi(this);

    voltageGraph1 = new channelControl(1);
    static_cast<QHBoxLayout*>(ui->centralWidget->layout())->insertWidget(1,voltageGraph1);

    ui->pushButton_Open->setEnabled(true);
    ui->pushButton_Close->setEnabled(false);

    hid             = new hidInterface();
    protocol        = new generalProtocol();
    protocolThreadH = new protocolThread(hid, protocol);
    connect(protocol, &generalProtocol::gpSend, this, &noiseControl::txData, Qt::QueuedConnection);
    /*One channel signals*/
    connect(voltageGraph1, &channelControl::stop, this, &noiseControl::chStop, Qt::QueuedConnection);
    connect(voltageGraph1, &channelControl::startClockWise, this, &noiseControl::chStartClockWise, Qt::QueuedConnection);
    connect(voltageGraph1, &channelControl::setRfCh, this, &noiseControl::setRfCh, Qt::QueuedConnection);
    connect(voltageGraph1, &channelControl::setOutLevel, this, &noiseControl::setOutLevel, Qt::QueuedConnection);

    isChannelRun = false;
}

noiseControl::~noiseControl()
{
    delete ui;
}

void noiseControl::txData(QVector<uint8_t> txData)
{
    hid->write(txData.data(), 64, 10);
}


void noiseControl::chStop(uint8_t channelIndex)
{
    isChannelRun = false;
    protocol->gpStopCommandTx(channelIndex);
}

void noiseControl::chStartClockWise(uint8_t channelIndex)
{
    protocol->gpStartClockWiseCommandTx(channelIndex);
    isChannelRun = true;
}

void noiseControl::setRfCh(uint32_t rfCh)
{
    protocol->gpSetRfCh(rfCh);
}

void noiseControl::setOutLevel(uint32_t outLeve)
{
    protocol->gpSetOutLevel(outLeve);
}

void noiseControl::on_pushButton_Open_clicked()
{
    errno = 0;
    hid->initUSB();
    if(!hid->openInterface(VID_DEVICE, PID_DEVICE)) {
        qDebug()<<"Can't open device";
        return;
    }
    protocolThreadH->start();
    ui->pushButton_Open->setEnabled(false);
    ui->pushButton_Close->setEnabled(true);
    qDebug()<<"Device opened";
}

void noiseControl::on_pushButton_Close_clicked()
{
    hid->closeInterface();
    ui->pushButton_Open->setEnabled(true);
    ui->pushButton_Close->setEnabled(false);
    protocolThreadH->quit();
}
