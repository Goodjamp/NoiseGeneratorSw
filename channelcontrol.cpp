#include <channelcontrol.h>
#include <ui_channelcontrol.h>
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>
#include <QRandomGenerator>

#include <QDebug>
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
    ui->lineEditRfCh->setValidator(validator);
    ui->lineEditMaxLow->setValidator(validator);
    ui->lineEditMaxHigh->setValidator(validator);
    ui->lineEditMinLow->setValidator(validator);
    ui->lineEditMinHigh->setValidator(validator);
    ui->checkBoxUseDevCh->setChecked(true);
    ui->comboBoxLevel->setCurrentIndex(ui->comboBoxLevel->count() - 1);
    qDebug()<<"ui->comboBoxLevel->count() = "<<ui->comboBoxLevel->count()<<"\n";
    /*
    connect(modulateControlTimer, &QTimer::timeout,
            this, &channelControl::timeout,
             Qt::QueuedConnection);
    */
}


channelControl::~channelControl()
{
    delete ui;
}

int channelControl::getChannel()
{
    return ui->lineEditRfCh->text().toInt(NULL);
}

int channelControl::getLevel()
{
    return ui->comboBoxLevel->currentIndex();
}

void channelControl::getTiming(int *minHigh, int *maxHigh,
                               int *minLow, int *maxLow)
{
    *minHigh = ui->lineEditMinHigh->text().toInt(NULL);
    *maxHigh = ui->lineEditMaxHigh->text().toInt(NULL);
    *minLow = ui->lineEditMinLow->text().toInt(NULL);
    *maxLow = ui->lineEditMaxLow->text().toInt(NULL);
}

bool channelControl::isUse()
{
    return ui->checkBoxUseDevCh->isChecked();
}

void channelControl::setUse()
{
    ui->checkBoxUseDevCh->setChecked(true);
}

void channelControl::clearUse()
{
    ui->checkBoxUseDevCh->setChecked(false);
}

/*
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


void channelControl::on_pushButtonModulateNoise_clicked()
{
    int temp;

    modNoiseMin = ui->minHigh->text().toInt();
    temp =  ui->maxHigh->text().toInt();
    if (temp < modNoiseMin) {
        return;
    }
    modNoiseLen = temp - modNoiseMin;

    modSilentMin = ui->minLow->text().toInt();
    temp =  ui->maxLow->text().toInt();
    if (temp < modSilentMin) {
        return;
    }
    modSilentLen = temp - modSilentMin;
    modulateIsRunning = true;
    timeout();
}
*/
