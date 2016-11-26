#include <ctime>
#include <cstdlib>
#include <vector>
#include <queue>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>
#define LOGLVL 5
#define DYMLYR 0
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
int map[H][W] = { -2};
int fmap[H][W];
int map_score[2][H][W] = {0};
int flag[2];
int peacemov = 0;
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
int bv[TOTALKIND] = {10, 9, 8, 7, 6, 5, 4, 3, 6, 7, 8, 150};
int bnum[TOTALKIND] = {1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 2, 1};

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
		int k;
		Cpiece(): x(0), y(0), k(-1) {}
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
			return this->rst < t.rst;
		}
		Cmove(): x(0), y(0), xx(0), yy(0) {}
		Cmove(int _x, int _y, int _xx, int _yy, int _rst): x(_x), y(_y), xx(_xx), yy(_yy), rst(_rst) {}
		void print(int l)
		{
			cerra(l) << "mov:[" << x << "," << y << "," << xx << "," << yy << "," << rst << "]" << endl;
		}
};

class Chessboard
{
	public:
		int cid;
		int (&cmap)[H][W];
		Chessboard(): cid(0), cmap(::map) {}
		Chessboard(int _cid, int (&_cmap)[H][W]):
			cid(_cid), cmap(_cmap) {}
		Chessboard fork(int (&m)[H][W]) const
		{
			return Chessboard(cid, m);
		}
//		void refresh_list()
//		{
//			priority_queue <Cpiece> tpcs;
////			cerr(0) << "CB " << cid << ":Refreshing list {" << endl;
//			for (int x = 0; x < H; x++)
//			{
//				for (int y = 0; y < W; y++)
//				{
//					if (cmap[x][y] != -2 && GetFlag(x, y) == cid)
//					{
//						tpcs.push(Cpiece(x, y));
////						cerra(0) << "found " << GetKind(x, y) << " " << x << " " << y << endl;
//					}
//				}
//			}
////			pcnt[cid] = tpcs.size();
////			cerra(0) << "id " << cid << ":" << pcnt[cid] << endl;
////			cerra(0) << "}" << endl;
//		}
		void MoveBoard(Cmove mov)
		{
//			if (!IsValidMove(mov))
//			{
//				cerr << "Boom" << endl;
//				cout << "halt";
//			}
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
					if(0 <= cy && cy < W && 0 <= cx && cx < H && !vmap[cx][cy] && ((i > 1 && IsOnHRail(cx, cy)) || (i < 2 && IsOnVRail(cx, cy))))
					{
						if (cx == xx && cy == yy)
						{
							cerra(0) << "(bfs: " << l << " " << r << ")=Y" << endl;
							return true;
						}
						if (cmap[cx][cy] != -2) continue;
						qx[r] = cx;
						qy[r] = cy;
						vmap[cx][cy] = 1;
						r++;
					}
				}
				l++;
			}
			cerra(0) << "(bfs: " << l << " " << r << ")=N" << endl;
			return false;
		}
		inline bool IsFlagBlocked(int tid)
		{
			int fx = i2x(flag[tid]), fy = i2y(flag[tid]);
			for (int i = 0; i < 8; i++)
			{
				int cx = fx + DX[i], cy = fy + DY[i];
				if (exist(cx, cy) && cmap[cx][cy] == -2) return false;
			}
			return true;
		}
		int PValue(int x, int y)
		{
			if ((mapi(x, y) == base[!cid * 2] || mapi(x, y) == base[!cid * 2 + 1]) && GetKind(x, y) != 11) return -INF / 2;
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
//			cerra(0) << "#id " << cid << " #o " << cmap[x][y] << " #t " << cmap[xx][yy] << " " << typ << " " << tgtflg << " " << objflg << ")validating.0.";
			bool t = ((x == xx && y == yy) || (!exist(x, y)) || (!exist(xx, yy)) || (cmap[x][y] == -2) || (objflg != cid) || (( x == 0 ||  x == 16) && (y == 1 || y == 3)) || (cmap[xx][yy] != -2 && IsCamp(xx, yy)) || ((cmap[xx][yy] != -2) && (tgtflg == cid)) || (typ == 9) || (typ == 11));
			if (t) return false;
			int dx = xx - x, dy = yy - y;
//			cerra(0) << "1.";
			if (!dx && (dy == 1 || dy == -1)) return true;
//			cerra(0) << "2.";
			if (!dy && (dx == 1 || dx == -1)) return true;
//			cerra(0) << "3.";
			if ((dx == 1 && (dy == -1 || dy == 1)) && IsNWE(x, y)) return true;
//			cerra(0) << "4.";
			if ((dx == -1 && (dy == -1 || dy == 1)) && IsSWE(x, y)) return true;
//			cerra(0) << "5.";
			if (IsOnHRail(x, y) && IsOnHRail(xx, yy) && IsHReach(x, y, xx, yy)) return true;
//			cerra(0) << "6.";
			if (IsOnVRail(x, y) && IsOnVRail(xx, yy) && IsVReach(x, y, xx, yy)) return true;
//			cerra(0) << "7.";
			if (typ == 8 && IsOnRail(x, y) && IsOnRail(xx, yy) && IsReach(x, y, xx, yy)) return true;
			return false;
		}
		void GenerateMoves(priority_queue <Cmove> &move)
		{
//			cerr(1) << "CB " << cid << ":Generating moves:{" << endl;
			for (int x = 0; x < H; x++)
			{
				for (int y = 0; y < W ; y++)
				{
					for (int xx = 0; xx < H; xx++)
					{
						for (int yy = 0; yy < W ; yy++)
						{
//							cerra(0) << "Trial " << " : " << x << ", " << y << " to " << xx << ", " << yy << "..(";
							if (IsValidMove(x, y, xx, yy))
							{
//								cerra(0) << "valid" << endl;
//								cerra(1) << "IsKill:" << IsKill(x, y, xx, yy) << " IsKilled:" << IsKilled(x, y, xx, yy) << " ValueSub:" << Value(x, y) << " ValueObj:" << Value(xx, yy) << endl;
								int r = map_score[cid][xx][yy] + Value(xx, yy) * IsKill(x, y, xx, yy) - Value(x, y) * IsKilled(x, y, xx, yy);
								move.push(Cmove(x, y, xx, yy, r));
//								cerra(1) << "push " << x << " " << y << " " << xx << " " << yy << " " << 0 << endl;
							}
//							else cerra(0) << "invalid" << endl;
						}
					}
				}
			}
			cerra(1) << "}\n";
		}
		int Evaluate()
		{
			int score[2] = {0};
			int pcnt[2] = {0};
			int pkind[2][TOTALKIND] = {0};
			Cpiece pcs[2][TOTALNUM];
			for (int x = 0; x < H; x++)
			{
				for (int y = 0; y < W; y++)
				{
					if (cmap[x][y] != -2)
					{
						int f = GetFlag(x, y), t = GetKind(x, y);
						pcs[f][pcnt[f]].x = x;
						pcs[f][pcnt[f]].y = y;
						pcs[f][pcnt[f]].k = t;
//						cerr(0) << "found " << t << " " << f << " " << x << " " << y << endl;
						pcnt[f]++;
						pkind[f][t]++;
					}
				}
			}
			for (int j = 0; j < 2; j++)
			{
				for (int i = 0; i < pcnt[j]; i++)
				{
					int k = pcs[j][i].k;
					score[j] += (1 + pkind[j][k]) / bnum[k] * bv[k] * (500 + map_score[j][pcs[j][i].x][pcs[j][i].y]);
//					score += bv[pcs[j][i].k] * 500;
				}
				if (pkind[j][9] < 3) score[j] -= 1500;
				if (IsFlagBlocked(j)) score[j] += 5000;
			}
//			for (int i = 0; i < pcnt[cid]; i++)
//			{
//				score += bv[pcs[cid][i].k] * (500 + map_score[cid][pcs[cid][i].x][pcs[cid][i].y]);
////				score += bv[pcs[cid][i].k] * 500;
//			}
			return score[cid] - score[!cid];
//			return score[cid];
		}
		inline bool IsGameover()
		{
			if (GetKind(i2x(flag[0]), i2y(flag[0])) != 11 || GetKind(i2x(flag[1]), i2y(flag[1])) != 11) return true;
			return false;
		}
		void print(int l)
		{
			cerra(l) << "CB " << cid << ":{" << endl;
			for (int i = 0; i < H; i++)
			{
				for (int j = 0; j < W; j++)
				{
					cerra(l) << cmap[i][j] << " ";
				}
				cerra(l) << endl;
			}
			cerra(l) << "}" << endl;
		}
};
Chessboard CB;

void change()
{
	int x, y, xx, yy, col, kind;
	cin >> x >> y >> xx >> yy >> col >> kind;
	peacemov++;
	if (CB.IsKill(x, y, xx, yy) || CB.IsKilled(x, y, xx, yy)) peacemov = 0;
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
	int opt[25] = {9, 11, 9, 2, 2, 10, 9, 10, 4, 4, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 1, 0, 3, 3};
//	int opt[25] = {9, 11, 9, 7, 8, 7, 9, 8, 8, 6, 4, 10, 4, 5, 6, 6, 5, 3, 10, 3, 0, 2, 1, 7, 2};
//	int opt[25] = {9, 11, 9, 7, 6, 7, 9, 8, 7, 6, 4, 6, 4, 5, 10, 8, 5, 3, 10, 3, 0, 2, 1, 8, 2};
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

int tcnt = 0, ecnt = 0, prcnt = 0, tnsum = 0;
int AlphaBeta(int depth, int alpha, int beta, bool f)
{
//	Chessboard TCB(CB.fork(fmap));
	Chessboard TCB((f ? id : !id), fmap);
//	TCB.cid = f ? id : !id;
//	TCB.print(1);
	int score = 0;
	if (depth <= 0 || TCB.IsGameover())
	{
		score = TCB.Evaluate();
		cerra(1 + depth) << "#0 Score:" << score << " Alpha:" << alpha << " Beta:" << beta << endl;
		tcnt++;
		return score;
	}
	cerra(1 + depth) << "AlphaBeta(depth " << depth << "):{\n";
	priority_queue <Cmove> move;
	TCB.GenerateMoves(move);
	if (move.empty())
	{
		score = TCB.Evaluate();
		cerra(1 + depth) << "#1 Score:" << score << " Alpha:" << alpha << " Beta:" << beta << endl;
		cerra(1 + depth) << "}\n";
		ecnt++;
		return score;
	}
	cerra(1 + depth) << "Moves:" << move.size() << endl;
	while (!move.empty())
	{
		Cmove mov = move.top();
		mov.print(1 + depth);
		int t1 = fmap[mov.x][mov.y], t2 = fmap[mov.xx][mov.yy];
		TCB.MoveBoard(mov);
		score = -AlphaBeta(depth - 1, -beta, -alpha, !f);
		cerra(1 + depth) << "#Score:" << score << endl;
		fmap[mov.x][mov.y] = t1, fmap[mov.xx][mov.yy] = t2;
		if (score >= beta)
		{
			cerra(1 + depth) << "#2 Score:" << score << " Alpha:" << alpha << " Beta:" << beta << endl;
			cerra(1 + depth) << "}\n";
			prcnt++;
			return beta;
		}
		if (score > alpha)
		{
			alpha = score;
		}
		move.pop();
	}
	cerra(1 + depth) << "#3 Score:" << score << " Alpha:" << alpha << " Beta:" << beta << endl;
	cerra(1 + depth) << "}\n";
	return alpha;
}

Cmove ABSearch(int depth)
{
	memcpy(fmap, map, sizeof(map));
	Chessboard TCB(CB.fork(fmap));
	Cmove mmov(-1, -1, -1, -1, -1);
	priority_queue <Cmove> move;
	TCB.GenerateMoves(move);
	cerra(1 + depth) << "ABSearch(depth " << depth << "):{\n";
	TCB.print(3);
	int score = -INF;
	while (!move.empty())
	{
		Cmove mov = move.top();
		mov.print(depth);
		if (mov.rst < -INF / 3)
		{
			move.pop();
			continue;
		}
		int t1 = fmap[mov.x][mov.y], t2 = fmap[mov.xx][mov.yy];
		TCB.MoveBoard(mov);
		if(TCB.IsGameover())
		{
			mmov = mov;
			break;
		}
		int ex = 0;
//		if (mov.rst > 3000) ex = 1;
		int t = -AlphaBeta(ex + depth - 1, -INF, INF, 0);
		cerra(1 + depth) << "#Score:" << t << endl;
		if (t > score)
		{
			score = t;
			mmov = mov;
		}
		fmap[mov.x][mov.y] = t1, fmap[mov.xx][mov.yy] = t2;
		move.pop();
	}
	cerra(1 + depth) << "#Max score:" << score << endl;
	cerra(1 + depth) << "}\n";
	return mmov;
}
double tmin = 5, tmax = 0, tsum = 0, tavg = 0;

Cmove Naive()
{
	priority_queue <Cmove> mov;
	CB.GenerateMoves(mov);
	return mov.top();
}

void make_decision(int &x, int &y, int &xx, int &yy)
{
	tcnt = ecnt = prcnt = 0;
	st = clock();
	Cmove t;
	if (peacemov >= 20)
	{
		cerr(5) << "Naive move!" << endl;
		t = Naive();
	}
	else
	{
#if DYMLYR == 1
		int m = 0;
		while (30 * get_time() < 1)
		{
			m++;
			t = ABSearch(m);
		}
		cerr(5) << "Layers: " << m << endl;
#else
		t = ABSearch(3);
#endif
	}
//	if(t.x == -1)
//	{
//		cout << "GG";
//		end();
//	}
	x = t.x;
	y = t.y;
	xx = t.xx;
	yy = t.yy;
//	cerr(2) << "printing valid moves:";
//	t.print(2);
	double tm = get_time();
	tmin = min(tmin, tm);
	tmax = max(tmax, tm);
	tsum += tm;
	tnsum += tcnt;
	tavg = tsum / rounds;
	cerr(5) << "[CT/Min/Max/Avg/TC/EC/PC/Spd:" << tm << "/" << tmin << "/" << tmax << "/" << tavg << "/" << tcnt << "/" << ecnt << "/" << prcnt << "/" << tsum / tnsum  << "]" << endl;
}

void MapScore()
{
	for (int i = 0; i < 2; i++)
	{
		int fx = i2x(flag[!i]), fy = i2y(flag[!i]);
		for (int x = 0; x < H; x++)
		{
			for (int y = 0; y < W; y++)
			{
				if (!exist(x, y) || CB.GetKind(x, y) == 11) continue;
				map_score[i][x][y] = 1000 / Dist(x, y, fx, fy);
			}
		}
		for (int k = 0; k < 4; k++)
		{
			int cx = fx + DX[k], cy = fy + DY[k];
			if (exist(cx, cy)) map_score[i][cx][cy] += 1500;
		}
		for (int c = 0; c < 10; c++)
		{
			map_score[i][i2x(camp[c])][i2y(camp[c])] += 150;
		}
		for (int c = !i * 5; c < !i * 5 + 5; c++)
		{
			map_score[i][i2x(camp[c])][i2y(camp[c])] += 100;
		}
		for (int c = 0; c < 4; c++)
		{
			map_score[i][i2x(base[c])][i2y(base[c])] = -INF / 2;
		}
		for (int c = i * 2; c < i * 2 + 2; c++)
		{
			map_score[i][i2x(base[c])][i2y(base[c])] = 0;
		}
		map_score[i][fx][6 - 2 * fy] = -50;
		map_score[i][fx][2] += 1000;
		map_score[i][fx][fy] = INF / 2;
	}

	for (int i = 0; i < 2; i++)
	{
		cerr << "id:" << i << endl;
		for (int x = 0; x < H; x++)
		{
			for (int y = 0; y < W; y++)
			{
				cerr << map_score[i][x][y] << " ";
			}
			cerr << endl;
		}
		cerr << endl;
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
			cout << "Imp-Trial-AB-RC-III" << endl;
			end();
		}
		else if (op == "refresh")
		{
			get_init();
			CB.cid = id;
			flag[0] = (map[0][1] == 11) ? 1 : 3;
			flag[1] = (map[16][1] == 11) ? mapi(16, 1) : mapi(16, 3);
			MapScore();
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
			rounds++;
			cerr(5) << "Round " << rounds << ": {" << endl;
			make_decision(x, y, xx, yy);
			cerra(5) << "}" << endl;
			cerr(5) << "Action: " << x << " " << y << " " << xx << " " << yy << endl;
			cout << x << " " << y << " " << xx << " " << yy << endl;
			end();
		}
	}
}
