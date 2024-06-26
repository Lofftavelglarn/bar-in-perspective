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

    faces = {
        {0, 1, 2, 3}, // Задняя грань
        {4, 5, 6, 7}, // Передняя грань
        {0, 1, 5, 4}, // Нижняя грань
        {2, 3, 7, 6}, // Верхняя грань
        {0, 3, 7, 4}, // Левая грань
        {1, 2, 6, 5}  // Правая грань
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

QVector3D BarPerspective::getCameraPosition()
{
    double radius = 1.50;
    double theta = angle;
    double phi = 45;

    double radianTheta = theta * M_PI / 180.0;
    double radianPhi = phi * M_PI / 180.0;

    double x = radius * cos(radianTheta) * sin(radianPhi);
    double y = radius * sin(radianTheta) * sin(radianPhi);
    double z = radius * cos(radianPhi);

    return QVector3D(x, y, z);
}

void BarPerspective::drawBar(QPainter &painter)
{
    QVector<QPointF> projectedVertices;
    projectedVertices.reserve(vertices.size());

    QVector<QVector3D> rotatedVertices;
    rotatedVertices.reserve(vertices.size());

    for (int i = 0; i < vertices.size(); ++i) {
        QVector3D rotatedVertex = rotatePoint(vertices[i], angle, QVector3D(0, 1, 0));
        rotatedVertices.append(rotatedVertex);
        projectedVertices.append(projectPoint(rotatedVertex));
    }

    QVector3D cameraPosition = getCameraPosition();

    foreach (const auto& face, faces) {
        if (isFaceVisible(face, rotatedVertices, cameraPosition)) {
            for (int i = 0; i < 4; ++i) {
                int startIndex = face[i];
                int endIndex = face[(i + 1) % 4];
                painter.drawLine(projectedVertices[startIndex], projectedVertices[endIndex]);
            }
        }
    }
}

bool BarPerspective::isFaceVisible(const QVector<int>& face, const QVector<QVector3D>& rotatedVertices, const QVector3D& cameraPosition)
{
    QVector3D v1 = rotatedVertices[face[1]] - rotatedVertices[face[0]];
    QVector3D v2 = rotatedVertices[face[2]] - rotatedVertices[face[1]];

    QVector3D normal = QVector3D::crossProduct(v1, v2);

    QVector3D faceCenter = (rotatedVertices[face[0]] + rotatedVertices[face[1]] + rotatedVertices[face[2]] + rotatedVertices[face[3]]) / 4.0;

    QVector3D viewDirection = cameraPosition - faceCenter;

    viewDirection.normalize();

    return QVector3D::dotProduct(normal, viewDirection) < 0;
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
