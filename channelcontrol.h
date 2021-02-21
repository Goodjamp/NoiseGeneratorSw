#ifndef VOLTAGEGRAPH_H
#define VOLTAGEGRAPH_H

#include <QWidget>
#include <QVector>
#include <QTimer>

namespace Ui {
class channelControl;
}

class channelControl : public QWidget
{
    Q_OBJECT

public:
    explicit channelControl(char inChannelIndex = 0 , QWidget *parent = nullptr);
    ~channelControl();
    bool addPoint(double x, double y);

private:
    char channelIndex;
    double xCnt;
    QTimer *modulateControlTimer;
    int modNoiseMin;
    int modNoiseLen;
    int modSilentMin;
    int modSilentLen;
    bool modulateIsRunning;
    enum {
       modulateHigh,
       modulateLow,
    } modulateState;

private slots:
    void on_pushButtonSetConfig_clicked();

    void on_pushButtonStopNoise_clicked();

    void on_pushButtonSingleNoise_clicked();

    void on_pushButtonModulateNoise_clicked();

    void timeout();

private:
    Ui::channelControl *ui;

signals:
    void stop(char channelIndex);
    void startClockWise(char channelIndex);
    void setRfCh(uint32_t rfCh);
    void setOutLevel(uint32_t outLeve);
};

#endif // VOLTAGEGRAPH_H
