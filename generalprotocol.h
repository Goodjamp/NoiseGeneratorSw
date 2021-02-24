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

    typedef struct {
        uint8_t devCh;
        uint8_t rfCh;
    } SetRfChArg;

    typedef struct {
        uint8_t devCh;
        uint8_t level;
    } SetLevelArg;

    bool gpDecode(uint8_t data[], uint32_t size);
    void gpStopCommandTx(QVector<uint8_t> chDevList);
    void gpStartClockWiseCommandTx(QVector<uint8_t> chDevList);
    void gpStartContrClockWiseCommandTx(QVector<uint8_t> chDevList);
    void gpSetPosition(uint8_t channel, uint32_t pos);
    void gpSetRfCh(QVector<SetRfChArg> rfChList);
    void gpSetOutLevel(QVector<SetLevelArg> levelList);
signals:
    void gpDataRxSize8(QVector<uint8_t> Data);
    void gpDataRxSize16(QVector<uint16_t> Data);
    void gpDataRxSize32(QVector<uint32_t> Data);
    void gpDataRxSizeDouble(QVector<double> Data);
    void gpSend(QVector<uint8_t> commandBuff);
};

#endif // GENERALPROTOCOL_H
