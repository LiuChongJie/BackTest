#include "Data.h"

#pragma warning(disable:4996)

using namespace std;

// 采样时间
const int TimeStamp[] = {94500, 100000, 101500, 103000, 104500, 110000, 111500, 113000, 131500, 133000, 134500, 140000, 141500, 143000, 144500, 145900, 240000};
const int GIndTS = 17;

// 读取权重数据 stk, dt, weight   weight = map[stkCode][dt]
int read_weight_file(map<int, map<int, double> >& wtTable, char* flName)
{
	int num = 0;
	char line[256];
	memset(line, 0, 256);
	int stkCode;
	int dt;
	double wt;
	FILE* fp = fopen(flName, "r");
	if(NULL == fp)
		return -1;
	while(fgets(line, 256, fp)){
		sscanf(line, "%d %d %lf", &stkCode, &dt, &wt);
		wtTable[stkCode][dt] = wt;
		memset(line, 0, 256);
		++num;
	}
	fclose(fp);
	printf("%s Weights file have been read\n", flName);
	return  num;
}
int read_idx_code(vector<int>& stkCodes, char* flName)
{
	int num = 0;
	char line[16];
	memset(line, 0, 16);
	int stkCode;
	FILE* fp = fopen(flName, "r");
	if(NULL == fp)
		return -1;
	while(fgets(line, 16, fp)){
		sscanf(line, "%6d", &stkCode);
		stkCodes.push_back(stkCode);
		memset(line, 0, 16);
		++num;
	}
	fclose(fp);
	printf("%s Stock code's Names have been read\n", flName);
	return num;
}

set<int> get_stk_code_set(char* str)
{
	int num = 0;
	set<int> res;
	int nCode = gStkCodes.size();
	int n = strlen(str);
	char* pch = strtok(str, "\t");
	while(NULL != pch){
		pch = strtok(NULL, "\t");
		if(NULL != pch && num < nCode && pch[0] == '1'){
			res.insert(gStkCodes[num]);
		}
		// 000300在其中，这是一个指数，应该去掉！
		res.erase(300);
		++num;
	}
	return res;
}

// 读取HS300指数成分
int read_idx_set(map<int, set<int> >& idxSets, char* flName)
{
	int num = 0;
	char line[2048];
	memset(line, 0, 2048);
	int dt, stkCode;
	FILE* fp = fopen(flName, "r");
	if(NULL == fp)
		return -1;
	while(fgets(line, 2048, fp) /* && num <= 1 */){	//TODO 只读取部分数据，测试结束后取消
		sscanf(line, "%d %d", &dt, &stkCode);
		idxSets[dt] = get_stk_code_set(line);		
		memset(line, 0, 2048);
		++num;
	}
	fclose(fp);
	printf("%s HS300 index component stocks' name have been read\n", flName);
	return num;
}


// 读取所有当日单一个股行情
int read_stk_daily_data(CStkDailyData& stkData, int stkCode, int dt)
{
	int num = 0;
	char line[256];
	memset(line, 0, 256);
	int tm;
	double prc;
	char strName[64];
	memset(strName, 0, 64);
	sprintf(strName, "%s/%8d/%06d.txt", gDataPathName, dt, stkCode);
	//printf("%s\n", strName);
	FILE* fp = fopen(strName, "r");
	if(NULL == fp)
		return -1;
	while(fgets(line, 256, fp)){
		sscanf(line, "%d\t%lf", &tm, &prc);
		// 此处将时间除以1000，变成秒为单位，便于后续处理
		// 且只读取合理时间内的价格数据
		tm /= 1000;	
		if(tm >= 92500 && tm <= 150000 && prc > 0.001){
			//tm /= 100;
			stkData.tm.push_back(tm);
			stkData.prc.push_back(prc);
		}
		++num;
	}
	// 标注更新角标，供后续将全部个股合并为一个数据表，模拟行情到来情况使用
	stkData.UpdateLoc = 0;
	fclose(fp);	
	return num;
}

// 读取所有当日所有个股行情
int read_daily_data(map<int,  CStkDailyData>& dayData, set<int>& stkCodes, int dt)
{
	for(set<int>::iterator iter = stkCodes.begin(); iter != stkCodes.end(); ++iter){
		CStkDailyData dat;
		read_stk_daily_data(dat, *iter, dt);
		dayData.insert(make_pair(*iter, dat));
	}
	if(!dayData.begin()->second.tm.empty())
		printf("%d 's all data files have been read\n", dt);
	//getchar();
	return stkCodes.size();
}

// 将当日所有数据合并为一张数据表，模仿行情到来时的情形
int tran_merge_data(map<int, CStkDailyData>& stkData, CStkDailyMergeData& stkMergeData)
{
	map<int, Crec> tmpData;
	int idxCode = 0;
	for(map<int, CStkDailyData>::iterator iter = stkData.begin(); iter != stkData.end(); ++iter){
		iter->first;
		int sCode = iter->first;
		int N = iter->second.tm.size();
		for(int i = 0; i < N; ++i){
			Crec rec;
			rec.tm     = iter->second.tm[i];
			rec.stkCode = sCode;
			rec.prc = iter->second.prc[i];
			// 101112 + 601235 => 101112 1235 2^31 = 2147483648
			// 上海的票，取后四位，且加上5000，确保和深圳不会重复
			idxCode = (sCode >= 600000) ? (5000 + sCode % 10000) : (sCode);
			idxCode += 10000*rec.tm;
			tmpData.insert(make_pair(idxCode , rec));
		}
	}
	for(map<int, Crec>::iterator iter = tmpData.begin(); iter != tmpData.end(); ++iter){
		stkMergeData.tm.push_back(iter->second.tm);
		stkMergeData.stk.push_back(iter->second.stkCode);
		stkMergeData.prc.push_back(iter->second.prc);
	}
	return 0;
}


int write_merge_data(char* flName, CStkDailyMergeData& stkMergeData)
{
	if(stkMergeData.tm.empty())
		return 0;

	FILE* fp = fopen(flName, "w");
	if(NULL == fp)
		return -1;
	
	int Num = stkMergeData.stk.size();
	for(int i = 0; i < Num; ++i){
		fprintf(fp, "%09d\t%06d\t%8.2f\n", stkMergeData.tm[i], stkMergeData.stk[i], stkMergeData.prc[i]);
	}
	fclose(fp);
	return Num;
}
int read_merge_data(CStkDailyMergeData& stkMergeData, char* flName)
{
	int num = 0;
	char line[64]; memset(line, 0, 64);
	int tm, stk;
	double prc;
	FILE* fp = fopen(flName, "r");
	while(fgets(line, 64, fp)){
		sscanf(line, "%d\t%d\t%lf", &tm, &stk, &prc);
		stkMergeData.tm.push_back(tm);
		stkMergeData.stk.push_back(stk);
		stkMergeData.prc.push_back(prc);
		memset(line, 0, 64);
		++num;
	}
	fclose(fp);
	return num;
}

int tran_model_data(CModelData& modelData, CStkDailyMergeData& stkMktData)
{
	if(stkMktData.tm.empty())
		return 0;

	// 获取第一条数据
	map<int, double> mktDataFeed;
	int N = stkMktData.tm.size();
	int tInd = 0;
	int UpdateTm = TimeStamp[tInd];
	// 价格初始化，实盘中，不会存在问题，应该都能取到当日开盘价！
	int tm      = stkMktData.tm[0];
	int stkCode = stkMktData.stk[0];
	double prc  = stkMktData.prc[0];
	int i = 0;
	map<int, double>::iterator iter;

	while(stkMktData.tm[i] < UpdateTm){
		tm      = stkMktData.tm[i];
		stkCode = stkMktData.stk[i];
		prc     = stkMktData.prc[i];
		mktDataFeed[stkCode] = prc;
		++i;
	}
	while(i < N){
		tm      = stkMktData.tm[i];
		stkCode =stkMktData.stk[i];
		prc     = stkMktData.prc[i];
		if(tm > UpdateTm){
			// 获取当前所有个股最新报价，生成模型数据的一条记录
			modelData[UpdateTm] = mktDataFeed;
			UpdateTm = TimeStamp[++tInd];			
		}
		// 更新对应个股的行情
		iter = mktDataFeed.find(stkCode);
		if(iter != mktDataFeed.end()){
			iter->second = prc;
		}
		++i;
	}

	return modelData.size();
}

int write_model_data(char* flName, CModelData& modelData)
{
	if(modelData.empty())
		return 0;

	FILE* fp = fopen(flName, "w");
	if(NULL == fp)
		return -1;
	// 行名
	fprintf(fp, "TIME");
	map<int, double> mktData(modelData.begin()->second);
	for(map<int, double>::iterator iter = mktData.begin(); iter != mktData.end(); ++iter){
		fprintf(fp, "\t S%06d", iter->first);
	}
	fprintf(fp, "\n");

	// 数据内容
	for(CModelData::iterator iter = modelData.begin(); iter != modelData.end(); ++iter){
		fprintf(fp, "%04d", iter->first);
		for(map<int, double>::iterator iterIn = iter->second.begin(); iterIn != iter->second.end(); ++iterIn){
			fprintf(fp, "\t%8.2f", iterIn->second);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	return modelData.size();
}

int write_model_weight_data(char* flName, CModelData& modelData, int dt)
{
	double wt = 1.0;
	if(modelData.empty())
		return 0;

	FILE* fp = fopen(flName, "w");
	if(NULL == fp)
		return -1;
	// 行名
	fprintf(fp, "TIME");
	map<int, double> mktData(modelData.begin()->second);
	for(map<int, double>::iterator iter = mktData.begin(); iter != mktData.end(); ++iter){
		fprintf(fp, "\t S%06d", iter->first);
	}
	fprintf(fp, "\n");

	map<int, map<int, double> >::iterator IterStkWt;
	map<int, double>::iterator IterWt;
	// 数据内容
	for(CModelData::iterator iter = modelData.begin(); iter != modelData.end(); ++iter){
		fprintf(fp, "%04d", iter->first);
		for(map<int, double>::iterator iterIn = iter->second.begin(); iterIn != iter->second.end(); ++iterIn){
			IterStkWt = gWtTable.find(iterIn->first);
			if(IterStkWt != gWtTable.end()){
				IterWt = IterStkWt->second.find(dt);
				if(IterWt != IterStkWt->second.end())
					wt = IterWt->second;
			}
			fprintf(fp, "\t%12.6f", iterIn->second * wt);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	return modelData.size();
}