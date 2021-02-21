#ifndef PROTOCOLTHREAD_H
#define PROTOCOLTHREAD_H

//#include <QObject>
#include <QTime>
#include <QThread>
#include "hidInterface.h"
#include "generalprotocol.h"

class protocolThread: public QThread
{
public:
    protocolThread(hidInterface *interfaceH, generalProtocol *protocolH);
    void run();

private:
    QTime           *rxTime;
    hidInterface    *interfaceHandler;
    generalProtocol *protocol;

};

#endif // PROTOCOLTHREAD_H
