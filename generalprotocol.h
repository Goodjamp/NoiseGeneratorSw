#ifndef JENERALPROTOCOL_H
#define JENERALPROTOCOL_H

#include "QObject"
#include "QWidget"
#include "QVector"
#include "QMutex"
#include "stdint.h"
#include "stdio.h"

class generalProtocol: public QObject
{
    Q_OBJECT

public:
    generalProtocol();
    ~generalProtocol();

    bool gpDecode(uint8_t data[], uint32_t size);
    void gpStopCommandTx(uint8_t channel);
    void gpStartClockWiseCommandTx(uint8_t channel);
    void gpStartContrClockWiseCommandTx(uint8_t channel);
    void gpSetPosition(uint8_t channel, uint32_t pos);
    void gpSetRfCh(uint32_t rfCh);
    void gpSetOutLevel(uint32_t outLevel);
signals:
    void gpDataRxSize8(QVector<uint8_t> Data);
    void gpDataRxSize16(QVector<uint16_t> Data);
    void gpDataRxSize32(QVector<uint32_t> Data);
    void gpDataRxSizeDouble(QVector<double> Data);
    void gpSend(QVector<uint8_t> commandBuff);
};

#endif // GENERALPROTOCOL_H
