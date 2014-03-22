#include "Data.h"
#include <list>
#pragma warning(disable:4996)

using namespace std;

// �ļ�Ŀ¼
char gIdxFileName[]  = "D:/RData/HouhaoData/Sets.txt";
char gCodeFileName[] = "D:/RData/HouhaoData/afterCombineStockName.txt";
char gWtFileName[]   = "D:/RData/HouhaoData/StkWeights.txt";
char gDataPathName[] = "D:/RData/HouhaoData/StkData";
char gMktPathName[]  = "D:/RData/HouhaoData/StkMktData";
char gMdlPathName[]  = "D:/RData/HouhaoData/ModelData";
char gWtMdlPathName[]  = "D:/RData/HouhaoData/WtModelData";
// ��������
int  gNumDays = 5;

// Ȩ�ر� stkCode, dt, weight
map<int, map<int, double> > gWtTable;
// HS300ָ���ɷ� NOTE: 000300�����У�����һ��ָ����Ӧ��ȥ����
map<int, set<int> > gIdxSets;
// HS300�ɷֹ����Ʊ�
vector<int> gStkCodes;
// ���������������
list<map<int, CStkDailyData> > gData;

int main()
{
	// ��ȡHS300�ɷֹ����Ʊ�������read_idx_set֮ǰ��
	read_idx_code(gStkCodes, gCodeFileName);
	// ��ȡHS300ָ���ɷ֣���Ҫʹ��gStkCodes������
	read_idx_set(gIdxSets, gIdxFileName);
	// ��ȡȨ������
	read_weight_file(gWtTable, gWtFileName);	
	

	// ��ÿ���������ݷ���һ���ļ���
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
