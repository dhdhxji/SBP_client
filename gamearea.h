#ifndef GAMEAREA_H
#define GAMEAREA_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

class GameArea : public QWidget
{
    Q_OBJECT

public:
    enum CellType { Sea, Ship, ShotSea, ShotShip, KilledShip };

    GameArea(QWidget *parent = nullptr);
    ~GameArea() override;

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;


signals:
    void cellClicked(QPoint point);


public slots:
    void setCell(QPoint point, CellType type);
    void clear();


protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;


private:
    CellType _area[10][10];
    QPixmap _seaMap;
    QPixmap _shipMap;
    QPixmap _shotShipMap;
    QPixmap _shotSeaMap;
    QPixmap _killedShipMap;

};
#endif // GAMEAREA_H
