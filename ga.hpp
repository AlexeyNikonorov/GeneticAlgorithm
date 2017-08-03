#ifndef GA_HPP
#define GA_HPP

#include <vector>
#include <algorithm>
#include <random>
#include <cmath>

bool flip(double p) {
	return rand() < RAND_MAX * p;
}

template<class Bot>
double fitness(const Bot& b) {
	return b.age + b.hp / (hp_cap * 10);
}

template<class Bot>
double fitness_avg(const vector<Bot>& bots) {
	double avg = accumulate(bots.begin(), bots.end(), 0.0,
		[](double s, const Bot& b){return s + fitness<Bot>(b);});
	return avg / bots.size();
}

template<class Bot>
bool cmp(const Bot& b1, const Bot& b2) {
	return fitness<Bot>(b1) > fitness<Bot>(b2);
}

template<class Bot>
vector<Bot> select(vector<Bot>& bots, size_t n) {
	sort(bots.begin(), bots.end(), cmp<Bot>);
	vector<Bot> selected(n);
	size_t top = bots.size() / 4;
	for (size_t i = 0; i < n; i++)
		selected[i] = bots[rand() % top];
	return selected;
}

template<class Bot>
void mutate(Bot& b, double p) {
	for (int& i : b.prog)
		if (flip(p))
			i = rand() % cmd_table.size();
}

template<class Bot>
void mutation(vector<Bot>& bots, double p) {
	for (size_t i = 3*bots.size()/4; i < bots.size(); i++)
		mutate<Bot>(bots[i], p);
}

template<class Bot>
vector<Bot> offspring(vector<Bot>& bots) {
	vector<Bot> selected = select<Bot>(bots, bots.size());
	mutation<Bot>(selected, 0.1);
	return selected;
}

#endif
