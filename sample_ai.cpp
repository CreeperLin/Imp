#include <ctime>
#include <cstdlib>
#include <vector>
#include <queue>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>
#define LOGLVL 2
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
int fmap[H][W];
int flag[2];
clock_t st;

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

inline double get_time()
{
	return static_cast<double>(clock() - st) / CLOCKS_PER_SEC;
}

int nwe[] = {mapi(1, 0), mapi(1, 2), mapi(1, 4), mapi(2, 1), mapi(2, 3), mapi(3, 0), mapi(3, 2), mapi(3, 4), mapi(4, 1), mapi(4, 3),   mapi(11, 0), mapi(11, 2), mapi(11, 4), mapi(12, 1), mapi(12, 3), mapi(13, 0), mapi(13, 2), mapi(13, 4), mapi(14, 1), mapi(14, 3)};
int swe[] = {mapi(15, 0), mapi(15, 2), mapi(15, 4), mapi(14, 1), mapi(14, 3), mapi(13, 0), mapi(13, 2), mapi(13, 4), mapi(12, 1), mapi(12, 3), mapi(5, 0), mapi(5, 2), mapi(5, 4), mapi(4, 1), mapi(4, 3) , mapi(3, 0), mapi(3, 2), mapi(3, 4), mapi(2, 1), mapi(2, 3)};
int base[] = {mapi(0, 1), mapi(0, 3), mapi(16, 1), mapi(16, 3)};
int camp[] = {mapi(2, 1), mapi(2, 3), mapi(3, 2), mapi(4, 1), mapi(4, 3), mapi(12, 1), mapi(12, 3), mapi(13, 2), mapi(14, 1), mapi(14, 3)};
int bv[TOTALKIND] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 7, 8, 15};

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

int Dist(int x1, int y1, int x2, int y2)
{
	return abs(x1 - x2) + abs(y1 - y2);
}

class Cpiece
{
	public:
		int x;
		int y;
		Cpiece(): x(0), y(0) {}
		Cpiece(int _x, int _y): x(_x), y(_y) {}
		bool operator<(Cpiece t) const
		{
			return (id) ^ (this->x < t.x);//?
		}
		void print(int l)
		{
			cerra(l) << "x:" << x << " y:" << y << endl;
		}
};

class Cmove
{
	public:
		int x, y, xx, yy, rst;
		bool operator<(Cmove t) const
		{
			return this->rst > t.rst;
		}
		Cmove(int _x, int _y, int _xx, int _yy, int _rst): x(_x), y(_y), xx(_xx), yy(_yy), rst(_rst) {}
		void print(int l)
		{
			cerra(l) << "x:" << x << " y:" << y << " xx:" << xx << " yy:" << yy << " rst:" << rst << endl;
		}
};

class Chessboard
{
	public:
		int cid;
		int (&cmap)[H][W];
		priority_queue <Cpiece> pcs;
		Chessboard(): cid(0), cmap(::map), pcs() {}
		Chessboard(int _cid, int (&_cmap)[H][W], const priority_queue<Cpiece> &_pcs):
			cid(_cid), cmap(_cmap), pcs(_pcs) {}
		Chessboard fork(int (&m)[H][W]) const
		{
//			return Chessboard(cid, m, pcs);
			return Chessboard(cid, m, pcs);
		}
		priority_queue <Cpiece> refresh_list()
		{
			priority_queue <Cpiece> tpcs;
//			cerr(0) << "CB " << cid << ":Refreshing list {" << endl;
			for (int x = 0; x < H; x++)
			{
				for (int y = 0; y < W; y++)
				{
					if (cmap[x][y] != -2 && GetFlag(x, y) == cid)
					{
						tpcs.push(Cpiece(x, y));
//						cerra(0) << "found " << GetKind(x, y) << " " << x << " " << y << endl;
					}
				}
			}
//			pcnt[cid] = tpcs.size();
//			cerra(0) << "id " << cid << ":" << pcnt[cid] << endl;
//			cerra(0) << "}" << endl;
			return tpcs;
		}
		void refresh()
		{
			pcs = refresh_list();
		}
		void MoveBoard(Cmove mov)
		{
			if (!IsValidMove(mov))
			{
				cerr << "Boom" << endl;
				cout << "halt";
			}
			bool t = IsKilled(mov);
			if(IsKill(mov) || cmap[mov.xx][mov.yy] == -2)
			{
				cmap[mov.xx][mov.yy] = cmap[mov.x][mov.y];
				if(t)
				{
					cmap[mov.xx][mov.yy] = -2;
				}
			}
			cmap[mov.x][mov.y] = -2;
			refresh();
		}
		inline int GetFlag(int x, int y)
		{
			return cmap[x][y] / TOTALKIND;
		}
		inline int GetKind(int x, int y)
		{
			return cmap[x][y] % TOTALKIND;
		}
		bool IsKill(Cmove mov)
		{
			return IsKill(mov.x, mov.y, mov.xx, mov.yy);
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
		bool IsKilled(Cmove mov)
		{
			return IsKilled(mov.x, mov.y, mov.xx, mov.yy);
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
		bool IsHReach(int x, int y, int xx, int yy)
		{
			if (x != xx) return false;
			int dy = (y > yy) ? -1 : 1;
			for (int i = y + dy; i != yy; i += dy)
			{
				if (!IsOnHRail(x, i) || cmap[x][i] != -2) return false;
			}
			return true;
		}
		bool IsVReach(int x, int y, int xx, int yy)
		{
			if (y != yy) return false;
			int dx = (x > xx) ? -1 : 1;
			for (int i = x + dx; i != xx; i += dx)
			{
				if (!IsOnVRail(i, y) || cmap[i][y] != -2) return false;
			}
			return true;
		}
		bool IsReach(int x, int y, int xx, int yy)
		{
			int vmap[H][W] = {0};
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
//						cerra(0) << "(bfs: " << l << " " << r << ")=Y" << endl;
						return true;
					}
					if(IsOnRail(cx, cy) && cmap[cx][cy] == -2 && !vmap[cx][cy])
					{
						qx[r] = cx;
						qy[r] = cy;
						vmap[cx][cy] = 1;
						r++;
					}
				}
				l++;
			}
//			cerra(0) << "(bfs: " << l << " " << r << ")=N" << endl;
			return false;
		}
		bool IsFlagBlocked(int tid)
		{
			int fx = i2x(flag[tid]), fy = i2y(flag[tid]);
			for (int i = 0; i < 4; i++)
			{
				int cx = fx + DX[i], cy = fy + DY[i];
				if (exist(cx, cy) && cmap[cx][cy] == -2) return false;
			}
			return true;
		}
		int PValue(int x, int y)
		{
//			if(IsBase(x, y) && cmap[x][y] != 11) return -INF;
			int d = Dist(x, y, i2x(flag[!cid]), i2y(flag[!cid]));
			int pv = -d * IsFlagBlocked(!cid) * 10;
			if (IsCamp(x, y)) pv += 10;
			return pv;
		}
		int Value(int x, int y)
		{
			int tv = 0, t = GetKind(x, y);
			if (t == -2) return 0;
			if (t == 11) tv += INF;
			tv = 500 * bv[t] + PValue(x, y);
			return tv;
		}
		bool IsValidMove(Cmove mov)
		{
			return IsValidMove(mov.x, mov.y, mov.xx, mov.yy);
		}
		bool IsValidMove(int x, int y, int xx, int yy)
		{
			int typ = GetKind(x, y), tgtflg = GetFlag(xx, yy), objflg = GetFlag(x, y);
			cerra(0) << "#id " << cid << " #o " << cmap[x][y] << " #t " << cmap[xx][yy] << " " << typ << " " << tgtflg << " " << objflg << ")validating.0.";
			bool t = ((x == xx && y == yy) || (!exist(x, y)) || (!exist(xx, yy)) || (cmap[x][y] == -2) || (( x == 0 ||  x == 16) && (y == 1 || y == 3)) || (cmap[xx][yy] != -2 && IsCamp(xx, yy)) || (objflg != cid) || ((cmap[xx][yy] != -2) && (tgtflg == cid)) || (typ == 9) || (typ == 11));
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
			if (cmap[x][y] == 8 && IsOnRail(x, y) && IsOnRail(xx, yy) && IsReach(x, y, xx, yy)) return true;
			return false;
		}
		vector <Cmove> GenerateMoves()
		{
			cerr(1) << "CB " << cid << ":Generating moves:{" << endl;
			vector <Cmove> move;
			priority_queue <Cpiece> tpcs = pcs;
			while (!tpcs.empty())
			{
				Cpiece tp = tpcs.top();
				int x = tp.x, y = tp.y;
				for (int xx = 0; xx < H; xx++)
				{
					for (int yy = 0; yy < W ; yy++)
					{
						cerra(0) << "Trial " << " : " << x << ", " << y << " to " << xx << ", " << yy << "..(";
						if (IsValidMove(x, y, xx, yy))
						{
							cerra(0) << "valid" << endl;
//							cerra(1) << "IsKill:" << IsKill(x, y, xx, yy) << " IsKilled:" << IsKilled(x, y, xx, yy) << " ValueSub:" << Value(x, y) << " ValueObj:" << Value(xx, yy) << endl;
//							int r = PValue(xx, yy) + Value(xx, yy) * IsKill(x, y, xx, yy) - Value(x, y) * IsKilled(x, y, xx, yy);
							move.push_back(Cmove(x, y, xx, yy, 0));
							cerra(1) << "push " << x << " " << y << " " << xx << " " << yy << " " << 0 << endl;
						}
						else cerra(0) << "invalid" << endl;
					}
				}
				tpcs.pop();
			}
			cerra(1) << "}\n";
			return move;
		}
		int Evaluate()
		{
			int score = 0;
			priority_queue <Cpiece> tpcs = pcs;
			while (!tpcs.empty())
			{
				Cpiece tp = tpcs.top();
				int x = tp.x, y = tp.y;
				score += 500 * bv[GetKind(x, y)] + 10 * PValue(x, y);
				tpcs.pop();
			}
			return score;
		}
		void print(int l)
		{
			cerra(l) << "CB " << cid << ":{" << endl;
			for (int i = H - 1; i >= 0; i--)
			{
				for (int j = 0; j < W; j++)
				{
					cerra(l) << cmap[i][j] << " ";
				}
				cerra(l) << endl;
			}
			priority_queue <Cpiece> tpcs = pcs;
			while (!tpcs.empty())
			{
				Cpiece tp = tpcs.top();
				tp.print(l);
				tpcs.pop();
			}
			cerra(l) << "size:" << pcs.size() << endl;
			cerra(l) << "}" << endl;
		}
};
Chessboard CB;

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

//Cmove MCTS()
//{
//	cerr(3) << "MCTS:{" << endl;
//	st = clock();
//	vector <Cmove> mov0 = CB[id].GenerateMoves();
//	if(mov0.empty())
//	{
//		cout << "GG";
//		end();
//	}
//	vector <Cmove> mov1 = CB[!id].GenerateMoves();
//	if(mov1.empty())
//	{
//		cerr(2) << "Opponent no move!!!" << endl;
//	}
////	while(1)
//	int n = 0;
//	cerra(2) << "Size: mov0:" << mov0.size() << " mov1:" << mov1.size() << endl;
//	while(get_time() < 5)
//	{
//		cerra(5) << "Test " << n++ << "{\n";
//		for (int i = 0; i < mov0.size(); i++)
//		{
//			mov0[i].print(2);
//			cerra(2) << i << " Searching mov0:{\n";
//			for (int j = 0; j < mov1.size(); j++)
//			{
//				cerra(2) << j << " Searching mov1:{\n";
//				mov1[j].print(2);
//				memcpy(fmap, map, sizeof(map));
//				Chessboard B0(CB[0].fork(fmap));
//				Chessboard B1(CB[1].fork(fmap));
//				B0.MoveBoard(mov0[i]);
//				B1.MoveBoard(mov1[j]);
//				for (int r = 0; r < 3; r++)
//				{
//					cerra(2) << "round " << r << ":{\n";
//					vector <Cmove> mv0 = B0.GenerateMoves();
//					vector <Cmove> mv1 = B1.GenerateMoves();
//					if(!(mv0.size() || mv1.size())) break;
//					int c0 = rand() % mv0.size(), c1 = rand() % mv1.size();
//					cerra(2) << "B0 move:";
//					mv0[c0].print(2);
//					cerra(2) << "B1 move:";
//					mv1[c0].print(2);
//					B0.MoveBoard(mv0[c0]);
//					B1.MoveBoard(mv1[c1]);
////					B0.print(2);
////					B1.print(2);
//					cerra(2) << "}\n";
//				}
//				int s1 = B0.Evaluate(), s2 = B1.Evaluate();
//				cerra(3) << "s1:" << s1 << " s2:" << s2 << endl;
//				mov0[i].rst += s1 - s2;
//				cerra(2) << "}\n";
//			}
//			cerra(2) << "}\n";
//		}
//		cerra(2) << "}\n";
//	}
//	sort(mov0.begin(), mov0.end());
//	for (int i = 0; i < mov0.size(); i++)
//	{
//		mov0[i].print(3);
//	}
//	cerra(3) << "}" << endl;
//	return mov0[0];
//}

int Negamax(int depth)
{
	Chessboard TCB(CB.fork(fmap));
	TCB.cid = (depth % 2) ? id : !id;
	TCB.refresh();
//	TCB.print(3);
	int t = TCB.Evaluate();
	if (depth >= 3)
	{
		cerra(3) << "End score:" << t << endl;
		return t;
	}
	cerr(3) << "Negamax(depth " << depth << "):{\n";
	vector <Cmove> mov = TCB.GenerateMoves();
	cerra(3) << "Moves:" << mov.size() << endl;
	if (mov.empty()) return t;
	int score = -INF;
	for (int i = 0; i < mov.size(); i++)
	{
		mov[i].print(2);
		int t1 = fmap[mov[i].x][mov[i].y], t2 = fmap[mov[i].xx][mov[i].yy];
		TCB.MoveBoard(mov[i]);
		score = max(score, -Negamax(depth + 1));
		fmap[mov[i].x][mov[i].y] = t1, fmap[mov[i].xx][mov[i].yy] = t2;
	}
	cerr(3) << "Score:" << score << endl;
	cerra(3) << "}\n";
	return score;
}

Cmove NMSearch(int depth)
{
	memcpy(fmap, map, sizeof(map));
	Chessboard TCB(CB.fork(fmap));
	Cmove mmov(-1, -1, -1, -1, -1);
	vector <Cmove> mov = TCB.GenerateMoves();
	if (mov.empty()) return mmov;
	cerr(3) << "NMSearch(depth " << depth << "):{\n";
	TCB.print(3);
	int score = -INF;
	for (int i = 0; i < mov.size(); i++)
	{
		mov[i].print(2);
		int t1 = fmap[mov[i].x][mov[i].y], t2 = fmap[mov[i].xx][mov[i].yy];
		TCB.MoveBoard(mov[i]);
		int t = -Negamax(depth + 1);
		if (t > score)
		{
			score = t;
			mmov = mov[i];
		}
		fmap[mov[i].x][mov[i].y] = t1, fmap[mov[i].xx][mov[i].yy] = t2;
	}
	cerr(3) << "Max score:" << score << endl;
	return mmov;
}

Cmove Alphabeta(int depth, int alpha, int beta)
{
	cerr(3) << "Alpha-Beta(depth " << depth << "):{\n";
	vector <Cmove> mov0 = CB.GenerateMoves();
	if(mov0.empty())
	{
		cout << "GG";
		end();
	}
	cerra(3) << "}\n";
}

void make_decision(int &x, int &y, int &xx, int &yy)
{
	st = clock();
	Cmove t = NMSearch(1);
	if(t.x == -1)
	{
		cout << "GG";
		end();
	}
	x = t.x;
	y = t.y;
	xx = t.xx;
	yy = t.yy;
	cerr(2) << "printing valid moves:";
	t.print(2);
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
			cout << "Imp-Trial-Probe" << endl;
			end();
		}
		else if (op == "refresh")
		{
			get_init();
			CB.cid = id;
			CB.refresh();
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
			CB.refresh();
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
