#include "gameclient.h"

GameClient::GameClient(QWidget *parent) : QWidget(parent)
{
    _ipLabel = new QLabel(tr("Server ip address : "), this);
    _ipLine = new QLineEdit(this);


    QString Octet = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipReg("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$");
    _ipLine->setValidator(new QRegExpValidator(ipReg));

    _usernameLabel = new QLabel(tr("Your name : "), this);
    _usernameLine = new QLineEdit(this);

    _connectBtn = new QPushButton(tr("Connect"), this);
    _disconnectBtn = new QPushButton(tr("Disconnect"), this);

    _player1Lbl = new QLabel(tr("Your fleet : "), this);
    _player2Lbl = new QLabel(tr("Enemy fleet : "), this);

    _player1Name = new QLabel(this);
    _player2Name = new QLabel(this);

    _player1Area = new GameArea(this);
    _player2Area = new GameArea(this);

    _readyButton = new QPushButton(tr("Ready"));

    _connected = new QLabel(tr("disconnected"), this);
    _lastMessage = new QLabel(this);

    QWidget* statusWidget = new QWidget;
    QHBoxLayout* statusLay = new QHBoxLayout(statusWidget);
    statusLay->addWidget(_lastMessage);
    QSpacerItem* spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding);
    statusLay->addSpacerItem(spacer);
    statusLay->addWidget(_connected);

    _status = new QStatusBar;
    _status->addWidget(statusWidget);



    QHBoxLayout* ipLayout = new QHBoxLayout;
    ipLayout->addWidget(_ipLabel);
    ipLayout->addWidget(_ipLine);
    ipLayout->addWidget(_connectBtn);
    ipLayout->addWidget(_disconnectBtn);

    QHBoxLayout* nameLayout = new QHBoxLayout;
    nameLayout->addWidget(_usernameLabel);
    nameLayout->addWidget(_usernameLine);

    QHBoxLayout* name1DescriptionL = new QHBoxLayout;
    name1DescriptionL->addWidget(_player1Lbl);
    name1DescriptionL->addWidget(_player1Name);

    QVBoxLayout* game1Field = new QVBoxLayout;
    game1Field->addLayout(name1DescriptionL);
    game1Field->addWidget(_player1Area);

    QHBoxLayout* name2DescriptionL = new QHBoxLayout;
    name2DescriptionL->addWidget(_player2Lbl);
    name2DescriptionL->addWidget(_player2Name);

    QVBoxLayout* game2Field = new QVBoxLayout;
    game2Field->addLayout(name2DescriptionL);
    game2Field->addWidget(_player2Area);

    QHBoxLayout* area = new QHBoxLayout;
    area->addLayout(game1Field);
    area->addLayout(game2Field);




    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(nameLayout);
    mainLayout->addLayout(ipLayout);

    mainLayout->addLayout(area);
    mainLayout->addWidget(_readyButton);
    mainLayout->addWidget(_status);


    //message provider for signals transfer
    _transciever = new MessageProvider(this);
    connect(_connectBtn, &QPushButton::clicked, _transciever,
            [=](){emit _transciever->connectToSrv(this->_ipLine->text(), 5555);});
    connect(_disconnectBtn, &QPushButton::clicked, _transciever,
            &MessageProvider::disconnectFromSrv);

    //updates from server
    connect(_transciever, &MessageProvider::sigOwnCellUpdate,
            _player1Area, &GameArea::setCell);
    connect(_transciever, &MessageProvider::sigEnemyCellUpdate,
        _player2Area, &GameArea::setCell);

    //request processing
    connect(_transciever, &MessageProvider::sigNameRequest,
            [=](){_transciever->nameResponse(_usernameLine->text());});

    //share name signal
    connect(_transciever, &MessageProvider::sigShareName,
            _player2Name, &QLabel::setText);

    //informations signals
    connect(_transciever, &MessageProvider::sigGameStarted,
            [=](){_lastMessage->setText(tr("Game started"));});
    connect(_transciever, &MessageProvider::sigYouTurn,
            [=](){_lastMessage->setText(tr("Your turn"));});

    //connect indicator
    connect(_transciever, &MessageProvider::connected,
            [=](){_connected->setText(tr("connected"));});
    connect(_transciever, &MessageProvider::disconnected,
            [=](){_connected->setText("disconnected");});
    connect(_transciever, &MessageProvider::disconnected,
            [=](){_readyButton->setEnabled(true);});
    connect(_transciever, &MessageProvider::disconnected,
            [=](){_player2Name->setText("");});

    connect(_transciever, &MessageProvider::connected,
            _player1Area, &GameArea::clear);
    connect(_transciever, &MessageProvider::disconnected,
            _player1Area, &GameArea::clear);
    connect(_transciever, &MessageProvider::connected,
            _player2Area, &GameArea::clear);
    connect(_transciever, &MessageProvider::disconnected,
            _player2Area, &GameArea::clear);

    //client feedback
    connect(_transciever, &MessageProvider::externalMsg,
            _lastMessage, &QLabel::setText);

    //game shot and setup output signals
    connect(_player1Area, &GameArea::cellClicked,
            _transciever, &MessageProvider::setShip);
    connect(_player2Area, &GameArea::cellClicked,
            _transciever, &MessageProvider::shot);

    //signals responsible for area setup
    connect(_transciever, &MessageProvider::sigShipPlacementOk,
            [=](){_readyButton->setDisabled(true);});
    connect(_transciever, &MessageProvider::sigWin,
            [=](){_readyButton->setEnabled(true);});
    connect(_transciever, &MessageProvider::sigLose,
            [=](){_readyButton->setEnabled(true);});
    connect(_readyButton, &QPushButton::clicked,
            _transciever, &MessageProvider::ready);
    connect(_transciever, &MessageProvider::sigNotReady,
            [=](){emit _readyButton->setEnabled(true);});


    setLayout(mainLayout);
}



GameClient::~GameClient()
{}
