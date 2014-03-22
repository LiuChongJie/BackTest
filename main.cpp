#include "Data.h"
#include <list>
#pragma warning(disable:4996)

using namespace std;

// 文件目录
char gIdxFileName[]  = "D:/RData/HouhaoData/Sets.txt";
char gCodeFileName[] = "D:/RData/HouhaoData/afterCombineStockName.txt";
char gWtFileName[]   = "D:/RData/HouhaoData/StkWeights.txt";
char gDataPathName[] = "D:/RData/HouhaoData/StkData";
char gMktPathName[]  = "D:/RData/HouhaoData/StkMktData";
char gMdlPathName[]  = "D:/RData/HouhaoData/ModelData";
char gWtMdlPathName[]  = "D:/RData/HouhaoData/WtModelData";
// 缓存天数
int  gNumDays = 5;

// 权重表 stkCode, dt, weight
map<int, map<int, double> > gWtTable;
// HS300指数成分 NOTE: 000300在其中，这是一个指数，应该去掉！
map<int, set<int> > gIdxSets;
// HS300成分股名称表
vector<int> gStkCodes;
// 连续若干天的数据
list<map<int, CStkDailyData> > gData;

int main()
{
	// 读取HS300成分股名称表，必须在read_idx_set之前！
	read_idx_code(gStkCodes, gCodeFileName);
	// 读取HS300指数成分，需要使用gStkCodes的数据
	read_idx_set(gIdxSets, gIdxFileName);
	// 读取权重数据
	read_weight_file(gWtTable, gWtFileName);	
	

	// 将每日行情数据放在一个文件中
	for(map<int, set<int> >::iterator iter = gIdxSets.begin(); iter != gIdxSets.end(); ++iter){
		set<int> smpSets(iter->second);
		map<int, CStkDailyData> dayData;
		CStkDailyMergeData dayMergeData;
		CModelData modelData;
		read_daily_data(dayData, iter->second, iter->first);		
		
		tran_merge_data(dayData, dayMergeData);

		char flName[64]; memset(flName, 0, 64);
		sprintf(flName, "%s/%d.txt", gMktPathName, iter->first);
		write_merge_data(flName, dayMergeData);

		tran_model_data(modelData, dayMergeData);
		memset(flName, 0, 64); sprintf(flName, "%s/%d.txt", gMdlPathName, iter->first);
		write_model_data(flName, modelData);

		memset(flName, 0, 64); sprintf(flName, "%s/%d.txt", gWtMdlPathName, iter->first);
		write_model_weight_data(flName, modelData, iter->first);
	}
	//CStkDailyMergeData dayMergeData;
	//CModelData modelData;
	//read_merge_data(dayMergeData, "D:/RData/HouhaoData/StkMktData/20100402.txt");
	//tran_model_data(modelData, dayMergeData);
	//write_model_data("D:/RData/HouhaoData/ModelData/20100402.txt", modelData);

	return 0;
}
