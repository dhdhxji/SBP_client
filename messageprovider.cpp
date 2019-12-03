#include "messageprovider.h"

MessageProvider::MessageProvider(QObject *parent) : QObject(parent)
{
    _socket = new QTcpSocket(this);

    connect(_socket, &QIODevice::readyRead, this, &MessageProvider::processMessage);
    connect(_socket, &QAbstractSocket::disconnected,
            [=](){emit this->disconnected();});
    connect(_socket, &QAbstractSocket::connected,
            [=](){emit this->connected();});
    connect(_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &MessageProvider::socketError);

    //_stream.setDevice(_socket);
    //_stream.setVersion(QDataStream::Qt_5_12);
}



void MessageProvider::connectToSrv(QString addr, qint16 port)
{
    _socket->connectToHost(addr, port);
}

void MessageProvider::disconnectFromSrv()
{
    _socket->disconnectFromHost();
}

void MessageProvider::setShip(QPoint point)
{
    char cmd[3];
    cmd[0] = SIG_SETUP_CELL;
    cmd[1] = point.x();
    cmd[2] = point.y();

    write(cmd, 3);
}

void MessageProvider::ready()
{
    char cmd[1];
    cmd[0] = SIG_READY;

    write(cmd, 1);
}

void MessageProvider::shot(QPoint point)
{
    char cmd[3];
    cmd[0] = SIG_SHOT;
    cmd[1] = point.x();
    cmd[2] = point.y();

    write(cmd, 3);
}

void MessageProvider::nameResponse(QString name)
{
    char cmdbuf[128];
    cmdbuf[0] = SIG_RESPONSE_NAME;
    cmdbuf[1] = char(name.length());

    for(int i = 0; i < name.length(); ++i)
    {
        cmdbuf[i+2] = name.toStdString()[i];
    }

    write(cmdbuf, 2+name.length());
}

void MessageProvider::processMessage()
{
    const int maxBufSize = 5*100*2; //5 bytes per cell repeated 100 times(full area)
    char buf[maxBufSize];
    int size = read(buf);

    char* command = buf;

        qDebug() << "received msg size: " << size;

    while(command < (buf+size))
    switch(*(command))
    {
    case SIG_REQUEST_NAME:
    {
        qDebug() << "DEBUG: name request";
        emit sigNameRequest();

        command+=1;
        break;
    }
    case SIG_SHARE_NAME:
    {
        qDebug() << "DEBUG: name share" << buf+2;

        int nameLen = command[1];
        int temp = command[2 + nameLen];
        command[2 + nameLen] = '\0';
        QString name(command+2);
        command[2 + nameLen] = temp;

        emit sigShareName(name);

        command += (2+nameLen);
        break;
    }

    case SIG_SET_CELL:
    {
        qDebug() << "DEBUG: set cell";
        GameArea::CellType t;
        qDebug() << "DEBUG: received cell type is : " <<
                 "X: " << static_cast<int>(command[1]) <<
                 " y: " << static_cast<int>(command[2]) <<
                 " wnemy: " << static_cast<int>(command[3]) <<
                 static_cast<int>(command[4]);
        switch(command[4])
        {
        case CELL_SEA:
        t = GameArea::Sea;
        break;

        case CELL_SHIP:
        t = GameArea::Ship;
        break;

        case CELL_SEA_SHOTED:
        t = GameArea::ShotSea;
        break;

        case CELL_SHIP_WOUNDED:
        t = GameArea::ShotShip;
        break;

        case CELL_SHIP_KILLED:
        t = GameArea::KilledShip;
        break;
        }

        if(command[3] == OWN)
            emit sigOwnCellUpdate(QPoint(command[1], command[2]), t);
        else if(command[3] == ENEMY)
            emit sigEnemyCellUpdate(QPoint(command[1], command[2]), t);

        command += 5;
        break;
    }

    case SIG_GAME_STARTED:
    {
        qDebug() << "DEBUG: game started";

        command += 1;
        break;
    }

    case SIG_FIRE:
    {
        qDebug() << "DEBUG: fire";
        emit sigYouTurn();

        command += 1;
        break;
    }

    case SIG_ERR_SHIPS_INCORRECT:
    {
        emit sigShipPlacementIncorrect();
        emit externalMsg(tr("Ship placement incorrect"));

        command += 1;
        break;
    }

    case SIG_SHIP_PLACEMENT_OK:
    {
        emit sigShipPlacementOk();

        command += 1;
        break;
    }

    case SIG_NOT_READY:
    {
        emit sigNotReady();
        command+=1;
        break;
    }

    case SIG_WIN:
    {
        emit sigWin();
        emit externalMsg("You win!");

        command += 1;
        break;
    }

    case SIG_LOSE:
    {
        emit sigLose();
        emit externalMsg("You lose =(");

        command += 1;
        break;
    }
    default: qDebug() << "DEBUG: Unknown command"; return;
    }
}


void MessageProvider::socketError(QAbstractSocket::SocketError err)
{
    switch (err) {

    case QAbstractSocket::HostNotFoundError:
        emit externalMsg(tr("The host was not found"));
        break;

    case QAbstractSocket::ConnectionRefusedError:
        emit externalMsg(tr("Connection was refused by destination host"));
        break;

    case QAbstractSocket::RemoteHostClosedError:
        emit externalMsg(tr("Host closed"));
        //emit disconnected();
        break;
    }

}

void MessageProvider::write(char* msg, int len)
{
    _socket->write(msg, len);
}

int MessageProvider::read(char* msgPtr)
{
    int len = _socket->bytesAvailable();
    _socket->read(msgPtr, 128);
    return len;
}
