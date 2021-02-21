#ifndef NOISE_CONTROL_H
#define NOISE_CONTROL_H

#include "stdio.h"


#include <QMainWindow>
#include "QTimer"
#include "QTime"
#include "QFile"
#include "QTextStream"
#include "QVector"

#include "channelcontrol.h"
#include "hidInterface.h"
#include "generalprotocol.h"
#include "protocolthread.h"


namespace Ui {
class noiseControl;
}

class noiseControl : public QMainWindow
{
    Q_OBJECT

public:
    explicit noiseControl(QWidget *parent = nullptr);
    ~noiseControl();

private:
    generalProtocol *protocol;
    protocolThread  *protocolThreadH;
    channelControl  *voltageGraph1;
    Ui::noiseControl *ui;
    hidInterface *hid;

    uint32_t dataBuffCnt;
    uint8_t *dataBuff;
    uint32_t ignorePacketCnt;
    /**Vector for keep elements*/
    QVector<double> dataFrameX;
    QVector<double> dataFrameY;
    int32_t dataFrameCnt;
    bool isChannelRun;

private slots:
    void txData(QVector<uint8_t> txData);
    /*channel command processing*/
    void chStop(uint8_t channelIndex);
    void chStartClockWise(uint8_t channelIndex);
    void setRfCh(uint32_t rfCh);
    void setOutLevel(uint32_t outLeve);
    void on_pushButton_Open_clicked();
    void on_pushButton_Close_clicked();
};

#endif // noiseControl_H
