#ifndef VIEWER_H
#define VIEWER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QSharedPointer>
#include <QKeyEvent>
#include <QPolygon>
#include <QPainterPath>

namespace surface_mesh { class Mesh; }
namespace ShapeOp { class Solver; }

class Viewer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    Viewer();
    QSharedPointer<surface_mesh::Mesh> mesh;
	QSharedPointer<ShapeOp::Solver> solver;

	void visualizeScalar(const std::string property);
	void normalizeVertexProperty(const std::string property);

	QVector<int> pnts;

protected:
    void initializeGL();
    void paintGL();

	void keyPressEvent(QKeyEvent*e);
};

#endif // VIEWER_H
