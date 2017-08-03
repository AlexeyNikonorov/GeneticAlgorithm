#include "gui.hpp"

Worker::Worker(Env *e) : QObject(), env(e) {}

void Worker::timerEvent(QTimerEvent*) {
	for (int i = 0; i < 1000; i++)
		if (int g = env->time_step())
			emit newGeneration(g, env->last_avg_age);
}

void Worker::start() {
	timer.start(0, this);
}

void Square::paintEvent(QPaintEvent*) {
	QRect rect(0, 0, width(), height());
	QPainter painter(this);
	painter.setPen(QColor(40,40,40));
	painter.setBrush(color);
	painter.drawRect(rect);
}

ToggleButton::ToggleButton(const QString& t1,
						   const QString& t2,
						   QWidget *parent)
						   : QPushButton(parent) {
	text[0] = t1;
	text[1] = t2;
	setText(t1);
	setCheckable(true);
	connect(this, SIGNAL(clicked(bool)), this, SLOT(onclick(bool)));
}

void ToggleButton::onclick(bool checked) {
	setText(text[checked]);
}

Board::Board(QWidget *parent) : QWidget(parent), Env() {
	timer = new QBasicTimer;
	grid = new QGridLayout(this);
	grid->setSpacing(0);
	for (int x = 0; x < f.xsize; x++) {
		for (int y = 0; y < f.ysize; y++) {
			Square *square = new Square;
			grid->addWidget(square, x, y);
			squares[x][y] = square;
		}
	}
}

void Board::paintEvent(QPaintEvent*) {
	static const QColor colors[] = {
		QColor(255,255,255),
		QColor(50,50,50), QColor(0,200,0),
		QColor(0,0,200), QColor(0,0,0),
	};
	
	for (int x = 0; x < f.xsize; x++)
		for (int y = 0; y < f.ysize; y++)
			squares[x][y]->color = colors[f.at(x, y)];
}

void Board::timerEvent(QTimerEvent*) {
	time_step();
	update();
}

void Board::start() {
	timer->start(100, this);
}

ControlPanel::ControlPanel(QWidget *parent) : QWidget(parent) {
	b = new ToggleButton("Start", "Stop");
	l[0] = new QLabel("Generation");
	l[1] = new QLabel("Age");
	l[2] = new QLabel;
	l[3] = new QLabel;
	for (int i = 0; i < 6; i++)
		le[i] = new QLineEdit;
	auto gl = new QGridLayout;
	gl->addWidget(l[0], 0, 0, 1, 1);
	gl->addWidget(l[1], 1, 0, 1, 1);
	gl->addWidget(l[2], 0, 1, 1, 1);
	gl->addWidget(l[3], 1, 1, 1, 1);
	gl->addWidget(new QLabel("Prog size"),    0, 2, 1, 1);
	gl->addWidget(new QLabel("Population"),   0, 4, 1, 1);
	gl->addWidget(new QLabel("Food balance"), 0, 6, 1, 1);
	gl->addWidget(new QLabel("Food bonus"),   1, 2, 1, 1);
	gl->addWidget(new QLabel("HP cap"),       1, 4, 1, 1);
	gl->addWidget(new QLabel("Lookup"),       1, 6, 1, 1);
	gl->addWidget(le[0], 0, 3, 1, 1);
	gl->addWidget(le[1], 0, 5, 1, 1);
	gl->addWidget(le[2], 0, 7, 1, 1);
	gl->addWidget(le[3], 1, 3, 1, 1);
	gl->addWidget(le[4], 1, 5, 1, 1);
	gl->addWidget(le[5], 1, 7, 1, 1);
	gl->addWidget(b , 2, 4, 1, 1);
	setLayout(gl);
}

double ControlPanel::getValue(int idx) {
	return le[idx]->text().toDouble();
}

void ControlPanel::setValue(int idx, double value) {
	le[idx]->setText(QString::number(value));
}

Window::Window() : QWidget() {
	int prog_size = 64, hp_cap = 1000, bonus = 1, lookup = 1;
	double population = 0.002, food = 0.1;
	
	auto gl = new QGridLayout;
	b = new Board;
	cp = new ControlPanel;
	b->set_parameters(prog_size, hp_cap, bonus, lookup, population, food);
	cp->setValue(0, prog_size);
	cp->setValue(1, population);
	cp->setValue(2, food);
	cp->setValue(3, bonus);
	cp->setValue(4, hp_cap);
	cp->setValue(5, lookup);
	gl->addWidget(b, 0, 0, 12, 12);
	gl->addWidget(cp, 13, 0, 2, 12);
	setLayout(gl);
	setSize();
	b->start();
	connect(cp->b, SIGNAL(clicked()), this, SLOT(onclick()));
}

Window::~Window() {
	thread.exit();
	thread.wait();
}

void Window::setSize() {
	QRect rect = QApplication::desktop()->availableGeometry();
	QPoint center = rect.center();
	int size = min(rect.width(), rect.height()) * 0.85;
	rect.setSize(QSize(size, size));
	rect.moveCenter(center);
	setGeometry(rect);
}

void Window::onapply() {
	b->set_prog_size(cp->getValue(0));
	b->set_bot_population(cp->getValue(1));
	b->set_food_balance(cp->getValue(2));
	b->set_food_bonus(cp->getValue(3));
	b->set_hp_cap(cp->getValue(4));
	b->set_lookup(cp->getValue(5));
}

void Window::onclick() {
	if (thread.isRunning()) {
		thread.exit();
		thread.wait();
		b->init();
		b->start();
	} else {
		onapply();
		b->timer->stop();
		auto worker = new Worker(b);
		worker->moveToThread(&thread);
		connect(&thread, SIGNAL(started()), worker, SLOT(start()));
		connect(&thread, SIGNAL(finished()), worker, SLOT(deleteLater()));
		connect(worker, SIGNAL(newGeneration(int,int)), this, SLOT(onupdate(int,int)));
		thread.start();
	}
}

void Window::onupdate(int gen, int avg) {
	cp->l[2]->setText(QString::number(gen));
	cp->l[3]->setText(QString::number(avg));
}
