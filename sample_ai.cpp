#include <ctime>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#define LOGLVL 0
#define cerr(i) if(LOGLVL<=i) cerr<<"INFO"<<" "<<i<<":"

using namespace std;

const int TOTALNUM = 25;
const int TOTALKIND = 12;
const int UPPER_RANGE_L = 11, UPPER_RANGE_R = 16;
const int LOWER_RANGE_L = 0, LOWER_RANGE_R = 5;
const int H = 17;
const int W = 5;
const int DX[8] = { -1, 1, 0, 0, 1, 1, -1, -1};
const int DY[8] = {0, 0, -1, 1, 1, -1, 1, -1};
//n,s,w,e,ne,nw,se,sw
int rounds = 0;
int id;
int map[H][W];

int mapi(int x, int y)
{
	return 5 * x + y;
}

bool IsKill(int x, int y, int xx, int yy)
{
	int t1 = map[x][y] % TOTALKIND, t2 = map[xx][yy] % TOTALKIND;
	if (t2==-2) return false;
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
	int t1 = map[x][y] % TOTALKIND, t2 = map[xx][yy] % TOTALKIND;
	if (t2==-2) return false;
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
}

int nwe[] = {mapi(1, 0), mapi(1, 2), mapi(1, 4), mapi(2, 1), mapi(2, 3), mapi(3, 0), mapi(3, 2), mapi(3, 4), mapi(4, 1), mapi(4, 3),   mapi(11, 0), mapi(11, 2), mapi(11, 4), mapi(12, 1), mapi(12, 3), mapi(13, 0), mapi(13, 2), mapi(13, 4), mapi(14, 1), mapi(14, 3)};
int swe[] = {mapi(15, 0), mapi(15, 2), mapi(15, 4), mapi(14, 1), mapi(14, 3), mapi(13, 0), mapi(13, 2), mapi(13, 4), mapi(12, 1), mapi(12, 3), mapi(5, 0), mapi(5, 2), mapi(5, 4), mapi(4, 1), mapi(4, 3) , mapi(3, 0), mapi(3, 2), mapi(3, 4), mapi(2, 1), mapi(2, 3)};
int railh[35];
int railv[37];
//int base[]={mapi(0, 1), mapi(0, 3), mapi(16, 1), mapi(16, 3)};
int camp[] = {mapi(2, 1), mapi(2, 3), mapi(3, 2), mapi(4, 1), mapi(4, 3), mapi(12, 1), mapi(12, 3), mapi(13, 2), mapi(14, 1), mapi(14, 3)};
void rail()
{
	int rx[] = {1, 5, 6, 8, 10, 11, 15};
	int n = 0;
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			railh[n++] = mapi(rx[i], j);
		}
	}
	n = 0;
	for (int i = 0; i < 5; i += 4)
	{
		for (int j = 1; j < 16; j++)
		{
			railv[n++] = mapi(j, i);
		}
	}
	for (int i = 5; i < 12; i++)
	{
		railv[n++] = mapi(i, 2);
	}
}

bool exist(int x, int y)
{
	return (LOWER_RANGE_L <= x && x <= LOWER_RANGE_R && 0 <= y && y < W)
	       || (UPPER_RANGE_L <= x && x <= UPPER_RANGE_R && 0 <= y && y < W)
	       || ((x == 6 || x == 8 || x == 10) && (y == 0 || y == 2 || y == 4));
}

bool IsOnVRail(int x, int y)
{
	return ((y == 0 || y == 4) && x > 0 && x < 16) || ((y == 2) && x > 4 && x < 12);
	int t = mapi(x, y);
	for (int i = 0; i < 37; i++)
	{
		if (t == railv[i]) return true;
	}
	return false;
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
//	return false;
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
				cerr << "(bfs: " << l << " " << r << ")=Y" << endl;
				return true;
			}
			if(exist(cx, cy) && map[cx][cy] == -2 && !cmap[cx][cy] && IsOnRail(cx, cy))
			{
				qx[r] = cx;
				qy[r] = cy;
				cmap[cx][cy] = 1;
				r++;
			}
		}
		l++;
	}
	cerr << "(bfs: " << l << " " << r << ")=N" << endl;
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
bool IsValidMove(int x, int y, int xx, int yy)
{
	int typ = map[x][y] % TOTALKIND, tgtflg = map[xx][yy] / TOTALKIND, objflg = map[x][y] / TOTALKIND;
	cerr << typ << " " << tgtflg << " " << objflg << "Valid.0.";
	bool t = ((x == xx && y == yy) || (!exist(x, y)) || (!exist(xx, yy)) || (map[x][y] == -2) || (((id == 0 && x == 0) || (id == 1 && x == 16)) && (y == 1 || y == 3)) || (map[xx][yy] != -2 && IsCamp(xx, yy)) || (objflg != id) || ((map[xx][yy] != -2) && (tgtflg == id)) || (typ == 9) || (typ == 11));
	if (t) return false;
	int dx = xx - x, dy = yy - y;
	cerr << "1.";
	if (!dx && (dy == 1 || dy == -1)) return true;
	cerr << "2.";
	if (!dy && (dx == 1 || dx == -1)) return true;
	cerr << "3.";
	if ((dx == 1 && (dy == -1 || dy == 1)) && IsSWE(x, y)) return true;
	cerr << "4.";
	if ((dx == -1 && (dy == -1 || dy == 1)) && IsNWE(x, y)) return true;
	cerr << "5.";
	if (IsOnHRail(x, y) && IsOnHRail(xx, yy) && IsHReach(x, y, xx, yy)) return true;
	cerr << "6.";
	if (IsOnVRail(x, y) && IsOnVRail(xx, yy) && IsVReach(x, y, xx, yy)) return true;
	cerr << "7.";
	if (map[x][y] == 8 && IsOnRail(x, y) && IsOnRail(xx, yy) && IsReach(x, y, xx, yy)) return true;
	return false;
}

void change()
{
	int x, y, xx, yy, col, kind;
	cin >> x >> y >> xx >> yy >> col >> kind;
	cerr(0) << "Get updates:"  << endl;
	cerr << x << ' ' << y << ' ' << xx << ' ' << yy << ' ' << col << ' ' << kind << endl;
	int tar = col * TOTALKIND + kind;
	if (x == xx && y == yy) map[x][y] = tar;
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
	int n = 0;
	while (n < 10000)
	{
		n++;
		x = rand() % H;
		y = rand() % W;
		xx = rand() % H;
		yy = rand() % W;
		cerr(0) << "Trial " << n << " : " << x << ", " << y << " to " << xx << ", " << yy << "...";
		if (IsValidMove(x, y, xx, yy))
		{
			cerr << "valid" << endl;
			int r=IsKill(x,y,xx,yy) - IsKilled(x,y,xx,yy);
			if (r == 1) return;
			else if (r==0&&n>1000) return;
			else if (n>5000) return;
		}
		cerr << "invalid" << endl;
	}
	cout << "GG";
	end();
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
	//rail();
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
			cerr(0) << id << endl;
			cout << "Imp-Trial-Derpy-Derp-Herpy-Boing~ZBJX" << endl;
			end();
		}
		else if (op == "refresh")
		{
			get_init();
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
			int x, y, xx, yy;
			cerr(5) << "Round " << rounds << ": {" <<endl;
			make_decision(x, y, xx, yy);
			cerr(5) << "Action: " << x << " " << y << " " << xx << " " << yy << endl<<"}"<<endl;
			cout << x << " " << y << " " << xx << " " << yy << endl;
			rounds++;
			end();
		}
	}
}