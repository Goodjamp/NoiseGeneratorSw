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

#define CHANNEL_CNT    5

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
    typedef enum {
       OPEN_DEVICE,
       CLOSE_DEVICE,
       STOP,
       RUN,
       RUN_MODULATE,
    } ActionType;
    typedef enum {
       MODULATE_HIGH,
       MODULATE_LOW,
    } ModulateState;


    generalProtocol *protocol;
    protocolThread  *protocolThreadH;
    QVector<channelControl*>  channelList;
    Ui::noiseControl *ui;
    hidInterface *hid;

    uint32_t dataBuffCnt;
    uint8_t *dataBuff;
    uint32_t ignorePacketCnt;
    /**Vector for keep elements*/
    QVector<double> dataFrameX;
    QVector<double> dataFrameY;
    int32_t dataFrameCnt;
    QTimer *modulateControlTimer;
    QVector<uint8_t> runDevChList;
    int modNoiseMin;
    int modNoiseLen;
    int modSilentMin;
    int modSilentLen;
    bool modulateIsRunning;
    ModulateState modulateState;

    void uiActiveUpdate(ActionType actionType);

private slots:
    void txData(QVector<uint8_t> txData);
    /*channel command processing*/
    void on_pushButton_Open_clicked();
    void on_pushButton_Close_clicked();
    void on_pushButtonSetChannel_clicked();
    void on_pushButtonSetLevel_clicked();
    void on_pushButtonRun_clicked();
    void on_pushButtonRunMod_clicked();
    void on_pushButtonStop_clicked();
    void timeout();
    void on_pushButtonAllCh_clicked();
    void on_pushButton_clicked();
};

#endif // noiseControl_H
