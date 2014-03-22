// ���ݸ�ʽ����
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

// �ļ�Ŀ¼
extern char gWtFileName[]   ;
extern char gCodeFileName[];
extern char gIdxFileName[]  ;
extern char gDataPathName[] ;
extern char gMktPathName[]  ;
extern int  gNumDays;


// Ȩ�ر� stkCode, dt, weight
extern map<int, map<int, double> > gWtTable;
// HS300ָ���ɷ�
extern map<int, set<int> > gIdxSets;
// HS300�ɷֹ����Ʊ�
extern vector<int> gStkCodes;

// ��ȡȨ������ stk, dt, weight   weight = map[stkCode][dt]
int read_weight_file(map<int, map<int, double> >& wtTable, char* flName);
// ��ȡÿ��HS300ָ���ɷ�
int read_idx_set(map<int, set<int> >& idxSets, char* flName);
// ��ȡHS300ָ���ɷֹ�����
int read_idx_code(vector<int>& stkCodes, char* flName);
// ��������������
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
// ���ڵ�һ�������¼
class Crec
{
public:
	int tm;
	int stkCode;
	double prc;
};
// ���и�������������
int read_stk_daily_data(CStkDailyData& stkData, int stkCode, int dt);
// ��ȡ���е������и�������
int read_daily_data(map<int,  CStkDailyData>& dayData, set<int>& stkCodes, int dt);
// �������������ݺϲ�Ϊһ�����ݱ�ģ�����鵽��ʱ������
int tran_merge_data(map<int, CStkDailyData>& stkData, CStkDailyMergeData& stkMergeData);
int write_merge_data(char* flName, CStkDailyMergeData& stkMergeData);
int read_merge_data(CStkDailyMergeData&stkMergeData, char* flName);
// tm stkCode prc
typedef map<int, map<int, double> > CModelData;
// ����������ת��Ϊ15���Ӳ���������
int tran_model_data(CModelData& modelData, CStkDailyMergeData& stkMktData);
int write_model_data(char* flName, CModelData& modelData);
int write_model_weight_data(char* flName, CModelData& modelData, int dt);
#endif