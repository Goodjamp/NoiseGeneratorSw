#include <noisecontrol.h>
#include <ui_noisecontrol.h>
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
#include <QRandomGenerator>

#define VID_DEVICE 0x0483
#define PID_DEVICE 0x5711

noiseControl::noiseControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::noiseControl)
{
    ui->setupUi(this);

    for (uint32_t k = 0; k < CHANNEL_CNT; k++) {
        channelList.append(new channelControl(1));
        static_cast<QHBoxLayout*>(ui->scrollAreaWidgetContents->layout())->insertWidget(k, channelList[k]);
    }


    ui->pushButton_Open->setEnabled(true);
    ui->pushButton_Close->setEnabled(false);

    hid             = new hidInterface();
    protocol        = new generalProtocol();
    protocolThreadH = new protocolThread(hid, protocol);
    connect(protocol, &generalProtocol::gpSend, this, &noiseControl::txData, Qt::QueuedConnection);

    modulateControlTimer = new QTimer(this);
    connect(modulateControlTimer, &QTimer::timeout,
            this, &noiseControl::timeout,
             Qt::QueuedConnection);
    uiActiveUpdate(CLOSE_DEVICE);
}

noiseControl::~noiseControl()
{
    delete ui;
}

void noiseControl::txData(QVector<uint8_t> txData)
{
    uint32_t rezTx;
    if ( (rezTx = hid->write(txData.data(), 64, 10)) == 0){
        on_pushButton_Close_clicked();
    }
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
    uiActiveUpdate(OPEN_DEVICE);
}

void noiseControl::on_pushButton_Close_clicked()
{
    hid->closeInterface();
    ui->pushButton_Open->setEnabled(true);
    ui->pushButton_Close->setEnabled(false);
    protocolThreadH->quit();
    uiActiveUpdate(CLOSE_DEVICE);
}

void noiseControl::on_pushButtonSetChannel_clicked()
{
    QVector<generalProtocol::SetRfChArg> chList;
    generalProtocol::SetRfChArg channelArg;
    for (int k = 0; k < channelList.size(); k++) {
        if (channelList[k]->isUse()) {
            channelArg.devCh = static_cast<uint8_t>(k);
            channelArg.rfCh =static_cast<uint8_t>(channelList[k]->getChannel());
            chList.push_back(channelArg);
        }
    }
    protocol->gpSetRfCh(chList);
}

void noiseControl::on_pushButtonSetLevel_clicked()
{
    QVector<generalProtocol::SetLevelArg> chList;
    generalProtocol::SetLevelArg levelArg;
    for (int k = 0; k < channelList.size(); k++) {
        if (channelList[k]->isUse()) {
            levelArg.devCh = static_cast<uint8_t>(k);
            levelArg.level = static_cast<uint8_t>(channelList[k]->getLevel());
            chList.push_back(levelArg);
        }
    }
    protocol->gpSetOutLevel(chList);
}

void noiseControl::on_pushButtonRun_clicked()
{
    runDevChList.clear();
    for (int k = 0; k < channelList.size(); k++) {
        if (channelList[k]->isUse()) {
            runDevChList.push_back(static_cast<uint8_t>(k));
        }
    }
    protocol->gpStartClockWiseCommandTx(runDevChList);
    uiActiveUpdate(RUN);
}

void noiseControl::on_pushButtonRunMod_clicked()
{
    runDevChList.clear();
    bool isDetect = false;
    for (int k = 0; k < channelList.size(); k++) {
        if (channelList[k]->isUse()) {
            runDevChList.push_back(k);
        }
        if (isDetect == false) {
            isDetect = true;
            channelList[k]->getTiming(&modNoiseMin, &modNoiseLen, &modSilentMin, &modSilentLen);
            if (modNoiseLen < modNoiseMin ||
                modSilentLen < modSilentMin) {
                return;
            }
            modNoiseLen = modNoiseLen - modNoiseMin;
            modSilentLen = modSilentLen - modSilentMin;
        }
    }
    modulateIsRunning = true;
    uiActiveUpdate(RUN_MODULATE);
    timeout();
}

void noiseControl::on_pushButtonStop_clicked()
{
    protocol->gpStopCommandTx(runDevChList);
    modulateIsRunning = false;
    uiActiveUpdate(STOP);
}

void noiseControl::uiActiveUpdate(ActionType actionType)
{
    switch (actionType) {

    case OPEN_DEVICE:
        ui->pushButton_Open->setEnabled(false);
        ui->pushButton_Close->setEnabled(true);
        ui->pushButtonSetChannel->setEnabled(true);
        ui->pushButtonSetLevel->setEnabled(true);
        ui->pushButtonRun->setEnabled(true);
        ui->pushButtonRunMod->setEnabled(true);
        ui->pushButtonStop->setEnabled(false);
        break;
    case CLOSE_DEVICE:
        ui->pushButton_Open->setEnabled(true);
        ui->pushButton_Close->setEnabled(false);
        ui->pushButtonSetChannel->setEnabled(false);
        ui->pushButtonSetLevel->setEnabled(false);
        ui->pushButtonRun->setEnabled(false);
        ui->pushButtonRunMod->setEnabled(false);
        ui->pushButtonStop->setEnabled(false);
        break;
    case STOP:
        ui->pushButtonRun->setEnabled(true);
        ui->pushButtonRunMod->setEnabled(true);
        ui->pushButtonStop->setEnabled(false);
        break;
    case RUN:
        ui->pushButtonRun->setEnabled(false);
        ui->pushButtonRunMod->setEnabled(false);
        ui->pushButtonStop->setEnabled(true);
        break;
    case RUN_MODULATE:
        ui->pushButtonRun->setEnabled(false);
        ui->pushButtonRunMod->setEnabled(false);
        ui->pushButtonStop->setEnabled(true);
        break;
    }
}

void noiseControl::timeout()
{
    if (modulateIsRunning == false) {
        return;
    }
    modulateControlTimer->stop();
    if (modulateState == MODULATE_LOW) {
        modulateState = MODULATE_HIGH;
        protocol->gpStartClockWiseCommandTx(runDevChList);
        modulateControlTimer->start(modNoiseMin
                                    + QRandomGenerator::global()->bounded(modNoiseLen));

    } else {
        modulateState = MODULATE_LOW;
        protocol->gpStopCommandTx(runDevChList);
        modulateControlTimer->start(modSilentMin
                                    + QRandomGenerator::global()->bounded(modSilentLen));
    }
}

void noiseControl::on_pushButtonAllCh_clicked()
{
    foreach(channelControl *channel, channelList) {
        channel->setUse();
    }
}

void noiseControl::on_pushButton_clicked()
{
    foreach(channelControl *channel, channelList) {
        channel->clearUse();
    }
}
