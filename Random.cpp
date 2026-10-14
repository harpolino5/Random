#include "Random.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <functional>
#include <iomanip>
#include <exception>

using namespace std;

RandomCellGenerator::RandomCellGenerator(int board_size) : p(board_size) {
    if (board_size <= 0) {
        throw "Board size must be > 0";
    }

    d = uniform_int_distribution<int>(0, board_size - 1);

    random_device r;
    e = mt19937(r());
}

Cell RandomCellGenerator::operator()() {
    return { d(e), d(e) };
}

int RandomCellGenerator::get_board_size() const {
    return p;
}

void run_experiment(int n, long long m) {
    if (m <= 0) {
        cout << "m = " << m << " | Skipping empty experiment\n";
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
        throw "Generated cells are outside our box";
    }

    sort(cells.begin(), cells.end(), [](const Cell& x, const Cell& y) {
        return (x <=> y) < 0;
        });

    My_Vector multiplicity;
    int current_count = 1;
    for (size_t i = 1; i < cells.size(); ++i) {
        if ((cells[i] <=> cells[i - 1]) == 0) {
            current_count++;
        }
        else {
            multiplicity.push_back(current_count);
            current_count = 1;
        }
    }
    multiplicity.push_back(current_count);

    sort(multiplicity.begin(), multiplicity.end(), greater<int>());

    long long sum = accumulate(multiplicity.begin(), multiplicity.end(), 0LL, plus<long long>());

    double expected_ratio = double(m) / (double(n) * double(n));
    double mean = double(sum) / double(multiplicity.size());
    double median = 0.0;

    size_t sz = multiplicity.size();
    if (sz % 2 != 0) {
        median = double(multiplicity[sz / 2]);
    }
    else {
        median = (double(multiplicity[sz / 2 - 1]) + double(multiplicity[sz / 2])) / 2.0;
    }

    int max_mult = multiplicity[0];
    double deviation = mean - expected_ratio;
    if (deviation < 0) deviation = -deviation;

    cout <<
        << "m = " << setw(10) << m
        << " | m/n^2 = " << fixed << setprecision(2) << setw(6) << expected_ratio
        << " | Mean = " << setw(6) << mean
        << " | Median = " << setw(6) << median
        << " | Max mult = " << setw(6) << max_mult
        << " | Dev = " << setw(6) << deviation << "\n";
}

void explore_size_dependency(const My_Vector& sizes, double ratio) {
    cout << "\nSize dependency (m/n^2 = " << fixed << setprecision(2) << ratio << " fixed):\n";
    cout << left << setw(10) << "n" << setw(15) << "Mean" << "Median\n";

    for (int n : sizes) {
        long long m = (long long)(ratio * double(n) * double(n));

        RandomCellGenerator generate_cell(n);
        vector<Cell> cells;
        cells.reserve(m);
        for (long long i = 0; i < m; ++i) cells.push_back(generate_cell());

        sort(cells.begin(), cells.end(), [](const Cell& x, const Cell& y) {
            return (x <=> y) < 0;
            });

        My_Vector multiplicity;
        int current_count = 1;
        for (size_t i = 1; i < cells.size(); ++i) {
            if ((cells[i] <=> cells[i - 1]) == 0) {
                current_count++;
            }
            else {
                multiplicity.push_back(current_count);
                current_count = 1;
            }
        }
        multiplicity.push_back(current_count);
        sort(multiplicity.begin(), multiplicity.end(), greater<int>());

        double mean = double(accumulate(multiplicity.begin(), multiplicity.end(), 0LL, plus<long long>())) / double(multiplicity.size());

        size_t sz = multiplicity.size();
        double median = (sz % 2 != 0) ? double(multiplicity[sz / 2])
            : (double(multiplicity[sz / 2 - 1]) + double(multiplicity[sz / 2])) / 2.0;

        cout << left << setw(10) << n
            << fixed << setprecision(4) << setw(15) << mean
            << median << "\n";
    }
}
