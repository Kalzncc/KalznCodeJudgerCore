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


const int N = 2e7+5;
const int inf =0x3f3f3f3f;
const int mod = 1e9+7;
ll x, y, k;
ll G[N];
ll _pow(ll a, ll b)
{
	ll res = 1;
	while(b)
	{
		if (b & 1) res = res * a % mod;
		b >>= 1;
		a = a * a % mod;
	}
	return res;
}
ll invs;
ll getn(ll a0, ll q, ll n)
{
	if (q ==  1) return a0 * n % mod;
	return a0 * (1 - _pow(q, n) + mod ) % mod * _pow( (1 - q + mod) % mod, mod-2) % mod;
}
ll ans = 0;
ll getans(ll d, int dep)
{
	if (dep == k) 
	{
		return G[k];	
	}
	ll pre = getans(d<<1, dep+1) % mod;
    ll g0 = _pow(x, d) * invs % mod;
    ll q0 = _pow(x, d<<1);
    pre = (pre + G[dep] * getn(g0, q0, 1ll << (k - dep - 1) ) % mod) % mod;
    g0 = g0 * _pow(y, d+1) % mod;
    q0 = _pow(x*y%mod, d<<1);
    pre = (pre - G[dep] * getn(g0, q0, 1ll << (k - dep - 1) ) % mod + mod) % mod;
    // if (dep == k - 1)
    // {
    //     cout << g0 << " "<< q0 << endl;
    //     cout <<  G[dep] * getn(g0, q0, 1ll << (k - dep - 1) ) % mod << endl;
    // }
    return pre;
}
int main()
{
	cin >> k;
	for (int i = 0; i <= k; i++)
	{
		scanf("%lld", &G[i]);
		if (i) G[i] = (G[i-1] * G[i]) % mod;
	}
	int m; cin >> m;
	while(m--)
	{
		scanf("%lld%lld", &x, &y);
		invs = _pow( (1-y+mod)%mod,  mod-2);
		ans = 0;
		printf("%lld\n", getans(1, 0));
		//printf("%lld\n", ans);
	}
	return 0;
}