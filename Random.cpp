#include "Random.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <functional>
#include <format>
#include <exception>

using namespace std;

RandomCellGenerator::RandomCellGenerator(int board_size) : p(board_size) {
    if (board_size <= 0) {
        throw "Board size must be > 0";
    }

    d = uniform_int_distribution<int>(0, board_size - 1);

    random_device r;
    seed_seq seeds{ r(), r(), r(), r(), r(), r() };
    e = mt19937(seeds);
}

Cell RandomCellGenerator::operator()() {
    return { d(e), d(e) };
}

int RandomCellGenerator::get_board_size() const {
    return p;
}

void run_experiment(int n, long long m) {
    if (m <= 0) {
        cout << format("m = {0} | Skipping empty experiment\n", m);
        return;
    }

    RandomCellGenerator generate_cell(n);
    vector<Cell> cells;
    cells.reserve(m);

    for (long long i = 0; i < m; ++i) {
        cells.push_back(generate_cell());
    }

    bool bounds_ok = all_of(cells.begin(), cells.end(), [n](const Cell& c) {
        return c.a >= 0 && c.a < n && c.b >= 0 && c.b < n;
        });

    if (!bounds_ok) {
        throw "Generated cells are out of bounds!";
    }

    sort(cells.begin(), cells.end(), [](const Cell& x, const Cell& y) {
        return (x <=> y) < 0;
        });

    VInt multiplicities;
    int current_count = 1;
    for (size_t i = 1; i < cells.size(); ++i) {
        if ((cells[i] <=> cells[i - 1]) == 0) {
            current_count++;
        }
        else {
            multiplicities.push_back(current_count);
            current_count = 1;
        }
    }
    multiplicities.push_back(current_count);

    sort(multiplicities.begin(), multiplicities.end(), greater<int>());

    long long sum = accumulate(multiplicities.begin(), multiplicities.end(), 0LL, plus<long long>());

    double expected_ratio = double(m) / (double(n) * double(n));
    double mean = double(sum) / double(multiplicities.size());
    double median = 0.0;

    size_t sz = multiplicities.size();
    if (sz % 2 != 0) {
        median = double(multiplicities[sz / 2]);
    }
    else {
        median = (double(multiplicities[sz / 2 - 1]) + double(multiplicities[sz / 2])) / 2.0;
    }

    int max_mult = multiplicities[0];
    double deviation = mean - expected_ratio;
    if (deviation < 0) deviation = -deviation;

    auto s = format("m = {0:<10} | m/n^2 = {1:<6.2f} | Mean = {2:<6.2f} | Median = {3:<6.2f} | Max mult = {4:<6} | Dev = {5:<6.2f}\n",
        m, expected_ratio, mean, median, max_mult, deviation);
    cout << s;
}

void explore_size_dependency(const VInt& sizes, double ratio) {
    cout << format("\nSize dependency (m/n^2 = {0:.2f} fixed):\n", ratio);
    cout << "n         Mean        Median\n";

    for (int n : sizes) {
        long long m = (long long)(ratio * double(n) * double(n));

        RandomCellGenerator generate_cell(n);
        vector<Cell> cells;
        cells.reserve(m);
        for (long long i = 0; i < m; ++i) cells.push_back(generate_cell());

        sort(cells.begin(), cells.end(), [](const Cell& x, const Cell& y) {
            return (x <=> y) < 0;
            });

        VInt multiplicities;
        int current_count = 1;
        for (size_t i = 1; i < cells.size(); ++i) {
            if ((cells[i] <=> cells[i - 1]) == 0) {
                current_count++;
            }
            else {
                multiplicities.push_back(current_count);
                current_count = 1;
            }
        }
        multiplicities.push_back(current_count);
        sort(multiplicities.begin(), multiplicities.end(), greater<int>());

        double mean = double(accumulate(multiplicities.begin(), multiplicities.end(), 0LL, plus<long long>())) / double(multiplicities.size());

        size_t sz = multiplicities.size();
        double median = (sz % 2 != 0) ? double(multiplicities[sz / 2])
            : (double(multiplicities[sz / 2 - 1]) + double(multiplicities[sz / 2])) / 2.0;

        cout << format("{0:<10}{1:<15.4f}{2:.4f}\n", n, mean, median);
    }
}