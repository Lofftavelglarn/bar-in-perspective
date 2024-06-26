#ifndef BARPERSPECTIVE_H
#define BARPERSPECTIVE_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPainter>
#include <QTimer>
#include <QVector3D>
#include <cmath>

class BarPerspective : public QWidget
{
    Q_OBJECT

public:
    BarPerspective(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateAnimation();

private:
    void drawBar(QPainter &painter);
    QPointF projectPoint(const QVector3D &point);
    QVector3D rotatePoint(const QVector3D &point, double angle, const QVector3D &axis);
    bool isFaceVisible(const QVector<int>& face, const QVector<QVector3D>& rotatedVertices, const QVector3D& cameraPosition);
    QVector3D getCameraPosition();
    bool isVertexVisible(const QVector3D& vertex, const QVector3D& cameraPosition, QPainter& painter);

    QTimer *timer;
    QElapsedTimer elapsedTimer;
    double angle;
    bool direction; // true для вращения влево, false для вращения вправо
    QVector<QVector3D> vertices;
    QVector<QVector<int>> faces;
};

#endif // BARPERSPECTIVE_H
