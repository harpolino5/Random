#ifndef RANDOM_H_INCLUDED
#define RANDOM_H_INCLUDED

#include <compare>
#include <random>
#include <vector>

using My_Vector = std::vector<int>;

struct Cell {
    int a = 0;
    int b = 0;

    auto operator<=>(const Cell& other) const = default;
};

class RandomCellGenerator {
private:
    int p;
    std::mt19937 e;
    std::uniform_int_distribution<int> d;

public:
    explicit RandomCellGenerator(int board_size);
    Cell operator()();
    int get_board_size() const;
};

void run_experiment(int n, long long m);
void explore_size_dependency(const My_Vector& sizes, double ratio);

#endif
