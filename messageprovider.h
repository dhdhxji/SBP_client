#ifndef MESSAGEPROVIDER_H
#define MESSAGEPROVIDER_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QPoint>

#include "../sea_battle/shared_include/signals.h"
#include "../sea_battle/shared_include/cell_state_defines.h"
#include "gamearea.h"


class MessageProvider : public QObject
{
    Q_OBJECT
public:
    explicit MessageProvider(QObject *parent = nullptr);

signals:
    void sigOwnCellUpdate(QPoint, GameArea::CellType type);
    void sigEnemyCellUpdate(QPoint, GameArea::CellType type);
    void sigNameRequest();
    void sigShareName(const QString& name);
    void sigGameStarted();
    void sigYouTurn();
    void sigShipPlacementIncorrect();
    void sigShipPlacementOk();
    void sigWin();
    void sigLose();

    void connected();
    void disconnected();

    void externalMsg(const QString& externalMsg);

public slots:
    void connectToSrv(QString address, qint16 port);
    void disconnectFromSrv();

    void setShip(QPoint point);
    void ready();
    void shot(QPoint point);

    void nameResponse(QString name);

private slots:
    void processMessage();

    void socketError(QAbstractSocket::SocketError err);

private:
    QTcpSocket* _socket;
    QDataStream _stream;

    void write(char* msg, int len);
    int read(char* msgPtr);
};

#endif // MESSAGEPROVIDER_H
