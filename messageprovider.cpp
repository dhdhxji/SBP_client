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
    char buf[128];
    int size = read(buf);

    switch(buf[0])
    {
    case SIG_REQUEST_NAME:
    {
        qDebug() << "DEBUG: name request";
        emit sigNameRequest();
        break;
    }
    case SIG_SHARE_NAME:
    {
        qDebug() << "DEBUG: name share" << buf+2;
        buf[size] = '\0';
        QString name(buf+2);

        emit sigShareName(name);
        break;
    }

    case SIG_SET_CELL:
    {
        qDebug() << "DEBUG: set cell";
        GameArea::CellType t;
        qDebug() << "DEBUG: received cell type is : " << static_cast<int>(buf[4]);
        switch(buf[4])
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

        if(buf[3] == OWN)
            emit sigOwnCellUpdate(QPoint(buf[1], buf[2]), t);
        else if(buf[3] == ENEMY)
            emit sigEnemyCellUpdate(QPoint(buf[1], buf[2]), t);
        break;
    }

    case SIG_GAME_STARTED:
    {
        qDebug() << "DEBUG: game started";
        break;
    }

    case SIG_FIRE:
    {
        qDebug() << "DEBUG: fire";
        emit sigYouTurn();
        break;
    }

    case SIG_ERR_SHIPS_INCORRECT:
    {
        emit sigShipPlacementIncorrect();
        emit externalMsg(tr("Ship placement incorrect"));
        break;
    }

    case SIG_SHIP_PLACEMENT_OK:
    {
        emit sigShipPlacementOk();
        break;
    }

    case SIG_WIN:
    {
        emit sigWin();
        emit externalMsg("You win!");
        break;
    }

    case SIG_LOSE:
    {
        emit sigLose();
        emit externalMsg("You lose =(");
        break;
    }
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
    int len = _socket->readBufferSize();
    _socket->read(msgPtr, 128);
    return len;
}
