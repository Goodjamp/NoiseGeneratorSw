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
    int getChannel();
    int getLevel();
    void getTiming(int *minHigh, int *maxHigh,
                   int *minLow, int *maxLow);
    bool isUse();
    void setUse();
    void clearUse();

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

private:
    Ui::channelControl *ui;
};

#endif // VOLTAGEGRAPH_H
