#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QRegExpValidator>
#include <QLayout>
#include <QSizePolicy>
#include <QSpacerItem>
#include "gamearea.h"
#include "messageprovider.h"


class GameClient : public QWidget
{
    Q_OBJECT
public:
    explicit GameClient(QWidget *parent = nullptr);

    ~GameClient();

signals:

public slots:


private:

    MessageProvider* _transciever;

    GameArea* _player1Area;
    GameArea* _player2Area;

    QLabel* _ipLabel;
    QLineEdit* _ipLine;
    QLabel* _usernameLabel;
    QLineEdit* _usernameLine;

    QPushButton* _connectBtn;
    QPushButton* _disconnectBtn;

    QLabel* _player1Lbl;
    QLabel* _player2Lbl;

    QLabel* _player1Name;
    QLabel* _player2Name;

    QPushButton* _readyButton;

    QStatusBar* _status;
    QLabel* _connected;
    QLabel* _lastMessage;
};

#endif // GAMECLIENT_H
