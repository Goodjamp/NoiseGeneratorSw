#include <channelcontrol.h>
#include <ui_channelcontrol.h>
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>
#include <QRandomGenerator>

#include <stdbool.h>
#include <stdint.h>

channelControl::channelControl(char inChannelIndex, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::channelControl)
{
    QValidator *validator = new QIntValidator(1, 120, this);
    modulateControlTimer = new QTimer();
    modulateIsRunning = false;
    channelIndex = inChannelIndex;
    ui->setupUi(this);
    ui->lineEdit->setValidator(validator);
    connect(modulateControlTimer, &QTimer::timeout,
            this, &channelControl::timeout,
             Qt::QueuedConnection);
}

void channelControl::timeout()
{
    if (modulateIsRunning == false) {
        return;
    }
    modulateControlTimer->stop();
    if (modulateState == modulateLow) {
        modulateState = modulateHigh;
        startClockWise(channelIndex);
        modulateControlTimer->start(modNoiseMin
                                    + QRandomGenerator::global()->bounded(modNoiseLen));

    } else {
        modulateState = modulateLow;
        emit stop(channelIndex);
        modulateControlTimer->start(modSilentMin
                                    + QRandomGenerator::global()->bounded(modSilentLen));
    }
}

channelControl::~channelControl()
{
    delete ui;
}

void channelControl::on_pushButtonSetConfig_clicked()
{
    uint32_t rfCh = ui->lineEdit->text().toUInt();
    if (rfCh > 120) {
        return;
    }
    emit setRfCh(rfCh);
    emit setOutLevel(ui->comboBox->currentIndex());
}

void channelControl::on_pushButtonStopNoise_clicked()
{
    emit stop(channelIndex);
    modulateIsRunning = false;
    modulateControlTimer->stop();
}

void channelControl::on_pushButtonSingleNoise_clicked()
{
    emit startClockWise(channelIndex);
}

void channelControl::on_pushButtonModulateNoise_clicked()
{
    int temp;
    /*Noise interval config*/
    modNoiseMin = ui->minHigh->text().toInt();
    temp =  ui->maxHigh->text().toInt();
    if (temp < modNoiseMin) {
        return;
    }
    modNoiseLen = temp - modNoiseMin;
    /*Silent interval config*/
    modSilentMin = ui->minLow->text().toInt();
    temp =  ui->maxLow->text().toInt();
    if (temp < modSilentMin) {
        return;
    }
    modSilentLen = temp - modSilentMin;
    modulateIsRunning = true;
    timeout();
}
