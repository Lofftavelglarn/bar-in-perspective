#include "barperspective.h"

BarPerspective::BarPerspective(QWidget *parent)
    : QWidget(parent), angle(0), direction(true)
{
    setFixedSize(800, 600);

    vertices = {
        QVector3D(-1.5, -0.5, -0.5),
        QVector3D( 1.5, -0.5, -0.5),
        QVector3D( 1.5,  0.5, -0.5),
        QVector3D(-1.5,  0.5, -0.5),
        QVector3D(-1.5, -0.5,  0.5),
        QVector3D( 1.5, -0.5,  0.5),
        QVector3D( 1.5,  0.5,  0.5),
        QVector3D(-1.5,  0.5,  0.5)
    };

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &BarPerspective::updateAnimation);
    timer->start(7);

    elapsedTimer.start();
}

void BarPerspective::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(width() / 2, height() / 2);

    drawBar(painter);
}

void BarPerspective::updateAnimation()
{
    qint64 elapsed = elapsedTimer.elapsed();
    elapsedTimer.restart();

    double deltaAngle = 180.0 * (elapsed / 1000.0) * (1.0 / 5.0);

    if (direction) {
        angle += deltaAngle;
        if (angle >= 180.0) {
            angle = 180.0;
            direction = false;
        }
    } else {
        angle -= deltaAngle;
        if (angle <= -180.0) {
            angle = -180.0;
            direction = true;
        }
    }

    update();
}

void BarPerspective::drawBar(QPainter &painter)
{
    QVector<QPointF> projectedVertices;
    projectedVertices.reserve(vertices.size());
    for (int i = 0; i < vertices.size(); ++i) {
        QVector3D rotatedVertex = rotatePoint(vertices[i], angle, QVector3D(0, 1, 0));
        projectedVertices.append(projectPoint(rotatedVertex));
    }

    const int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    for (int i = 0; i < 12; ++i) {
        if (edges[i][0] == 1 && edges[i][1] == 2) {
            QPen redPen(Qt::red);
            painter.setPen(redPen);
        } else {
            QPen blackPen(Qt::black);
            painter.setPen(blackPen);
        }
        painter.drawLine(projectedVertices[edges[i][0]], projectedVertices[edges[i][1]]);
    }
}

QPointF BarPerspective::projectPoint(const QVector3D &point)
{
    double d = 2.0; // расстояние до проекционной плоскости

    double x = point.x() / (1 - point.z() / d);
    double y = point.y() / (1 - point.z() / d);

    return QPointF(x * 100, y * 100);
}

QVector3D BarPerspective::rotatePoint(const QVector3D &point, double angle, const QVector3D &axis)
{
    double radian = angle * M_PI / 180.0;
    double cosA = cos(radian);
    double sinA = sin(radian);
    QVector3D rotated;

    if (axis == QVector3D(1, 0, 0)) { // вращение вокруг оси X
        rotated.setX(point.x());
        rotated.setY(point.y() * cosA - point.z() * sinA);
        rotated.setZ(point.y() * sinA + point.z() * cosA);
    } else if (axis == QVector3D(0, 1, 0)) { // вращение вокруг оси Y
        rotated.setX(point.x() * cosA + point.z() * sinA);
        rotated.setY(point.y());
        rotated.setZ(-point.x() * sinA + point.z() * cosA);
    } else if (axis == QVector3D(0, 0, 1)) { // вращение вокруг оси Z
        rotated.setX(point.x() * cosA - point.y() * sinA);
        rotated.setY(point.x() * sinA + point.y() * cosA);
        rotated.setZ(point.z());
    }

    return rotated;
}
