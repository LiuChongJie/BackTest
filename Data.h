// 数据格式定义
#ifndef _MKT_DATA_H_
#define _MKT_DATA_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

// 文件目录
extern char gWtFileName[]   ;
extern char gCodeFileName[];
extern char gIdxFileName[]  ;
extern char gDataPathName[] ;
extern char gMktPathName[]  ;
extern int  gNumDays;


// 权重表 stkCode, dt, weight
extern map<int, map<int, double> > gWtTable;
// HS300指数成分
extern map<int, set<int> > gIdxSets;
// HS300成分股名称表
extern vector<int> gStkCodes;

// 读取权重数据 stk, dt, weight   weight = map[stkCode][dt]
int read_weight_file(map<int, map<int, double> >& wtTable, char* flName);
// 读取每日HS300指数成分
int read_idx_set(map<int, set<int> >& idxSets, char* flName);
// 读取HS300指数成分股名称
int read_idx_code(vector<int>& stkCodes, char* flName);
// 个股日行情数据
class CStkDailyData
{
public:
	vector<int> tm;
	vector<double> prc;
	int UpdateLoc;
};
class CStkDailyMergeData
{
public:
	vector<int> stk;
	vector<int> tm;
	vector<double> prc;
};
// 日内的一条行情记录
class Crec
{
public:
	int tm;
	int stkCode;
	double prc;
};
// 所有个股日行情数据
int read_stk_daily_data(CStkDailyData& stkData, int stkCode, int dt);
// 读取所有当日所有个股行情
int read_daily_data(map<int,  CStkDailyData>& dayData, set<int>& stkCodes, int dt);
// 将当日所有数据合并为一张数据表，模仿行情到来时的情形
int tran_merge_data(map<int, CStkDailyData>& stkData, CStkDailyMergeData& stkMergeData);
int write_merge_data(char* flName, CStkDailyMergeData& stkMergeData);
int read_merge_data(CStkDailyMergeData&stkMergeData, char* flName);
// tm stkCode prc
typedef map<int, map<int, double> > CModelData;
// 将日内行情转化为15分钟采样的数据
int tran_model_data(CModelData& modelData, CStkDailyMergeData& stkMktData);
int write_model_data(char* flName, CModelData& modelData);
int write_model_weight_data(char* flName, CModelData& modelData, int dt);
#endif