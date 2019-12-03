#include "gamearea.h"

GameArea::GameArea(QWidget *parent)
    : QWidget(parent)
{
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 10; ++x) {
            _area[y][x] = Sea;
        }
    }

    _seaMap = QPixmap(":/textures/sea.png");
    _shotSeaMap = QPixmap(":/textures/sea_shot.png");
    _shipMap = QPixmap(":/textures/ship.png");
    _shotShipMap = QPixmap(":/textures/ship_wounded.png");
    _killedShipMap = QPixmap(":/textures/ship_killed.png");

    QSizePolicy pol;
    pol.setHorizontalPolicy(QSizePolicy::Expanding);
    pol.setVerticalPolicy(QSizePolicy::Expanding);
    setSizePolicy(pol);
}

GameArea::~GameArea()
{
}

QSize GameArea::minimumSizeHint() const
{
    return QSize(200, 200);
}

QSize GameArea::sizeHint() const
{
    return QSize(300, 300);
}


void GameArea::setCell(QPoint point, CellType type)
{
    _area[point.ry()][point.rx()] = type;
    update();
}

void GameArea::clear()
{
    for(int y = 0; y < 10; ++y)
    {
        for(int x = 0; x < 10; ++x)
            _area[y][x] = Sea;
    }

    update();
}

void GameArea::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    QRectF source(0, 0, 100, 100);

    qreal cellW = qreal(width())/10;
    qreal cellH = qreal(height())/10;

    for(int y = 0; y < 10; ++y)
    {
        for(int x = 0; x < 10; ++x)
        {
            //draw battle area
            QRectF target(cellW*x, cellH*y, cellW, cellH);

            switch(_area[y][x])
            {
                case CellType::Sea:
                {
                    painter.drawPixmap(target, _seaMap, source);
                    break;
                }

                case CellType::Ship:
                {
                    painter.drawPixmap(target, _shipMap, source);
                    break;
                }

                case CellType::ShotSea:
                {
                    painter.drawPixmap(target, _shotSeaMap, source);
                    break;
                }

                case CellType::ShotShip:
                {
                    painter.drawPixmap(target, _shotShipMap, source);
                    break;
                }

                case CellType::KilledShip:
                {
                    painter.drawPixmap(target, _killedShipMap, source);
                    break;
                }
            }
        }
    }

    QPen pen(Qt::green, 2);
    painter.setPen(pen);

    for(int i = 1; i < 10; ++i)
    {
        qreal xCoordinate = i * cellW;
        qreal yCoordinate = i * cellH;

        painter.drawLine(QPointF(xCoordinate, 0), QPointF(xCoordinate, height()));
        painter.drawLine(QPointF(0, yCoordinate), QPointF(width(), yCoordinate));
    }
}


void GameArea:: mousePressEvent(QMouseEvent *event)
{
    QPoint pt = event->pos();
    pt.setX(pt.x() * 10 / width());
    pt.setY(pt.y() * 10 / height());

    qDebug() << "mouse click" << pt.x() << pt.y();
    emit cellClicked(pt);
}
