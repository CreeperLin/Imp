#include <ctime>
#include <cstdlib>
#include <vector>
#include <queue>
#include <string>
#include <iostream>
#include <algorithm>
#define LOGLVL 1
#define cerr(i) if(LOGLVL<=i) cerr<<"INFO"<<" "<<i<<":"
#define cerra(i) if(LOGLVL<=i) cerr
using namespace std;

const int TOTALNUM = 25;
const int TOTALKIND = 12;
const int UPPER_RANGE_L = 11, UPPER_RANGE_R = 16;
const int LOWER_RANGE_L = 0, LOWER_RANGE_R = 5;
const int H = 17;
const int W = 5;
const int DX[8] = { -1, 1, 0, 0, 1, 1, -1, -1};
const int DY[8] = {0, 0, -1, 1, 1, -1, 1, -1};
const int INF = 0x3F3F3F3F;
//n,s,w,e,ne,nw,se,sw
int rounds = 0;
int id;
int map[H][W];
int pcnt[2] = {0};
int flag[2];

//class Chessboard
//{
//	public:
//
//};

class Cpiece
{
	public:
		bool enable;
		int x;
		int y;
		void reset()
		{
			enable = false;
			x = y = 0;
		}
};

class Cmove
{
	public:
		int x, y, xx, yy, rst;
		bool operator<(Cmove t) const
		{
			return this->rst < t.rst;
		}
		Cmove(int _x, int _y, int _xx, int _yy, int _rst): x(_x), y(_y), xx(_xx), yy(_yy), rst(_rst) {}
};

Cpiece pcs[2][TOTALNUM];

inline int GetFlag(int x, int y)
{
	return map[x][y] / TOTALKIND;
}

inline int GetKind(int x, int y)
{
	return map[x][y] % TOTALKIND;
}

inline int mapi(int x, int y)
{
	return W * x + y;
}

inline int i2x(int n)
{
	return n /= W;
}

inline int i2y(int n)
{
	return n %= W;
}

void refresh_list()
{
	cerr(0) << "refreshing list {" << endl;
	pcnt[0] = pcnt[1] = 0;
	for (int x = 0; x < H; x++)
	{
		for (int y = 0; y < W; y++)
		{
			if (map[x][y] != -2)
			{
				int f = GetFlag(x, y);
				pcs[f][pcnt[f]].x = x;
				pcs[f][pcnt[f]].y = y;
				pcs[f][pcnt[f]].enable = true;
				cerr(0) << "found " << GetKind(x, y) << " " << f << " " << x << " " << y << endl;
				pcnt[f]++;
			}
		}
	}
	cerra(0) << "id 0:" << pcnt[0] << " id 1:" << pcnt[1] << endl;
	cerra(0) << "}" << endl;
}

bool IsKill(int x, int y, int xx, int yy)
{
	int t1 = GetKind(x, y), t2 = GetKind(xx, yy);
	if (t2 == -2) return false;
	if (t1 == 10 || t2 == 10)
		return true;
	if (t2 == 9)
	{
		if (t1 == 8)
			return true;
		return false;
	}
	if (t1 <= t2)
		return true;
	return false;
}

bool IsKilled(int x, int y, int xx, int yy)
{
	int t1 = GetKind(x, y), t2 = GetKind(xx, yy);
	if (t2 == -2) return false;
	if (t1 == 10 || t2 == 10)
		return true;
	if (t2 == 9)
	{
		if (t1 == 8)
			return false;
		return true;
	}
	if (t1 >= t2)
		return true;
	return false;
}

int nwe[] = {mapi(1, 0), mapi(1, 2), mapi(1, 4), mapi(2, 1), mapi(2, 3), mapi(3, 0), mapi(3, 2), mapi(3, 4), mapi(4, 1), mapi(4, 3),   mapi(11, 0), mapi(11, 2), mapi(11, 4), mapi(12, 1), mapi(12, 3), mapi(13, 0), mapi(13, 2), mapi(13, 4), mapi(14, 1), mapi(14, 3)};
int swe[] = {mapi(15, 0), mapi(15, 2), mapi(15, 4), mapi(14, 1), mapi(14, 3), mapi(13, 0), mapi(13, 2), mapi(13, 4), mapi(12, 1), mapi(12, 3), mapi(5, 0), mapi(5, 2), mapi(5, 4), mapi(4, 1), mapi(4, 3) , mapi(3, 0), mapi(3, 2), mapi(3, 4), mapi(2, 1), mapi(2, 3)};
int base[] = {mapi(0, 1), mapi(0, 3), mapi(16, 1), mapi(16, 3)};
int camp[] = {mapi(2, 1), mapi(2, 3), mapi(3, 2), mapi(4, 1), mapi(4, 3), mapi(12, 1), mapi(12, 3), mapi(13, 2), mapi(14, 1), mapi(14, 3)};

bool exist(int x, int y)
{
	return (LOWER_RANGE_L <= x && x <= LOWER_RANGE_R && 0 <= y && y < W)
	       || (UPPER_RANGE_L <= x && x <= UPPER_RANGE_R && 0 <= y && y < W)
	       || ((x == 6 || x == 8 || x == 10) && (y == 0 || y == 2 || y == 4));
}

bool IsOnVRail(int x, int y)
{
	return ((y == 0 || y == 4) && x > 0 && x < 16) || ((y == 2) && x > 4 && x < 12);
}

bool IsOnHRail(int x, int y)
{
	int rx[] = {1, 5, 6, 8, 10, 11, 15};
	for (int i = 0; i < 7; i++)
	{
		if (x == rx[i]) return true;
	}
	return false;
}
bool IsOnRail(int x, int y)
{
	return IsOnVRail(x, y) || IsOnHRail(x, y);
}
bool IsHReach(int x, int y, int xx, int yy)
{
	if (x != xx) return false;
	int dy = (y > yy) ? -1 : 1;
	for (int i = y + dy; i != yy; i += dy)
	{
		if (!IsOnHRail(x, i) || map[x][i] != -2) return false;
	}
	return true;
}
bool IsVReach(int x, int y, int xx, int yy)
{
	if (y != yy) return false;
	int dx = (x > xx) ? -1 : 1;
	for (int i = x + dx; i != xx; i += dx)
	{
		if (!IsOnVRail(i, y) || map[i][y] != -2) return false;
	}
	return true;
}
bool IsReach(int x, int y, int xx, int yy)
{
	int cmap[H][W] = {0};
	int qx[100], qy[100];
	int l = 0, r = 1;
	qx[0] = x;
	qy[0] = y;
	while (l < r)
	{
		for (int i = 0; i < 4; i++)
		{
			int cx = qx[l] + DX[i], cy = qy[l] + DY[i];
			if(cx == xx && cy == yy)
			{
				cerra(0) << "(bfs: " << l << " " << r << ")=Y" << endl;
				return true;
			}
			if(IsOnRail(cx, cy) && map[cx][cy] == -2 && !cmap[cx][cy])
			{
				qx[r] = cx;
				qy[r] = cy;
				cmap[cx][cy] = 1;
				r++;
			}
		}
		l++;
	}
	cerra(0) << "(bfs: " << l << " " << r << ")=N" << endl;
	return false;
}
bool IsNWE(int  x, int y)
{
	int t = mapi(x, y);
	for (int i = 0; i < 20; i++)
	{
		if (t == nwe[i]) return true;
	}
	return false;
}
bool IsSWE(int  x, int y)
{
	int t = mapi(x, y);
	for (int i = 0; i < 20; i++)
	{
		if (t == swe[i]) return true;
	}
	return false;
}
bool IsCamp(int x, int y)
{
	int t = mapi(x, y);
	for (int i = 0; i < 10; i++)
	{
		if(t == camp[i]) return true;
	}
	return false;
}
bool IsBase(int x, int y)
{
	int t = mapi(x, y);
	for (int i = 0; i < 4; i++)
	{
		if(t == base[i]) return true;
	}
	return false;
}
bool IsValidMove(int x, int y, int xx, int yy)
{
	int typ = GetKind(x, y), tgtflg = GetFlag(xx, yy), objflg = GetFlag(x, y);
	cerra(0) << typ << " " << tgtflg << " " << objflg << ")validating.0.";
	bool t = ((x == xx && y == yy) || (!exist(x, y)) || (!exist(xx, yy)) || (map[x][y] == -2) || (( x == 0 ||  x == 16) && (y == 1 || y == 3)) || (map[xx][yy] != -2 && IsCamp(xx, yy)) || (objflg != id) || ((map[xx][yy] != -2) && (tgtflg == id)) || (typ == 9) || (typ == 11));
	if (t) return false;
	int dx = xx - x, dy = yy - y;
	cerra(0) << "1.";
	if (!dx && (dy == 1 || dy == -1)) return true;
	cerra(0) << "2.";
	if (!dy && (dx == 1 || dx == -1)) return true;
	cerra(0) << "3.";
	if ((dx == 1 && (dy == -1 || dy == 1)) && IsNWE(x, y)) return true;
	cerra(0) << "4.";
	if ((dx == -1 && (dy == -1 || dy == 1)) && IsSWE(x, y)) return true;
	cerra(0) << "5.";
	if (IsOnHRail(x, y) && IsOnHRail(xx, yy) && IsHReach(x, y, xx, yy)) return true;
	cerra(0) << "6.";
	if (IsOnVRail(x, y) && IsOnVRail(xx, yy) && IsVReach(x, y, xx, yy)) return true;
	cerra(0) << "7.";
	if (map[x][y] == 8 && IsOnRail(x, y) && IsOnRail(xx, yy) && IsReach(x, y, xx, yy)) return true;
	return false;
}

int Dist(int x1, int y1, int x2, int y2)
{
	return abs(x1 - x2) + abs(y1 - y2);
}

bool IsFlagBlocked(int cid)
{
	int fx = i2x(flag[cid]), fy = i2y(flag[cid]);
	for (int i = 0; i < 4; i++)
	{
		int cx = fx + DX[i], cy = fy + DY[i];
		if (exist(cx, cy) && map[cx][cy] == -2) return false;
	}
	return true;
}

int PValue(int x, int y)
{
	if(IsBase(x, y) && map[x][y] != 11) return -INF;
	int d = Dist(x, y, i2x(flag[!id]), i2y(flag[!id]));
	int pv = -d * IsFlagBlocked(!id) * 10;
	if (IsCamp(x, y)) pv += 10;
	return pv;
}

int Value(int x, int y)
{
	int tv = 0, f = GetFlag(x, y), t = GetKind(x, y);
	int bv[TOTALKIND] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 8, 15};
	tv = 100 * bv[t] + PValue(x, y);
	return tv;
}

void change()
{
	int x, y, xx, yy, col, kind;
	cin >> x >> y >> xx >> yy >> col >> kind;
	cerr(1) << "Get updates:"  << endl;
	cerra(1) << x << ' ' << y << ' ' << xx << ' ' << yy << ' ' << col << ' ' << kind << endl;
	int tar = col * TOTALKIND + kind;
	if (x == xx && y == yy)
	{
		cerr(5) << "WOW" << endl;
		map[x][y] = tar;
	}
	else
	{
		map[x][y] = -2;
		map[xx][yy] = tar;
	}
}

void show_init(int id)
{
	//give a initializing map here
	//this line : kind1 kind2 ... etc
	//Imagine that the chesses are listed from the bottom to the top, left to right
	//This is a stupid start:
	int opt[25] = {10, 11, 10, 2, 2, 9, 9, 9, 4, 4, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 1, 0, 3, 3};
//	int opt[25] = {9, 11, 9, 7, 8, 7, 9, 8, 8, 6, 4, 10, 4, 5, 6, 6, 5, 3, 10, 3, 0, 2, 1, 7, 2};
	for (int i = 0; i < 25; ++i)
		cout << opt[i] << ' ';
	cout << endl;
}
void get_init()
{
	int arr0[25], arr1[25];
	for (int i = 0; i < 25; ++i)
	{
		cin >> arr0[i];
	}
	for (int i = 0; i < 25; ++i)
	{
		cin >> arr1[i];
		arr1[i] += TOTALKIND;
	}
	map[0][0] = arr0[0];
	map[0][1] = arr0[1];
	map[0][2] = arr0[2];
	map[0][3] = arr0[3];
	map[0][4] = arr0[4];
	map[1][0] = arr0[5];
	map[1][1] = arr0[6];
	map[1][2] = arr0[7];
	map[1][3] = arr0[8];
	map[1][4] = arr0[9];
	map[2][0] = arr0[10];
	map[2][2] = arr0[11];
	map[2][4] = arr0[12];
	map[3][0] = arr0[13];
	map[3][1] = arr0[14];
	map[3][3] = arr0[15];
	map[3][4] = arr0[16];
	map[4][0] = arr0[17];
	map[4][2] = arr0[18];
	map[4][4] = arr0[19];
	map[5][0] = arr0[20];
	map[5][1] = arr0[21];
	map[5][2] = arr0[22];
	map[5][3] = arr0[23];
	map[5][4] = arr0[24];

	map[16][0] = arr1[4];
	map[16][1] = arr1[3];
	map[16][2] = arr1[2];
	map[16][3] = arr1[1];
	map[16][4] = arr1[0];
	map[15][0] = arr1[9];
	map[15][1] = arr1[8];
	map[15][2] = arr1[7];
	map[15][3] = arr1[6];
	map[15][4] = arr1[5];
	map[14][0] = arr1[12];
	map[14][2] = arr1[11];
	map[14][4] = arr1[10];
	map[13][0] = arr1[16];
	map[13][1] = arr1[15];
	map[13][3] = arr1[14];
	map[13][4] = arr1[13];
	map[12][0] = arr1[19];
	map[12][2] = arr1[18];
	map[12][4] = arr1[17];
	map[11][0] = arr1[24];
	map[11][1] = arr1[23];
	map[11][2] = arr1[22];
	map[11][3] = arr1[21];
	map[11][4] = arr1[20];
}

inline void end()
{
	std::cout << "END\n" << std::flush;
}

void make_decision(int &x, int &y, int &xx, int &yy)
{
	clock_t st = clock();
	double ct = 0;
	priority_queue <Cmove> move;
	cerr(1) << "Trying {" << endl;
	for (int i = 0; i < TOTALNUM; i++)
	{
		if (!pcs[id][i].enable) continue;
		x = pcs[id][i].x;
		y = pcs[id][i].y;
		for (xx = 0; xx < H; xx++)
		{
			for (yy = 0; yy < W && (ct = static_cast<double>(clock() - st) / CLOCKS_PER_SEC) < 0.95; yy++)
			{
				cerra(0) << "Trial " << ct << " : " << x << ", " << y << " to " << xx << ", " << yy << "..(";
				if (IsValidMove(x, y, xx, yy))
				{
					cerra(0) << "valid" << endl;
					cerra(1) << "IsKill:" << IsKill(x, y, xx, yy) << " IsKilled:" << IsKilled(x, y, xx, yy) << " ValueSub:" << Value(x, y) << " ValueObj:" << Value(xx, yy) << endl;
					int r = PValue(xx, yy) + Value(xx, yy) * IsKill(x, y, xx, yy) - Value(x, y) * IsKilled(x, y, xx, yy);
					move.push(Cmove(x, y, xx, yy, r));
					cerra(1) << "push " << x << " " << y << " " << xx << " " << yy << " " << r << endl;
				}
				else cerra(0) << "invalid" << endl;
			}
		}
	}
	cerra(1) << "}" << endl;
	if(move.empty())
	{
		cout << "GG";
		end();
	}
	else
	{
		cerr(2) << "valid queue size:" << move.size() << endl;
		Cmove t = move.top();
		x = t.x;
		y = t.y;
		xx = t.xx;
		yy = t.yy;
		cerr(2) << "printing queue:{" << endl;
		while(!move.empty())
		{
			Cmove s = move.top();
			cerr << s.x << " " << s.y << " " << s.xx << " " << s.yy << " " << s.rst << endl;
			move.pop();
		}
		cerra(2) << "}" << endl;
	}
}

int main(int argc, char** argv)
{
	unsigned seed = time(0);
	if (argc == 2)
	{
		cerr(0) << "Excited! Get given seed = " << argv[1] << endl;
		seed = 0;
		for (char *pc = argv[1]; *pc; ++pc)
			seed = seed * 10 + (*pc - '0');
	}
	srand(seed);
	for (int i = 0; i < H; ++i)
	{
		for (int j = 0; j < W; ++j)
		{
			map[i][j] = -2;
		}
	}
	string op;
	while (true)
	{
		cin >> op;
		if (op == "id")
		{
			cin >> id;
			cerr(5) << "My id:" << id << endl;
			cout << "Imp-Trial-ZBJX" << endl;
			end();
		}
		else if (op == "refresh")
		{
			get_init();
			refresh_list();
			flag[0] = (map[0][1] == 11) ? 1 : 3;
			flag[1] = (map[16][1] == 11) ? mapi(16, 1) : mapi(16, 3);
		}
		else if (op == "init")
		{
			show_init(id);
			end();
		}
		else if (op == "message")
		{
			change();
		}
		else if (op == "action")
		{
			refresh_list();
			int x, y, xx, yy;
			cerr(5) << "Round " << rounds << ": {" << endl;
			make_decision(x, y, xx, yy);
			cerra(5) << "}" << endl;
			cerr(5) << "Action: " << x << " " << y << " " << xx << " " << yy << endl;
			cout << x << " " << y << " " << xx << " " << yy << endl;
			rounds++;
			end();
		}
	}
}
