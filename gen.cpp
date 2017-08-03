#include "utility.hpp"
#include "gen.hpp"

static int prog_size = 64;
static int hp_cap = 1000;
static int max_depth = 100;
static int food_bonus = 1;
static int food_balance, now_food;
static CmdTable cmd_table;
static PolarMatrix m;

#include "ga.hpp"

Bot::Bot() {}

void Bot::refresh() {
	age = 0;
	hp = hp_cap;
	d = 0;
}

int Bot::inc_age() {
	age++;
	return --hp;
}

void Bot::inc_hp(int bonus) {
	hp = min(hp + bonus, hp_cap);
}

void Bot::turn(int angle) {
	d = (d + angle) % 8;
}

void Bot::set_prog(size_t size) {
	prog.resize(size);
	generate(prog.begin(), prog.end(),
		[](){return rand() % cmd_table.size();});
}

int Bot::act(Field& f) {
	int idx = 0;
	
	for (int i = 0; i < max_depth; i++) {	
		if (hp == 0)
			return -1;
	
		idx = idx % prog.size();
		auto cmd = cmd_table[prog[idx]];
	
		if (cmd.type == JUMP) {
			idx = cmd.val;
		} else if (cmd.type == TURN) {
			turn(cmd.val);
			idx++;
		} else {
			auto p = pos + m.at(cmd.r, cmd.val, d);
			auto obj = f.get(p);
	
			if (cmd.type == LOOKUP)
				idx += obj;
			else if (cmd.type == GRASP)
				return grasp(f, p, obj);
			else
				return move(f, p, obj);
		}
	}
	
	return inc_age();
}

void Bot::replace(Field& f, Point& p, int bonus) {
	f.at(pos) = EMPTY;
	f.at(p) = BOT;
	inc_hp(bonus);
	pos = p;
}

int Bot::grasp(Field& f, Point& p, int obj) {
	if (obj == FOOD) {
		f.at(p) = EMPTY;
		inc_hp(food_bonus);
		now_food--;
	}
	return inc_age();
}

int Bot::move(Field& f, Point& p, int obj) {
	if (obj == EMPTY)
		replace(f, p);
	if (obj == FOOD) {
		replace(f, p, food_bonus);
		now_food--;
	}
	return inc_age();	
}

static Point replace_some(Field& f, int type1, int type2) {
	int i;
	do {
		i = rand() % f.data.size();
	} while (f.data[i] != type1);
	f.data[i] = type2;
	return Point(i % f.xsize, i / f.xsize);
}

Env::Env() : f(envx, envy) {
	generation = 0;
}

int Env::time_step() {
	for (auto& b : bots)
		if (!b.act(f))
			f.at(b.pos) = EMPTY;
	
	if (bots_alive() == 0.0) {
		last_avg_age = avg_age();
		bots = offspring<Bot>(bots);
		init();
		return generation++;
	}
	
	set_food();
	return 0;
}

void Env::set_food() {
	int disappear = 0.005 * now_food;
	now_food -= disappear;
	
	for (int i = 0; i < disappear; i++)
		replace_some(f, FOOD, EMPTY);
	
	for (; now_food < food_balance; now_food++)
		replace_some(f, EMPTY, FOOD);
}

void Env::set_bots() {
	for (auto& b : bots) {
		b.pos = replace_some(f, EMPTY, BOT);
		b.refresh();
	}
}

void Env::init() {
	now_food = 0;
	fill(f.data.begin(), f.data.end(), EMPTY);
	set_bots();
	set_food();
}

int Env::avg_age() {
	int avg = 0;
	for (auto& b : bots)
		avg += b.age;
	return avg / bots.size();
}

double Env::bots_alive() {
	int n = 0;
	for (auto& b : bots)
		n += (b.hp != 0);
	return (double) n / bots.size();
}

void Env::set_parameters(int prog_size, int hp_cap,
						 int food_bonus, int lookup,
						 double bot_population,
						 double food_balance) {
	set_prog_size(prog_size);
	set_hp_cap(hp_cap);
	set_food_bonus(food_bonus);
	set_food_balance(food_balance);
	set_lookup(lookup);
	set_bot_population(bot_population);
	init();
}

void Env::set_prog_size(int value) {
	prog_size = value;
	for (auto& b : bots)
		b.prog.resize(prog_size);
}

void Env::set_food_balance(double value) {
	food_balance = f.data.size() * value;
}

void Env::set_food_bonus(int value) {
	food_bonus = value;
}

void Env::set_hp_cap(int value) {
	hp_cap = value;
}

void Env::set_bot_population(double value) {
	size_t old_size = bots.size();
	size_t new_size = f.data.size() * value;
	
	if (new_size == old_size)
		return;
	
	for (auto& b : bots)
		f.at(b.pos) = EMPTY;
	
	bots.clear();
	bots.resize(new_size);
	
	for (auto& b : bots) {
		b.set_prog(prog_size);
		b.refresh();
	}
	
	init();
}

void Env::set_lookup(int value) {
	m.extend(value);
	cmd_table.clear();
	cmd_table.add(MOVE, 1, 8);
	for (int r = 1; r <= value; r++)
		cmd_table.add(LOOKUP, r, 8*r);
	cmd_table.add(TURN, 0, 7, 1);
	cmd_table.add(JUMP, 0, prog_size);
}
