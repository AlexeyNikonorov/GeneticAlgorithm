#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <vector>
#include <random>
#include <algorithm>
#include <iostream>

using namespace std;

enum {
	EMPTY = 1,
	FOOD = 2,
	BOT = 3,
	WALL = 4,
	MOVE = 1,
	GRASP = 2,
	LOOKUP = 3,
	TURN = 4,
	JUMP = 5,
};

struct Point {
	int x, y;
	Point() {}
	Point(int X, int Y) : x(X), y(Y) {}
	Point operator+(const Point& p) {return Point(x+p.x,y+p.y);}
};

struct Field {
	vector<int> data;
	int xsize, ysize;
	Field() {}
	Field(int x, int y) : data(x * y), xsize(x), ysize(y) {}
	int& at(int idx) {return data[idx];}
	int& at(int x, int y) {return at(idx(x, y));}
	int& at(const Point& p) {return at(p.x, p.y);}
	int get(const Point& p) {return in_range(p) ? at(p) : WALL;}
	int idx(int x, int y) {return y * xsize + x;}
	bool in_range(const Point& p) {return in(p.x, xsize) && in(p.y, ysize);}
	static bool in(int a, int d) {return 0 <= a && a < d;}
};

struct PolarMatrix {
	vector<Point> data;
	PolarMatrix() {}
	PolarMatrix(int rmax) {extend(rmax);}
	void extend(int rmax) {
		data.resize(cover(rmax));
		fill(rmax);
	}
	void fill(int rmax) {
		int i, idx = 0;
		for (int r = 1; r <= rmax; r++) {
			for (i = -r; i <= r; i++)
				data[idx++] = Point(i, r);
			for (i = r - 1; i > -r; i--)
				data[idx++] = Point(r, i);
			for (i = r; i >= -r; i--)
				data[idx++] = Point(i, -r);
			for (i = -r + 1; i < r; i++)
				data[idx++] = Point(-r, i);
		}
	}
	const Point& at(int r, int angle) const {return data[idx(r, angle)];}
	const Point& at(int r, int angle, int d) const {return at(r, (angle+d*r)%(8*r));}
	static int idx(int r, int angle) {return cover(r - 1) + angle;}
	static int cover(int r) {return 4 * r * (r + 1);}
};

struct Cmd {
	int type, r, val;
	Cmd(int t=0, int R=0, int v=0) : type(t), r(R), val(v) {}
};

struct CmdTable : public vector<Cmd> {
	void add(int type, int r, int span, int start=0) {
		for (int i = start; i < span + start; i++)
			push_back(Cmd(type, r, i));
	}
};

#endif
