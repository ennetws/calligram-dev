#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>

namespace Ui {
class Viewer;
}

class Viewer : public QWidget
{
    Q_OBJECT

public:
    explicit Viewer(QWidget *parent = 0);
    ~Viewer();

	void paintEvent(QPaintEvent *);
	
	void keyPressEvent(QKeyEvent *);
	
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);

    void drawLetters();

	bool scribbling;
	QPoint lastPoint;
	QVector<QPolygonF> paths;
    QVector<QPainterPath> contours;
    QVector<qreal> angles;
    QVector<qreal> lengths;

	QString message;
    int padding;
    QRectF inputRect;

private:
    Ui::Viewer *ui;
};

#endif // VIEWER_H
