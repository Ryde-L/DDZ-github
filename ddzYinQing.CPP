//#include "ddzv100.h"   2014 CAAI 基础策略
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#include "ddzZSC.h"  
#include<iostream>
#include<fstream>


//评估型策略 - （未用）  
//思路辅助
/*double CalCardsValue(int iPlaOnHand[])
{
int i;
double dSum = 100;			//估值
for(i=0;iPlaOnHand[i]>=0;i++)
{
dSum=dSum-5;			//手牌越少越好没多一张牌优势减5分
if (i >= 1 && iPlaOnHand[i - 1] / 4 == iPlaOnHand[i] / 4)
dSum = dSum + 2;	//相邻两牌同点加2分
if (i >= 4 && iPlaOnHand[i - 4] / 4 <=7
&&iPlaOnHand[i - 4] / 4 + 1 ==iPlaOnHand[i - 3] / 4
&&iPlaOnHand[i - 3] / 4 + 1 ==iPlaOnHand[i - 2] / 4
&&iPlaOnHand[i - 2] / 4 + 1 ==iPlaOnHand[i - 1] / 4
&&iPlaOnHand[i - 1] / 4 + 1 ==iPlaOnHand[i] / 4)
dSum = dSum + 6;	//2以下相邻五牌单顺加6分
}
return dSum;
}*/

void Log(char* s)
{
	ofstream fout;
	fout.open("log.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	fout << s << endl;
	fout.close();
}
void Log(char* title, char* s)
{
	ofstream fout;
	fout.open("log.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	fout << title << ":" << s << endl;
	fout.close();
}

void Log(char* title, int s)
{
	ofstream fout;
	fout.open("log.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	fout << title;
	fout << s << endl;
	fout.close();
}

/*叫牌计算函数，如果前面的人叫的分数大于等于我要叫的分数，我就叫0分，等于不叫*/
int CalBid(struct ddz * dp)
{
	pokersort(dp->iOnHand);
	int i, bid = 1, Flag1 = 0, Flag2 = 0;  //初始化bid=1， 防止另外两家都不叫的情况下游戏无法进行
	int sign = dp->cDir - 'A';        //记录位置

	for (i = 0; dp->iOnHand[i] >= 0; i++)
	{
		if (dp->iOnHand[i] >= 48 && dp->iOnHand[i] <= 51) Flag1++;
		else if (dp->iOnHand[i] == 52 || dp->iOnHand[i] == 53) Flag2++;
	}
	if (Flag1 == 4 || Flag2 == 2 || (Flag1 >= 3 && Flag2 == 1))
		bid = 3;  //对王 或 四个2 或 单王带三个以上的2 叫3分
	else if ((Flag1 >= 3 && Flag1 == 0) || (Flag1 == 2 && Flag2 == 1))
		bid = 2;  //三个以上的2 或 单王带两个2 叫2分
	else if ((Flag1 == 1 && Flag2 == 1) || (Flag1 == 2 && Flag2 == 0))
		bid = 1;  //单王带一个2 或 对2 叫1分
	for (i = 0; i<sign; i++)
		if (dp->iBid[i] >= bid)
			bid = 0;  //防止叫重分
	return bid;
}


/*出牌计算函数
计算要出的牌储存至 要出牌iToTable[]数组中
*/
void CalPla(struct ddz * dp)			//
{
	struct PaiXing dd, *px = &dd;
	int i, j, k = 0, iOnTableW[4000], iToTableW[4000];

	Lineup(dp); //阵容确定

	for (i = dp->iOTmax - 1; i >= 0; i--)
	{
		for (j = 0; dp->iOnTable[i][j] >= 0; j++, k++)
			iOnTableW[k] = dp->iOnTable[i][j];//二维数组转一维
		iOnTableW[k] = -1;//-1表手数间隔
		k++;
	}
	iOnTableW[k] = -2;//-2表结束

	pokersort(dp->iOnHand);//从小到大排序
	GameInit(dp->iOnHand, px);     //初始化
	Query(px);            	//手牌类方案初始化和分牌
	AllHand(px);            //计算手数
	helpdz(dp->iOnHand, iOnTableW, iToTableW, dp->iRestHandCard, dp->WarLine, px, dp);
	for (i = 0; iToTableW[i] >= 0; i++)
		dp->iToTable[i] = iToTableW[i];
	dp->iToTable[i] = -1;
}


/*处理叫牌信息*/
void GetBid(struct ddz * dp)
{
	if (dp->sCommandIn[4] == 'W')					//如果输入信息为BID WHAT
	{
		strcpy(dp->sCommandOut, "BID _0");
		dp->sCommandOut[4] = dp->cDir;
		dp->iBid[dp->cDir - 'A'] = CalBid(dp);		//调用叫牌函数
		dp->sCommandOut[5] = dp->iBid[dp->cDir - 'A'] + '0';
		dp->sCommandOut[6] = '\0';
	}
	if (dp->sCommandIn[4] >= 'A'&&dp->sCommandIn[4] <= 'C')  //输入信息为BID **
	{
		dp->iBid[dp->sCommandIn[4] - 'A'] = dp->sCommandIn[5] - '0';
		strcpy(dp->sCommandOut, "OK BID");
		//可在此处增加收集叫牌信息的数组便于计算使用
	}

}


/*PLAY 指令处理函数*/
void GetPla(struct ddz * dp)
{
	int iGeWei;//个位
	int iShiWei;//十位
	int i, j;
	if (dp->sCommandIn[5] == 'W')					//接收信息为PLAY WHAT：应调用出牌计算函数计算出牌
	{
		CalPla(dp);								//调用出牌计算函数计算出牌
		strcpy(dp->sCommandOut, "PLAY _");
		dp->sCommandOut[5] = dp->cDir;			//输出命令的预备信息准备到sCommandOut数组
		iTos(dp->iToTable, dp->sCommandOut, dp);		//要出牌数组iToTable[]中的数字转化为字符并连接到sCommandOut中
		TakeOff(dp->iOnHand, dp->iToTable);		//把打出的牌iToTable[] 从 手中的牌iOnHand[] 中剔除
		for (i = 0; dp->iToTable[i] >= 0; i++)			//把打出的牌iToTable[] 储存到 iOnTable[][]
		{
			dp->iOnTable[dp->iOTmax][i] = dp->iToTable[i];
		}
		dp->iOnTable[dp->iOTmax][i] = -1;
	}
	else										//否则收到信息为其它玩家出牌
	{

		iShiWei = -1;   iGeWei = -1;   j = 0;					//将另外两方所出的牌储存到iOnTable数组中
																//eg:PLAY C1,2,33,50
		if (dp->sCommandIn[6] == '-')
		{
			dp->iOnTable[dp->iOTmax][0] = -1;
		}
		else
		{
			for (i = 6; dp->sCommandIn[i - 1] != '\0'; i++)
			{
				if (dp->sCommandIn[i] != ',' && dp->sCommandIn[i] != '\0')
				{//减牌添加在这里
					if (iShiWei == -1)
						iShiWei = dp->sCommandIn[i] - '0';
					else
						iGeWei = dp->sCommandIn[i] - '0';
				}
				else
				{

					if (iGeWei == -1)
						dp->iOnTable[dp->iOTmax][j] = iShiWei;
					else
						dp->iOnTable[dp->iOTmax][j] = iShiWei * 10 + iGeWei;

					dp->iRestHandCard[dp->sCommandIn[5] - 'A']--;//剩余手牌数-1
					iShiWei = -1;   iGeWei = -1;
					j++;
				}
			}
			dp->iOnTable[dp->iOTmax][j] = -1;
		}
		//回复收到
		strcpy(dp->sCommandOut, "OK PLAY");
	}
	//当前手数加1
	dp->iOTmax++;
}


/*输入指令*/
void AIInput(struct ddz * dp)
{
	cin.getline(dp->sCommandIn, 80);
}


/*分析指令*/
void AICal(struct ddz * dp)
{
	char cShort[4];
	for (int i = 0; i<3; i++)
		cShort[i] = dp->sCommandIn[i];
	cShort[3] = '\0';
	if (strcmp(cShort, "DOU") == 0)					//版本信息
		GetDou(dp);
	if (strcmp(cShort, "INF") == 0)					//轮局信息
		GetInf(dp);
	if (strcmp(cShort, "DEA") == 0)					//牌套信息
		GetDea(dp);
	if (strcmp(cShort, "BID") == 0)					//叫牌过程
		GetBid(dp);
	if (strcmp(cShort, "LEF") == 0)					//底牌信息
		GetLef(dp);
	if (strcmp(cShort, "PLA") == 0)					//出牌过程
		GetPla(dp);
	if (strcmp(cShort, "GAM") == 0)					//胜负信息
		GetGam(dp);
}

/*输出指令*/
void AIOutput(struct ddz * dp)
{
	cout << dp->sCommandOut << endl;
}

/*指令空闲*/
void CalOther(struct ddz * dp)
{
	dp->iVoid = 0;
}

int main()
{
	struct ddz d, *dp = &d;
	init(dp);
	while (dp->iStatus != 0) {

		AIInput(dp);							//输入信息
		AICal(dp);								//分析并计算
		AIOutput(dp);							//输出信息
		CalOther(dp);							//闲暇时间利用
		Log("左边剩余牌：", dp->iRestHandCard[(dp->cDir - 'A' + 2) % 3]);
		Log("自己剩余牌：", dp->iRestHandCard[dp->cDir - 'A']);
		Log("右边剩余牌：", dp->iRestHandCard[(dp->cDir - 'A' + 1) % 3]);
	}
	return 0;
}
