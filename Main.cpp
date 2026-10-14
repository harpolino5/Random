#include "Random.h"
#include <iostream>
#include <exception>

using namespace std;

int main() {
    try {
        cout << "Task 9" << endl;
        int p = 50;
        int pp = p * p;
        cout << "Board size: " << p << "x" << p << " (" << pp << " cells)\n";
        run_experiment(p, 2500);
        run_experiment(p, 12500);
        run_experiment(p, 25000);
        run_experiment(p, 50000);
        run_experiment(p, 250000);

        My_Vector sizes = { 10, 20, 30, 40, 50, 70, 100 };
        explore_size_dependency(sizes, 5.0);
    }
    catch (const char* msg) {
        cout << "Error: " << msg << endl;
        return 1;
    }
    catch (const exception& e) {
        cout << "Standard Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
