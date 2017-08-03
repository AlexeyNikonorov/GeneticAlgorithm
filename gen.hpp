#ifndef GEN_HPP
#define GEN_HPP

#include "utility.hpp"
#include <vector>

const int envx = 100, envy = 100;

struct Bot {
	vector<int> prog;
	int age, hp, d;
	Point pos;
	
	Bot();
	void refresh();
	int inc_age();
	void inc_hp(int bonus);
	void turn(int angle);
	void set_prog(size_t size);
	int act(Field& f);
	void replace(Field& f, Point& p, int bonus=0);
	int grasp(Field& f, Point& p, int obj);
	int move(Field& f, Point& p, int obj);
};

struct Env {
	Field f;
	vector<Bot> bots;
	int generation;
	int last_avg_age;
	
	Env();
	int time_step();
	void set_food();
	void set_bots();
	void init();
	int avg_age();
	double bots_alive();
	
	void set_parameters(int prog_size, int hp_cap,
						int food_bonus, int lookup,
						double bot_population,
						double food_balance);
	void set_prog_size(int value);
	void set_bot_population(double value);
	void set_food_balance(double value);
	void set_food_bonus(int value);
	void set_hp_cap(int value);
	void set_lookup(int value);
};

#endif
