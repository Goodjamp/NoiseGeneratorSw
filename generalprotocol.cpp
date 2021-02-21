#include "QObject"
#include "QVector"
#include "QDebug"
#include "QMutex"
#include "generalprotocol.h"
#include "stdint.h"
#include "string.h"

#define BUFF_SIZE 64
#define CHANNEL_CNT 0x7
#define CHANNEL_ALL 0xFF

typedef enum {
    COMMAND_DATA_FLAG_8_BIT_SIZE  = 0,
    COMMAND_DATA_FLAG_16_BIT_SIZE = 1,
    COMMAND_DATA_FLAG_32_BIT_SIZE = 2,
    COMMAND_DATA_FLAG_SIZE_MASK   = 3,
}COMMAND_DATA_FLAG;

typedef enum{
    GP_DATA                   = 0,
    GP_STOP                   = 1,
    GP_START_CLOCK_WISE       = 2,
    GP_START_CONTR_CLOCK_WISE = 3,
    GP_START_AUTO_SWITCHER    = 4,
    GP_SET_POS                = 5,
    GP_SET_RF_CH              = 6,
    GP_SET_OUT_LEVEL          = 7,
} GP_COMMAND;

#pragma pack(push, 1)
#define PROTOCOL_BUFF_SIZE (BUFF_SIZE - 1)
typedef struct {
    uint8_t  channel;
    uint8_t  flags;
    uint16_t cnt;
    uint8_t  data[];
}GpDataSubcommand;

typedef struct {
    uint8_t  channel;
}GpStartClockWiseSubcommand;

typedef struct {
    uint8_t  channel;
}GpStartContrClockWiseSubcommand;

typedef struct {
    uint8_t  channel;
}GpStoptSubcommand;

typedef struct {
    uint8_t  channel;
    uint16_t offTime;
    uint16_t onTime;
    uint32_t cnt;
}GpStartAutoSwitcher;

typedef struct {
    uint8_t  channel;
    uint32_t position;
}GpSetPos;

typedef struct {
    uint8_t rfChannel;
}GpSeRfCh;

typedef struct {
    uint8_t outLevel;
}GpSetOutLevel;

typedef struct GpCommand{
    uint8_t headr;
    union {
        GpDataSubcommand                dataSubComand;
        GpStartClockWiseSubcommand      startClockWiseSubcommand;
        GpStartContrClockWiseSubcommand startContrClockWiseSubcommand;
        GpStoptSubcommand               stoptSubcommand;
        GpStartAutoSwitcher             startAutoSwitcherSubcommand;
        GpSetPos                        gpSetPos;
        GpSeRfCh                        seRfCh;
        GpSetOutLevel                   setOutLevel;
        uint8_t buffSubComand[PROTOCOL_BUFF_SIZE];
    };
} GpCommand;

typedef union {
    GpCommand command;
    uint8_t buff[sizeof(GpCommand)];
} GpCommandBuff ;
#pragma pack(pop)

generalProtocol::generalProtocol()
{

}

generalProtocol::~generalProtocol()
{
    //
}

bool generalProtocol::gpDecode(uint8_t data[], uint32_t size)
{
    GpCommandBuff gpCommandBuff;
    memcpy((gpCommandBuff.buff), static_cast<uint8_t*>(data), size);
    switch(gpCommandBuff.command.headr) {
        case GP_DATA:
            uint8_t sizeFlag = gpCommandBuff.command.dataSubComand.flags & COMMAND_DATA_FLAG_SIZE_MASK;
            QVector<uint8_t>  dataSize8;
            switch(sizeFlag) {
            case COMMAND_DATA_FLAG_8_BIT_SIZE: {
                    QVector<uint8_t>  dataSize8(gpCommandBuff.command.dataSubComand.cnt / sizeof(uint8_t));
                    uint8_t *rxType = (uint8_t*)(gpCommandBuff.command.dataSubComand.data);
                    for(uint16_t k = 0; k < gpCommandBuff.command.dataSubComand.cnt; k += sizeof(int8_t)) {
                        dataSize8[k / sizeof(uint8_t)] = (*rxType++);
                    }
                    emit gpDataRxSize8(dataSize8);
                }
                break;
            case COMMAND_DATA_FLAG_16_BIT_SIZE: {
                QVector<uint16_t> dataSize16(gpCommandBuff.command.dataSubComand.cnt / sizeof(uint16_t));
                //QVector<double> dataSizeDouble(gpCommandBuff.command.dataSubComand.cnt / sizeof(uint16_t));
                uint16_t *rxType = (uint16_t*)gpCommandBuff.command.dataSubComand.data;
                for(uint16_t k = 0; k < gpCommandBuff.command.dataSubComand.cnt; k += sizeof(int16_t)) {
                    dataSize16[k / sizeof(uint16_t)] = (*rxType++);
                }
                emit gpDataRxSize16(dataSize16);
            }
                break;
/*
            case COMMAND_DATA_FLAG_32_BIT_SIZE:{
                QVector<double> dataSizeDouble(gpCommandBuff.command.dataSubComand.cnt / sizeof(uint32_t));
                uint32_t *rxType = (uint32_t*)(gpCommandBuff.command.dataSubComand.data);
                for(uint16_t k = 0; k < gpCommandBuff.command.dataSubComand.cnt; k += sizeof(int32_t)) {
                    dataSize32[k / sizeof(uint32_t)] = (*rxType++);
                }
                emit gpDataRxSize32(dataSize32);
            }
                break;
            }
*/
            case COMMAND_DATA_FLAG_32_BIT_SIZE:{
                QVector<double> dataSizeDouble(gpCommandBuff.command.dataSubComand.cnt / sizeof(uint32_t));
                uint32_t *rxType = (uint32_t*)(gpCommandBuff.command.dataSubComand.data);
                for(uint16_t k = 0; k < gpCommandBuff.command.dataSubComand.cnt; k += sizeof(int32_t)) {
                    dataSizeDouble[k / sizeof(uint32_t)] = static_cast<double>(*rxType++);
                }
                emit gpDataRxSizeDouble(dataSizeDouble);
            }
                break;
            }
            break;
    };
    return true;
}

void generalProtocol::gpStopCommandTx(uint8_t channel)
{
    QVector<uint8_t> gpCommandV;
    gpCommandV.resize(sizeof(GpCommand));
    GpCommand *gpCommand = (GpCommand*)(gpCommandV.data());
    gpCommand->headr = GP_STOP;
    gpCommand->stoptSubcommand.channel = static_cast<uint8_t>(channel);
    emit gpSend(gpCommandV);
}

void generalProtocol::gpStartClockWiseCommandTx(uint8_t channel)
{
    QVector<uint8_t> gpCommandV;
    gpCommandV.resize(sizeof(GpCommand));
    GpCommand *gpCommand = (GpCommand*)(gpCommandV.data());
    gpCommand->headr = GP_START_CLOCK_WISE;
    gpCommand->stoptSubcommand.channel = static_cast<uint8_t>(channel);
    emit gpSend(gpCommandV);
}

void generalProtocol::gpStartContrClockWiseCommandTx(uint8_t channel)
{
    QVector<uint8_t> gpCommandV;
    gpCommandV.resize(sizeof(GpCommand));
    GpCommand *gpCommand = (GpCommand*)(gpCommandV.data());
    gpCommand->headr = GP_START_CONTR_CLOCK_WISE;
    gpCommand->stoptSubcommand.channel = static_cast<uint8_t>(channel);
    emit gpSend(gpCommandV);
}

void generalProtocol::gpSetPosition(uint8_t channel, uint32_t position)
{
    QVector<uint8_t> gpCommandV;
    gpCommandV.resize(sizeof(GpCommand));
    GpCommand *gpCommand = (GpCommand*)(gpCommandV.data());
    gpCommand->headr = GP_SET_POS;
    gpCommand->gpSetPos.channel = static_cast<uint8_t>(channel);
    gpCommand->gpSetPos.position = static_cast<uint32_t>(position);
    emit gpSend(gpCommandV);
}

void generalProtocol::gpSetRfCh(uint32_t rfCh)
{
    QVector<uint8_t> gpCommandV;
    gpCommandV.resize(sizeof(GpCommand));
    GpCommand *gpCommand = (GpCommand*)(gpCommandV.data());
    gpCommand->headr = GP_SET_RF_CH;
    gpCommand->seRfCh.rfChannel = static_cast<uint8_t>(rfCh);
    emit gpSend(gpCommandV);
}

void generalProtocol::gpSetOutLevel(uint32_t outLevel)
{
    QVector<uint8_t> gpCommandV;
    gpCommandV.resize(sizeof(GpCommand));
    GpCommand *gpCommand = reinterpret_cast<GpCommand*>(gpCommandV.data());
    gpCommand->headr = GP_SET_OUT_LEVEL;
    gpCommand->setOutLevel.outLevel = static_cast<uint8_t>(outLevel);
    emit gpSend(gpCommandV);
}
