//#include "QObject"
#include "QTime"
#include "QDebug"

#include "protocolthread.h"
#include "generalprotocol.h"
#include "hidInterface.h"

protocolThread::protocolThread(hidInterface *interfaceH, generalProtocol *protocolH)
{
    interfaceHandler = interfaceH;
    protocol         = protocolH;

    /**Test code**/
    rxTime = new QTime();
}

#define READ_SIZE 64
#define CH_CNT    7
void protocolThread::run() {
    uint32_t numRead;
    uint8_t  rxHIDBuff[READ_SIZE];
    int rxdT;
    while(true) {
        if((numRead = interfaceHandler->read(rxHIDBuff, sizeof(rxHIDBuff), 100)) == 0){
            continue;
        }
        rxdT = rxTime->restart();
        if(rxdT >= 15) {
            qDebug()<<"Rx interval = "<<rxdT;
        }
        protocol->gpDecode(rxHIDBuff, numRead);
    }
}
