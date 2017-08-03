#ifndef GUI_HPP
#define GUI_HPP

#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QPaintEvent>
#include <QGridLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QString>
#include <QBasicTimer>
#include <QThread>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include "../gen.hpp"

class Worker : public QObject {
	Q_OBJECT;
	QBasicTimer timer;
	Env *env;
public:
	Worker(Env *e);
	void timerEvent(QTimerEvent*);
public slots:
	void start();
signals:
	void newGeneration(int gen, int age);
};

class Square : public QWidget {
	Q_OBJECT;
public:
	QColor color;
	void paintEvent(QPaintEvent*);
};

class ToggleButton : public QPushButton {
	Q_OBJECT;
	QString text[2];
public:
	ToggleButton(const QString& t1, const QString& t2, QWidget *parent=0);
private slots:
	void onclick(bool checked);
};

class Board : public QWidget, public Env {
	Q_OBJECT;
public:
	Square *squares[envx][envy];
	QBasicTimer *timer;
	QGridLayout *grid;
	Board(QWidget *parent=0);
	void paintEvent(QPaintEvent*);
	void timerEvent(QTimerEvent*);
public slots:
	void start();
};

class ControlPanel : public QWidget {
	Q_OBJECT;
public:
	ToggleButton *b;
	QLabel *l[4];
	QLineEdit *le[6];
	ControlPanel(QWidget *parent=0);
	bool hasValue(int idx);
	double getValue(int idx);
	void setValue(int idx, double value);
	void setOutput(int idx, const QString& str);
};

class Window : public QWidget {
	Q_OBJECT;
	Board *b;
	ControlPanel *cp;
	QThread thread;
public:
	Window();
	~Window();
	void setSize();
public slots:
	void onapply();
	void onclick();
	void onupdate(int gen, int age);
};

#endif
