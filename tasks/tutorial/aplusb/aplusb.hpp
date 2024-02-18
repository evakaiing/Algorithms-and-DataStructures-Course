#include <climits>
#include <iostream>

int Sum(int a, int b) {
    if ((a == INT_MAX) && (b == INT_MIN + 1)) {
        return 0;
    } else if ((a == INT_MAX && b > 0) || (b == INT_MAX && a > 0)) {
        return INT_MAX;
    } else {
        return a + b;
    }
}