// % everyone
#include <cstdio>
#include<iostream>
#include<cstring>
#include <map>
#include <queue>
#include <set>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <list>
#include <cctype>
#include <time.h>

namespace IO {
    double start_time = 0.0;
    void ct() { start_time = clock(); return; }
    void fast_cin() { std::ios::sync_with_stdio(false); std::cin.tie(); }
    void read_f(int flag = 0) { freopen("0.in", "r", stdin); if(!flag) freopen("0.out", "w", stdout); }
    void run_time() { std::cout << "\nESC in : " << ( clock() - start_time ) * 1000.0 / CLOCKS_PER_SEC << "ms" << std::endl; }
}
using namespace IO;
template <typename T>
bool bacmp(const T & a, const T & b) { return a > b; }
template <typename T>
bool pecmp(const T & a, const T & b) { return a < b; }

#define ll long long
#define ull unsigned ll
#define _min(x, y) ((x)>(y)?(y):(x))
#define _max(x, y) ((x)>(y)?(x):(y))
#define max3(x, y, z) ( max( (x), max( (y), (z) ) ) )
#define min3(x, y, z) ( min( (x), min( (y), (z) ) ) )
#define pr make_pair
#define pb push_back
using namespace std;

int main()
{
    int a, b;
    cin >> a >> b;
    double l = 0, r = 1e5;
    while (abs(r - l) > 1e-2) {
        double mid = (l + r) / 2.0;
        if (a * a + b * b > mid * mid) l = mid;
        else r = mid;
    }
    printf("%.6f\n", l);
    return 0;
}