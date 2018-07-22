#include <stdio.h> //8.17敲定版
#include <string.h>
#include <iostream>
#include <fstream>
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
using namespace std;

struct ddz
{
	int  iStatus;				//引擎状态-1错误,0结束,1开始
	char sCommandIn[80];		//通信输入内容
	char sCommandOut[80];		//通信输出内容
	int iOnHand[21];            //手中牌（所有值初始化为-1）
	int iOnTable[162][21];      //以出牌数组（所有值初始化为-2）每行是一手牌，以-1结尾，Pass记为-1
	int iToTable[21];           //要出的牌

	char sVer[80];			    //协议版本号
	char sName[80];		        //参赛选手称呼
	char cDir;			        //玩家方位编号
	char cLandlord;	            //地主玩家方位编号
	char cWinner;	            //胜利者方位编号
	int iBid[3];                //叫牌过程
	int iBmax;                  //当前叫牌数，值域{0,1,2,3}
	int iOTmax;                 //桌面牌手数
	int iRoundNow;              //当前局次
	int iRoundTotal;            //和总局数
	int iLevelUp;	            //晋级选手数
	int iScoreMax;	            //转换得分界限
	int iVoid;			        //闲暇并行计算参数
	int iTurnNow;               //当前轮次
	int iTurnTotal;             //总轮数

	int iRestHandCard[3];		//三家的剩余手牌
	int WarLine;                //阵容确定 1.【农 AI 农】 2.【友 AI 地】 3.【地 AI 友】
};



struct PaiXing {
	int ShouPai[21];       //暂存手牌 
	bool Rocket;             //有无火箭 1  52 53
	int ZhaDan[5][4];      //储存炸弹 2
	int DanPai[21];        //单张 3
	int DuiPai[10][2];     //对牌 4
	int SanZhang[6][3];    //三张 5
						   //三带一 三带二  6 
	int DanShun[4][15];    //储存单顺  0位为该顺有多少张 7
	int ShS[3][21]; //双顺 0为多少对 8
	int SaS[2][19]; //三顺 从三张里面查询连续的三张 9   
					// 三顺带牌 0为张数 10
					// 四带2 四带1 判定 11  
	int Time;    //剩余手数
};

void log(char* s)
{
	ofstream fout;
	fout.open("log.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	fout << s << endl;
	fout.close();
}
void log(char* title, int s)
{
	ofstream fout;
	fout.open("log.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	fout << title;
	fout << s << endl;
	fout.close();
}

/*任意手出牌时 三顺>双顺>单顺>三张>对子>单张 */
void FreeToTable(struct PaiXing *px, int ToTable[], int &Num, int Line, int Hands[])
{
	int i, sign = 0, cmp;
	if (px->SaS[0][0]>0)
	{
		cmp = px->SaS[0][1] / 4;     //从小牌出手
		for (i = 0; px->SaS[i][0]>0; i++)
		{
			if (cmp > px->SaS[i][1] / 4) {
				cmp = px->SaS[i][1] / 4;
				sign = i;
			}
		}
		if (px->SaS[sign][px->SaS[sign][0] * 3]<40 || px->SaS[sign][0] >= 4)
		{                              //最后一张牌需要小于K 或者形如 AAABBBCCCDDD
			int count = 0;//计数，判断带牌
			for (i = 0; px->DanPai[i] >= 0; i++) count++;
			if (count >= px->SaS[sign][0])
			{                               //先处理带牌,有多少带多少，不能带的拆开留下来
				for (i = 0; i<count; i++)
					ToTable[Num++] = px->DanPai[i];
				for (i = 1; px->SaS[sign][i] >= 0; i++)
					ToTable[Num++] = px->SaS[sign][i];
			}
			else if (count < px->SaS[sign][0] && count >= 2)
			{
				for (i = 0; i<count; i++)
					ToTable[Num++] = px->DanPai[i];
				for (i = 1; i<count * 3; i++)
					ToTable[Num++] = px->SaS[sign][i];
			}
			else
				for (i = 1; px->SaS[sign][i] >= 0; i++)
					ToTable[Num++] = px->SaS[sign][i];
			ToTable[Num++] = -1;
		}
	}

	if (px->ShS[0][0]>0)
	{
		sign = 0;
		cmp = px->ShS[0][1] / 4;
		for (i = 0; px->ShS[i][0]>0; i++)
		{
			if (cmp > px->ShS[i][1] / 4) {
				cmp = px->ShS[i][1] / 4;
				sign = i;
			}
		}
		if (px->ShS[sign][px->ShS[sign][0] * 2]<44 || px->ShS[sign][0] >= 5)
		{                            //最后一张牌小于A 或者形如 AABBCCDDEE
			for (i = 1; px->ShS[sign][i] >= 0; i++)
				ToTable[Num++] = px->ShS[sign][i];
			ToTable[Num++] = -1;
		}
	}

	if (px->DanShun[0][0]>0)
	{
		sign = 0;
		cmp = px->DanShun[0][1] / 4;   //从小牌出手
		for (i = 0; px->DanShun[i][0]>0; i++)
		{
			if ((cmp > px->DanShun[i][1] / 4) ||
				(cmp == px->DanShun[i][1] / 4 && px->DanShun[sign][0] / 4 < px->DanShun[i][0] / 4))
			{//最小牌更小 或 最小牌相同&&张数多 的优先                                  
				cmp = px->DanShun[i][1] / 4;
				sign = i;
			}
		}
		if (px->DanShun[sign][px->DanShun[sign][0]]<44 || px->DanShun[sign][0] >= 7 || px->Time <= 3)
		{                             //最后一张牌小于A 或者形如 3456789 或手数小于3
			for (i = 1; px->DanShun[sign][i] >= 0; i++)
				ToTable[Num++] = px->DanShun[sign][i];
			ToTable[Num++] = -1;
		}
	}

	if (px->SanZhang[0][0] >= 0)
	{
		sign = 0;
		cmp = px->SanZhang[0][0];
		for (i = 0; px->SanZhang[i][0] >= 0; i++) if (cmp > px->SanZhang[i][0])
		{
			cmp = px->SanZhang[i][0];
			sign = i;
		}
		if (px->SanZhang[sign][0] <40 ||
			(px->Time <= 3 && (px->DanPai[0] >= 0 || px->DuiPai[0][0] >= 0)))
		{                                       //KKK AAA 222 不出 
			for (i = 0; px->SanZhang[sign][i] >= 0; i++)
				ToTable[Num++] = px->SanZhang[sign][i];
			//如果有单牌和对牌 哪样多出哪样 AA22  A 2不带
			int count1 = 0, count2 = 0;
			for (i = 0; px->DanPai[i] >= 0 && (px->DanPai[i]<44 || px->Time<4); i++) count1++;
			for (i = 0; px->DuiPai[i][0] >= 0 && (px->DuiPai[i][0]<44 || px->Time<4); i++) count2++;
			if (count2 >= count1 && count2 != 0)
			{
				ToTable[Num++] = px->DuiPai[0][0];
				ToTable[Num++] = px->DuiPai[0][1];
			}
			else if (count1>count2) ToTable[Num++] = px->DanPai[0];
			ToTable[Num++] = -1;
		}
	}
	if (px->DuiPai[0][0] >= 0 && (px->DuiPai[0][0]< 44 || px->Time <= 2))
	{  //大过K的对牌暂不考虑 （AA 22）
		int count1 = 0, count2 = 0;
		for (i = 0; px->DanPai[i] >= 0; i++) count1++;
		for (i = 0; px->DuiPai[i][0] >= 0; i++) count2++;
		if (count2<count1&&px->DuiPai[0][0] / 4 - px->DanPai[0] / 4 <= 4)  // 对牌比较少的时候先出
		{
			if (px->SanZhang[0][0] >= 0 && px->SanZhang[0][0]<48)
				sign = 1;
			else sign = 0;

			ToTable[Num++] = px->DuiPai[sign][0];
			ToTable[Num++] = px->DuiPai[sign][1];
			ToTable[Num++] = -1;
		}
		else if (count2 == count1 == 1)
		{
			if (px->DuiPai[0][0]<px->DanPai[0])
			{
				ToTable[Num++] = px->DuiPai[sign][0];
				ToTable[Num++] = px->DuiPai[sign][1];
				ToTable[Num++] = -1;
			}
		}
	}
	if (px->DanPai[0] >= 0 && (px->Time>3 || px->SanZhang[0][0]<0)) {
		ToTable[Num++] = px->DanPai[0];
		ToTable[Num++] = -1;
	}
	///////////////////////////以下为上面的牌型皆不满足条件时时考虑策略/////////////////////////////////////////

	if (px->SaS[0][0]>0)
	{                                          //三顺
		cmp = px->SaS[0][1] / 4;     //从小牌出手
		for (i = 0; px->SaS[i][0]>0; i++)
		{
			if (cmp > px->SaS[i][1] / 4) {
				cmp = px->SaS[i][1] / 4;
				sign = i;
			}
		}
		int count = 0;//计数，判断带牌
		for (i = 0; px->DanPai[i] >= 0; i++) count++;
		if (count >= px->SaS[sign][0])
		{                               //先处理带牌,有多少带多少
			for (i = 0; i<count; i++)
				ToTable[Num++] = px->DanPai[i];
			for (i = 1; px->SaS[sign][i] >= 0; i++)
				ToTable[Num++] = px->SaS[sign][i];
		}
		else if (count < px->SaS[sign][0] && count >= 2)
		{
			for (i = 0; i<count; i++)
				ToTable[Num++] = px->DanPai[i];
			for (i = 1; i<count * 3; i++)
				ToTable[Num++] = px->SaS[sign][i];
		}
		else
			for (i = 1; px->SaS[sign][i] >= 0; i++)
				ToTable[Num++] = px->SaS[sign][i];
		ToTable[Num++] = -1;
	}

	if (px->ShS[0][0]>0)
	{                                 //双顺
		sign = 0;
		cmp = px->ShS[0][1] / 4;        //从小牌出手
		for (i = 0; px->ShS[i][0]>0; i++)
		{
			if (cmp > px->ShS[i][1] / 4) {
				cmp = px->ShS[i][1] / 4;
				sign = i;
			}
		}
		for (i = 1; px->ShS[sign][i] >= 0; i++)
			ToTable[Num++] = px->ShS[sign][i];
		ToTable[Num++] = -1;
	}

	if (px->DanShun[0][0]>0)
	{                                 //单顺
		sign = 0;
		cmp = px->DanShun[0][1] / 4;   //从小牌出手
		for (i = 0; px->DanShun[i][0]>0; i++)
		{
			if ((cmp > px->DanShun[i][1] / 4) ||
				(cmp == px->DanShun[i][1] / 4 && px->DanShun[sign][0] / 4 < px->DanShun[i][0] / 4))
			{//最小牌更小 或 最小牌相同&&张数多 的优先                                  
				cmp = px->DanShun[i][1] / 4;
				sign = i;
			}
		}
		for (i = 1; px->DanShun[sign][i] >= 0; i++)
			ToTable[Num++] = px->DanShun[sign][i];
		ToTable[Num++] = -1;
	}

	if (px->SanZhang[0][0] >= 0) {                     //三张
		sign = 0;
		cmp = px->SanZhang[0][0];
		for (i = 0; px->SanZhang[i][0] >= 0; i++) if (cmp > px->SanZhang[i][0])
		{
			cmp = px->SanZhang[i][0];
			sign = i;
		}
		for (i = 0; px->SanZhang[sign][i] >= 0; i++)
			ToTable[Num++] = px->SanZhang[sign][i];

		int count1 = 0, count2 = 0;
		for (i = 0; px->DanPai[i] >= 0 && (px->DanPai[i]<44 || px->Time <= 3); i++) count1++;
		for (i = 0; px->DuiPai[i][0] >= 0 && (px->DuiPai[i][0]<44 || px->Time <= 3); i++) count2++;

		if (count2 >= count1 && count2 != 0)
		{
			ToTable[Num++] = px->DuiPai[0][0];
			ToTable[Num++] = px->DuiPai[0][1];
		}
		else if (count1>count2) ToTable[Num++] = px->DanPai[0];
		ToTable[Num++] = -1;
	}
	if (px->Rocket&&px->Time <= 2)
	{
		ToTable[Num++] = 52;
		ToTable[Num++] = 53;
		ToTable[Num++] = -1;
	}
	if (px->ZhaDan[0][0] >= 0 && px->Time <= 3) {
		for (i = 0; i<4; i++)
			ToTable[Num++] = px->ZhaDan[0][i];
		ToTable[Num++] = -1;
	}

	if (px->DuiPai[0][0] >= 0) {  //对牌
		ToTable[Num++] = px->DuiPai[0][0];
		ToTable[Num++] = px->DuiPai[0][1];
		ToTable[Num++] = -1;
	}
	if (px->DanPai[0] >= 0) {  //单牌
		ToTable[Num++] = px->DanPai[0];
		ToTable[Num++] = -1;
	}


}


/*计算有效牌数*/
int  count(int cards[])
{
	int iCount = 0;
	while (cards[iCount] >= 0)
		iCount++;
	return iCount;
}


/*复制牌*/
void copycards(int arrOnHandCopy[], int arrOnHand[])
{
	int i = 0;
	for (i = 0; arrOnHand[i] >= 0; i++)
		arrOnHandCopy[i] = arrOnHand[i];
	arrOnHandCopy[i] = -1;
}


/*将totable一四张，三张，两张，一张排序，方便确定出牌类型，并比较大小*/
void mainsort(int cards[])
{
	int card_num = 0;/*牌的数量*/
	int i, j, k = 0, l;
	int sorted = 0;
	int fitcount = 0;//用来记录程序运行的次数  
	int sortcards[100];
	int jilu[20] = { -4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4 };
	int JL = 0;
	int JLX = 0;

	for (i = 0; cards[i]>-1; i++)//查询所出牌的张数
		card_num++;

	for (i = 4; i>0; i--)//先找出一样的四张牌，若有则写入sortcards【】中，然后再寻找三张的，一次类推	  
	{
		fitcount = 1;
		for (j = 0; j<card_num; j++)
		{
			if (j == card_num - 1)
			{
				if (cards[j] / 4 != cards[j - 1] / 4)
					if (fitcount == i)
					{
						sortcards[card_num - 1] = cards[j];
					}
			}
			else
			{
				k = 0;
				for (JLX = 0; jilu[JLX]>0; JLX++)
				{
					if (jilu[JLX] / 4 == cards[j] / 4)
						k = -1;
				}
				if (cards[j] / 4 == cards[j + 1] / 4 && k == 0)
					fitcount++;
				else
					fitcount = 1;

				if (fitcount == i && k == 0)
				{
					if (fitcount>1)
					{
						j++;
					}
					for (k = 0; k<i; k++)
					{
						sortcards[sorted++] = cards[j - i + 1 + k];
					}
					jilu[JL++] = sortcards[sorted - 1];//防止已经排列过三张的牌，再参加二张的排列
					fitcount = 1;
				}
			}
		}
	}
	sortcards[card_num] = -1;
	for (l = 0; l <= card_num; l++)//将排列好的牌，覆盖在原来的牌上面	
		cards[l] = sortcards[l];
}


/*将传入的数组从小到大排序
@param：int cards[] 传入的数组
*/
void pokersort(int cards[])
{
	int i, j;
	int temp;//交换值的中间变量

	for (i = 0; cards[i] >= 0; i++)
	{
		for (j = i + 1; cards[j] >= 0; j++)
			if (cards[i]>cards[j])
			{
				temp = cards[i];
				cards[i] = cards[j];
				cards[j] = temp;
			}
	}
}


/*
确定地主所出牌型
@para：int nCardsArr[] 已出牌数组
@return：出牌类型
*/
int  cardtype(int nCardsArr[])
{
	int i = 0;
	int nFlag = 0;
	int nLength = 0;

	pokersort(nCardsArr);
	mainsort(nCardsArr);


	for (i = 0; nCardsArr[i]>-1; i++)//查询所出牌的张数
		nLength++;
	//cout<<"cards"<<nCardsArr[0]<<" "<<nCardsArr[1]<<" "<<nCardsArr[2];
	//出的牌张数为零
	if (nLength == 0)
		return 0;

	//火箭
	if (nLength == 2 && nCardsArr[0] == 52 && nCardsArr[1] == 53)//牌的张数为二且牌为大王和小王
		return 1;

	//炸弹
	if (nLength == 4 	//牌的张数为四切四张牌的点数相等
		&& nCardsArr[0] / 4 == nCardsArr[1] / 4 && nCardsArr[1] / 4 == nCardsArr[2] / 4 && nCardsArr[2] / 4 == nCardsArr[3] / 4)
		return 2;
	/************************************************************************/
	/* 如果需要允许如33334444牌型可以将下列注释去掉                                                                     */
	/************************************************************************/
	// 	if (nLength == 8
	// 		&& nCardsArr[0]/4 == nCardsArr[1]/4 && nCardsArr[1]/4 == nCardsArr[2]/4 && nCardsArr[2]/4 == nCardsArr[3]/4
	// 		&& nCardsArr[3]/4+1 == nCardsArr[4]/4	&& nCardsArr[7]/4 != 12//第四张牌与第五张牌点数相差一且最大的牌不为二
	// 		&& nCardsArr[4]/4 == nCardsArr[5]/4 && nCardsArr[5]/4 == nCardsArr[6]/4 && nCardsArr[6]/4 == nCardsArr[7]/4)
	// 		return 2;
	// 	if (nLength == 12
	// 		&& nCardsArr[0]/4 == nCardsArr[1]/4 && nCardsArr[1]/4 == nCardsArr[2]/4 && nCardsArr[2]/4 == nCardsArr[3]/4
	// 		&& nCardsArr[3]/4+1 == nCardsArr[4]/4 && nCardsArr[11]/4 != 12	//第四张牌与第五张牌点数相差一且最大的牌不为二
	// 		&& nCardsArr[4]/4 == nCardsArr[5]/4 && nCardsArr[5]/4 == nCardsArr[6]/4 && nCardsArr[6]/4 == nCardsArr[7]/4
	// 		&& nCardsArr[7]/4+1 == nCardsArr[8]/4
	// 		&& nCardsArr[8]/4 == nCardsArr[9]/4 && nCardsArr[9]/4 == nCardsArr[10]/4 && nCardsArr[10]/4 == nCardsArr[11]/4)
	// 		return 2;

	//单牌
	if (nLength == 1)	//牌的张数为一
		return 3;

	//对牌
	if (nLength == 2										//牌的张数为二 
		&& nCardsArr[0] / 4 == nCardsArr[1] / 4)	//两张牌点数相同 
		return 4;

	//三连
	if (nLength == 3																							    //牌的张数为三
		&& nCardsArr[0] / 4 == nCardsArr[1] / 4 && nCardsArr[1] / 4 == nCardsArr[2] / 4)	//三张牌点数相同 
		return 5;

	//三带一
	if (nLength == 4																								//牌的张数为四
		&& nCardsArr[0] / 4 == nCardsArr[1] / 4 && nCardsArr[1] / 4 == nCardsArr[2] / 4	//前三张牌点数相同 
		&& nCardsArr[3] / 4 != nCardsArr[1] / 4)															    //第四张牌与前三张牌不同点	
		return 6;

	//三带二
	if (nLength == 5
		&& nCardsArr[0] / 4 == nCardsArr[1] / 4 && nCardsArr[1] / 4 == nCardsArr[2] / 4	//前三张牌点数相同 
		&& nCardsArr[3] / 4 == nCardsArr[4] / 4)															//后两张牌点数相同
		return 6;

	//单顺
	int nFlagOfSingleShun = 1;
	for (i = 0; i<nLength - 1; i++)
	{
		if (nCardsArr[i] / 4 == nCardsArr[i + 1] / 4 - 1	//前一张牌与后一张牌点数相差一
			&& nLength >= 5								//牌的长度需大于等于五
			&& nCardsArr[nLength - 1]<48)			//牌的最大点数不能超过A（即不能为二，小王，大王）														
			continue;
		else
		{
			nFlagOfSingleShun = 0;
			break;
		}
	}
	if (nFlagOfSingleShun)
		return 7;

	//双顺
	nFlag = 0;
	//牌的张数需大于等于六且为偶数，最后一张牌的大小需小于二
	if (nLength >= 6 && nLength % 2 == 0 && nCardsArr[nLength - 1]<48)
		for (i = 0; i<nLength / 2; i++)
		{
			if (nCardsArr[2 * i] / 4 == nCardsArr[2 * i + 1] / 4)//连续两张点数相同
				nFlag++;
			else
				break;
			if (i != nLength / 2 - 1 && nCardsArr[2 * i] / 4 == nCardsArr[2 * (i + 1)] / 4 - 1)//相隔一张牌的两牌点数相差一
				nFlag++;
			else//如果是最后两张牌，则无需再使其与下两张牌相差一，自身为对牌即可
				if (i == nLength / 2 - 1)//【************这个条件太简单，万一对方出错出成3344JJ，我方也出错**************】
					nFlag++;
				else
					break;
		}
	if (nFlag == nLength)
		return 8;

	//三顺
	nFlag = 0;
	if (nLength >= 6 && nLength % 3 == 0)							//牌的张数需大于六,且为三的倍数
		for (i = 0; i<nLength / 3; i++)
		{																				//连续的三张牌相等
			if (nCardsArr[i * 3] / 4 == nCardsArr[i * 3 + 1] / 4 && nCardsArr[i * 3 + 1] / 4 == nCardsArr[i * 3 + 2] / 4)
				nFlag++;
			else
				break;
			if (nCardsArr[i * 3] / 4 == nCardsArr[3 * (i + 1)] / 4 - 1)	//相隔两张牌的两牌点数相差一,且不为最后两张牌
				nFlag += 2;
			else
				if (i == nLength / 3 - 1)
					nFlag += 2;
				else
					break;
		}
	if (nFlag == nLength)
		return 9;

	//飞机带单
	nFlag = 0;
	if (nLength >= 8 && nLength % 4 == 0)								//牌的张数需大于八，且为四的倍数
		for (i = 0; i<nLength / 4; i++)											//长度除以四即为构成三顺的牌的张数
		{																				//连续的三张牌相等
			if (nCardsArr[i * 3] / 4 == nCardsArr[i * 3 + 1] / 4 && nCardsArr[i * 3 + 1] / 4 == nCardsArr[i * 3 + 2] / 4)
				nFlag++;
			else
				break;
			if (nCardsArr[i * 3] / 4 == nCardsArr[3 * (i + 1)] / 4 - 1)	//相隔两张牌的两牌点数相差一,且不为最后两张牌
				nFlag += 2;
			else
				if (i == nLength / 4 - 1)
					nFlag += 2;
				else
					break;
		}
	if ((nLength - nFlag) == nFlag / 3)//三顺牌的不同点数个数与单牌数目相同
		return 10;

	//飞机带双	
	nFlag = 0;
	int temp = 0;

	if (nLength >= 10 && nLength % 5 == 0)							//牌的张数需大于十,且为五的倍数
		for (i = 0; i<nLength / 5; i++)
		{																				//连续的三张牌相等
			if (nCardsArr[i * 3] / 4 == nCardsArr[i * 3 + 1] / 4 && nCardsArr[i * 3 + 1] / 4 == nCardsArr[i * 3 + 2] / 4)
				nFlag++;
			else
				break;
			if (nCardsArr[i * 3] / 4 == nCardsArr[3 * (i + 1)] / 4 - 1)	//相隔两张牌的两牌点数相差一,且不为最后两张牌
				nFlag += 2;
			else
				if (i == nLength / 5 - 1)
					nFlag += 2;
				else
					break;
		}
	if ((nLength - nFlag) % 2 == 0 && (nLength - nFlag) / 2 == nFlag / 3)//对数的个数与三张牌相同的点数的个数相等
	{
		temp = nFlag;//记录下三张牌相同的总牌数
		for (i = 0; i<temp / 3; i++)//三张牌相同的点数的个数即为所需对数的个数
		{
			if (nCardsArr[2 * i + temp] / 4 == nCardsArr[2 * i + 1 + temp] / 4)
				nFlag += 2;
		}
	}
	if (nFlag == nLength)
		return 10;

	//四带两单
	if (nLength == 6					//牌的张数为六
		&& nCardsArr[0] / 4 == nCardsArr[1] / 4 && nCardsArr[1] / 4 == nCardsArr[2] / 4 && nCardsArr[2] / 4 == nCardsArr[3] / 4  //前四张牌相等
		&& nCardsArr[4] / 4 != nCardsArr[5] / 4)//后两张不能为对牌
		return 11;

	//四带两双
	if (nLength == 8					//牌的张数为八
		&& nCardsArr[0] / 4 == nCardsArr[1] / 4 && nCardsArr[1] / 4 == nCardsArr[2] / 4 && nCardsArr[2] / 4 == nCardsArr[3] / 4 //前四张牌相等
		&& nCardsArr[4] / 4 == nCardsArr[5] / 4 && nCardsArr[6] / 4 == nCardsArr[7] / 4
		&& nCardsArr[7] != 53		 //最后两张不能为王炸
		)
		return 11;
	return -1;
}

/*数据初始化*/
void init(struct ddz * dp)
{
	int i, j;
	dp->iStatus = 1;							//初始
	dp->iBmax = 0;							//叫牌分数
	dp->iOTmax = 0;							//桌面牌手数

	dp->iBid[0] = 0;
	dp->iBid[1] = 0;
	dp->iBid[2] = 0;
	for (i = 0; i<162; i++)						//初始化桌面牌
		for (j = 0; j<21; j++)
			dp->iOnTable[i][j] = -2;
	for (i = 0; i<21; i++)						//初始化手牌
		dp->iOnHand[i] = -2;

	//初始化手牌数
	dp->iRestHandCard[0] = 17;
	dp->iRestHandCard[1] = 17;
	dp->iRestHandCard[2] = 17;
}

/*
将iToTable[]中的数字转化为sCommandOut[]字符串
@param：iToTable
@param：sCommandOut
*/
void iTos(int iToTable[], char sCommandOut[], struct ddz *dp)
{
	int i;
	char t[4];
	if (iToTable[0] == -1)
	{
		t[0] = '-';
		t[1] = '1';
		t[3] = '\0';
		strcat(sCommandOut, t);
	}
	else
	{
		for (i = 0; iToTable[i] >= 0; i++)
		{
			if (iToTable[i] >= 10)
			{
				t[0] = iToTable[i] / 10 + '0';
				t[1] = iToTable[i] % 10 + '0';
				t[2] = ',';
				t[3] = '\0';
			}
			else
			{
				t[0] = iToTable[i] % 10 + '0';
				t[1] = ',';
				t[2] = '\0';
			}
			dp->iRestHandCard[dp->cDir - 'A']--;//玩家剩余手牌--
			strcat(sCommandOut, t);
		}
	}
	sCommandOut[strlen(sCommandOut) - 1] = '\0';
}


/*将iToTable[]中的牌从iOnHand[]中剔除
@param：int iOnHand[]
@param：int iToTable[]
*/
void TakeOff(int iOnHand[], int iToTable[])
{
	int i, j, k;
	for (i = 0; iToTable[i] >= 0; i++)
		for (j = 0; iOnHand[j] >= 0; j++)
			if (iToTable[i] == iOnHand[j])
			{
				for (k = j + 1; iOnHand[k] >= -1 && k<21; k++)
					iOnHand[k - 1] = iOnHand[k];
				break;
			}
}


/*版本信息*/
void GetDou(struct ddz * dp)
{
	int i;
	for (i = 0; dp->sCommandIn[i] != '\0'; i++)
		dp->sVer[i] = dp->sCommandIn[i];
	dp->sVer[i] = '\0';
	strcpy(dp->sCommandOut, "AuroraStudio_DDZ");
}


/*轮次信息处理函数:
输入INFO 1/4 1/9 9 2450
输出OK INFO
*/
void GetInf(struct ddz * dp)
{
	int a, b, c, d;
	dp->iTurnNow = dp->sCommandIn[5] - '0';	//当前轮次
	dp->iTurnTotal = dp->sCommandIn[7] - '0';	//总轮数
	dp->iRoundNow = dp->sCommandIn[9] - '0';	//当前局次
	dp->iRoundTotal = dp->sCommandIn[11] - '0';	//总局数
	dp->iLevelUp = dp->sCommandIn[13] - '0';	//晋级选手数
	a = dp->sCommandIn[15] - '0';
	b = dp->sCommandIn[16] - '0';
	c = dp->sCommandIn[17] - '0';
	d = dp->sCommandIn[18] - '0';
	dp->iScoreMax = a * 1000 + b * 100 + c * 10 + d;	//得分界限
	strcpy(dp->sCommandOut, "OK INFO");
}


/*发牌信息*/
void GetDea(struct ddz * dp)
{
	int i, j = 0, t;
	dp->cDir = dp->sCommandIn[5];     //获取本家AI方位编号

									  //将DEAL A0,4,5,…51,53中的牌整理到 手中牌iOnHand[]中
	dp->iOnHand[0] = 0;
	dp->iOnHand[1] = -1;
	for (i = 6; dp->sCommandIn[i] != '\0'; i++)
	{
		if (dp->sCommandIn[i] != ',')
			dp->iOnHand[0] = dp->iOnHand[0] * 10 + dp->sCommandIn[i] - '0';
		else
		{
			for (j = 21; j >= 1; j--)
				dp->iOnHand[j] = dp->iOnHand[j - 1];
			dp->iOnHand[0] = 0;
		}
	}
	for (i = 0; i<9; i++)
	{
		t = dp->iOnHand[i];
		dp->iOnHand[i] = dp->iOnHand[16 - i];
		dp->iOnHand[16 - i] = t;
	}
	strcpy(dp->sCommandOut, "OK DEAL");  //回复信息
	pokersort(dp->iOnHand);  //iOnHand[]从小到大排序
							 /*for (int i = 0; i < 21; i++)
							 log("\n牌：", dp->iOnHand[i]);*/
}


/*处理底牌信息*/
void GetLef(struct ddz * dp)
{
	int test = 0;//test
	int i, j = 17;
	dp->cLandlord = dp->sCommandIn[9];     //确定地主方
	dp->iRestHandCard[dp->cLandlord - 'A'] += 3;//地主+3牌
	if (dp->sCommandIn[9] == dp->cDir)
	{
		for (j = 21; j >= 1; j--)
			dp->iOnHand[j] = dp->iOnHand[j - 1];
		dp->iOnHand[0] = 0;
		for (i = 10; dp->sCommandIn[i] != '\0'; i++)
		{
			if (dp->sCommandIn[i] != ',')
				dp->iOnHand[0] = dp->iOnHand[0] * 10 + dp->sCommandIn[i] - '0';
			else
			{
				for (j = 21; j >= 1; j--)
					dp->iOnHand[j] = dp->iOnHand[j - 1];
				dp->iOnHand[0] = 0;
			}
		}
	}
	strcpy(dp->sCommandOut, "OK LEFTOVER");
	pokersort(dp->iOnHand);					//iOnHand[]从小到大排序
}


/*GAMEOVER 指令处理*/
void GetGam(struct ddz * dp)
{
	dp->cWinner = dp->sCommandIn[9];			//胜利者方位编号
	strcpy(dp->sCommandOut, "OK GAMEOVER");
	//cout<<dp->sCommandOut<<endl;
	if (dp->iRoundNow == dp->iRoundTotal)		//当前局数与每轮局相等时
		dp->iStatus = 0;						//引擎状态变量设为结束
	else									//否则
		init(dp);							//引擎状态变量设为重新开始
}


/*在arrToTable中返回所有可行序列 -1间隔,-2结束
@param：int arrOnHand[]		手中牌
@param：int arrOnTable[]	已出牌数组
@param：int arrToTable[]	保存要出的牌
@param：int Hands[]			三家剩余的剩余牌
@param：struct PaiXing *px
@param：int Line			阵容1.【农 AI 农】 2.【友 AI 地】 3.【地 AI 友】
*/
void helpdz(int arrOnHand[], int arrOnTable[], int arrToTable[], int Hands[], int Line, struct PaiXing *px, struct ddz * dp)
{
	int nCardToPlay;//确定地主所出的牌型 1.火箭2.炸弹3.单牌4.对牌5.三张策略6.三带X 7.连续单顺策略8.连续双顺策略9.连续三顺策略10三带一单策略11.四带二12.放弃
	int nInsertCards = 0;//在arrToTable中插入可以出的牌
	int nNumToCompare;//对方出的牌的第一张，用以比较大小
	int nToTableCardsNum = 0;//打出去的长度	
	int l = (dp->cDir - 'A' + 2) % 3;
	int r = (dp->cDir - 'A' + 1) % 3;
	int anotherHands[2] = { Hands[l], Hands[r] };//左边玩家剩余手牌，右边玩家剩余手牌
	int i = 0;
	int j = 0;
	if (arrOnTable[0] == -1 && arrOnTable[1] == -1 || arrOnTable[0] == -2)//两家放弃，地主随意
	{
		nCardToPlay = 0;
	}
	else
	{//Line阵容确定
	 /*log("打印arrOnTable数组：");
	 for (int i = 0; i < 21; i++)
	 log("牌：", arrOnTable[i]);*/
		if (arrOnTable[0] == -1)//上家放弃，牌型根据下手家判定
		{
			//cout<<arrOnTable[0]<<" "<<arrOnTable[1]<<" "<<arrOnTable[2]<<endl;
			if (arrOnTable[1] >= 0 && arrOnTable[1]<4 && arrOnTable[2] < 0)//对方出单牌3
			{
				nCardToPlay = 3;  //mainsort堆栈出问题，空间不足被复用，暂时把此段特殊化
				nNumToCompare = 0;
			}
			else if (Line == 3 && (px->Time > 3 || (anotherHands[l] == 1 && px->DanPai[1] < 48 && px->DanPai[i] >= 0)))//手数>3或者(左边地主报单并且我最大单牌小于2	
			{
				nCardToPlay = 12;
			}
			else
			{
				//pokersort(arrOnTable+1);
				//mainsort(arrOnTable+1);
				nCardToPlay = cardtype(arrOnTable + 1);//地主牌类型
				nNumToCompare = arrOnTable[1];//右边玩家的第一张牌
				nToTableCardsNum = count(arrOnTable + 1);//计算打出多少牌
				if (nNumToCompare < 52)
					nNumToCompare = nNumToCompare / 4;
			}
		}//左手放弃End
		else//牌型根据左手家判定
		{
			//队友手牌小于等于6且地主牌书大于2 或者队友出牌大于K 或者队友出炸时 放弃出牌让队友
			if (Line == 2 && (anotherHands[0] <= 6 && anotherHands[1] > 2 || arrOnTable[0] >= 40 || cardtype(arrOnTable) == 2)) nCardToPlay = 12;//让友军，放弃出牌
			else
			{
				nCardToPlay = cardtype(arrOnTable);//地主牌类型
				nNumToCompare = arrOnTable[0];//左边友军玩家的第一张牌
				nToTableCardsNum = count(arrOnTable);//计算打出多少牌
				if (nNumToCompare < 52)
					nNumToCompare = nNumToCompare / 4;
			}
		}
	}
	log("出牌类型为：", nCardToPlay);
	bool over = 0;
	//[未用]int arrOnHandCopy[22];
	int n = 0;
	int nDelCards = 0;//删除手牌中重复的牌，只留下其中的一个
	int nCardsLength = 0;//看已知连续的牌的张数是否与希望的单顺长度相同
	int nCanInsert = 0;


	switch (nCardToPlay)
	{
	case 0:
		FreeToTable(px, arrToTable, nInsertCards, Line, anotherHands);
		break;

	case 1://火箭			
		break;

	case 2://炸弹【只有在对方出炸弹才出？】
		if (px->ZhaDan[0][0] >= 0)//自己有炸
		{
			int sign = 0, cmp = 55;
			for (i = 0; px->ZhaDan[i][0] >= 0; i++)
				if (px->ZhaDan[i][0]>nNumToCompare &&px->ZhaDan[i][0]<cmp)//选最小的炸
				{
					sign = i;//index
					cmp = px->ZhaDan[i][0];
				}
			for (i = 0; i<4; i++)
				arrToTable[nInsertCards++] = px->ZhaDan[sign][i]; //出牌 
			arrToTable[nInsertCards++] = -1;
		}
		break;

	case 3://单牌       （暂存在强制拆牌问题  考虑一下套路   待改ing************************************************************
		   //【对方报警的时候牌应该从大到小打吧。。。】
		if (px->DanPai[0] >= 0 || px->DuiPai[0][0] >= 0 || px->SanZhang[0][0] >= 0)//若有有单，对或三张
		{                  //考虑拆牌
			bool key = 0;//0拆牌 1可不用拆
			for (i = 0; px->DanPai[i] >= 0; i++) if (px->DanPai[i] / 4 > nNumToCompare /*&& px->DanPai[i] / 4 - nNumToCompare <= 4*/)//有单牌比对手牌<=4的出。死命不出牌的原因<=4
			{
				arrToTable[nInsertCards++] = px->DanPai[i];
				arrToTable[nInsertCards++] = -1;
				key = 1;
			}
			//A或以上的三张可以拆******************************
			//key=0 且 无单张 且 (三张大过A 且 自身手数3以下 或 ( 敌人报警) )
			if (!key&&px->DanPai[0]<0 && (px->SanZhang[0][0] >= 44 && px->Time <= 3 ||
				(
				((Line == 1 && (anotherHands[0] <= 3 || anotherHands[1] <= 3)) ||//自己是地主，对方有人报警
					(Line == 2 && anotherHands[1] <= 3) ||//右边的地主报警
					(Line == 3 && anotherHands[0] <= 3)//左边地主报警
					))))
			{
				key = 0;  //重置确认获取可出牌
				for (i = 0; px->SanZhang[i][0] >= 0; i++) if (px->SanZhang[i][0] / 4 > nNumToCompare)
				{
					arrToTable[nInsertCards++] = px->SanZhang[i][0];
					arrToTable[nInsertCards++] = -1;
					key = 1;
					break;
				}
			}
			//2或以上的对牌可以拆
			if (!key &&px->DanPai[0]<0 && (px->DuiPai[0][0] >= 48 && px->Time <= 3 ||
				((Line == 1 && (anotherHands[0] <= 3 || anotherHands[1] <= 3)) ||
				(Line == 2 && anotherHands[1] <= 3) ||
					(Line == 3 && anotherHands[0] <= 3))))
			{
				for (i = 0; px->DuiPai[i][0] >= 0; i++) if (px->DuiPai[i][0] / 4 > nNumToCompare && px->DuiPai[i][0]>47)//48 2
				{
					arrToTable[nInsertCards++] = px->DuiPai[i][0];
					arrToTable[nInsertCards++] = -1;
					break;
				}
			}
		}
		break;

	case 4://对牌   （暂存在强制拆牌问题  考虑一下套路   待改ing************************************************************
		if (nNumToCompare<12) // 桌上的牌小于对2
		{
			bool key = 0;
			for (i = 0; px->DuiPai[i][0] >= 0; i++) if (px->DuiPai[i][0] / 4 > nNumToCompare /*&&
																							 px->DuiPai[i][0] / 4 - nNumToCompare <= 4*/)//对牌相差在4以内可以出
			{
				arrToTable[nInsertCards++] = px->DuiPai[i][0];
				arrToTable[nInsertCards++] = px->DuiPai[i][1];
				arrToTable[nInsertCards++] = -1;
				key = 1;
				break;
			}//end for
			 //剩余三手以下可拆三张A或以上
			if (!key && px->DuiPai[0][0]<0 && (px->SanZhang[0][0] >= 44 && px->Time <= 3 ||
				(
				((Line == 1 && (anotherHands[0] <= 3 || anotherHands[1] <= 3)) ||
					(Line == 2 && anotherHands[1] <= 3) ||
					(Line == 3 && anotherHands[0] <= 3)))))
			{
				for (i = 0; px->SanZhang[i][0] >= 0; i++) if (px->SanZhang[i][0] / 4 > nNumToCompare && px->SanZhang[i][0] >= 44)//44 A
				{
					arrToTable[nInsertCards++] = px->SanZhang[i][0];
					arrToTable[nInsertCards++] = px->SanZhang[i][1];
					arrToTable[nInsertCards++] = -1;
					key = 1;
					break;
				} //RECORD  (  待改****************************************************************************************************8
			}//end if
		}//end 
		break;

	case 5://三张策略
		if (px->SanZhang[0][0] >= 0)
		{
			//log("only three", dp->iOTmax);
			int Test[3] = { 55,-1,-1 }, key = 0;   //key查看是否能出牌
			for (i = 0; px->SanZhang[i][0] >= 0; i++)
			{                                      //策略 从搜索牌型中寻找最小可打牌
				if (px->SanZhang[i][0] / 4 > nNumToCompare && px->SanZhang[i][0]<Test[0])
				{
					Test[0] = px->SanZhang[i][0];
					Test[1] = px->SanZhang[i][1];
					Test[2] = px->SanZhang[i][2];
					px->SanZhang[i][0] = 55;
					key = 1;
				}

			}
			if (key)
			{
				arrToTable[nInsertCards++] = Test[0];
				arrToTable[nInsertCards++] = Test[1];
				arrToTable[nInsertCards++] = Test[2];
				arrToTable[nInsertCards++] = -1;
			}
		}
		break;

	case 6: //三带单 三带对			
		if (px->SanZhang[0][0] >= 0 && (px->DuiPai[0][0] >= 0 || px->DanPai[0] >= 0))
		{
			//log("case6牌数：", nToTableCardsNum);
			int Test[3] = { 55,-1,-1 }, key = 0;
			for (i = 0; px->SanZhang[i][0] >= 0; i++)
			{
				Test[0] = 55;
				if (px->SanZhang[i][0] / 4 > nNumToCompare && px->SanZhang[i][0]<Test[0])
				{
					Test[0] = px->SanZhang[i][0];
					Test[1] = px->SanZhang[i][1];
					Test[2] = px->SanZhang[i][2];
					px->SanZhang[i][0] = 55;
					key = 1;
					break;
				}


			}
			//log("case6 key值：", key);
			if (key)
			{



				//log("case6 DuiPai[0][0]值：", px->DuiPai[0][0]);
				//log("case6 px->DanPai[0]值：", px->DanPai[0]);
				if (px->DuiPai[0][0] >= 0 && nToTableCardsNum == 5)
				{
					arrToTable[nInsertCards++] = Test[0];
					arrToTable[nInsertCards++] = Test[1];
					arrToTable[nInsertCards++] = Test[2];
					arrToTable[nInsertCards++] = px->DuiPai[0][0];
					arrToTable[nInsertCards++] = px->DuiPai[0][1];
					//log("three and two", dp->iOTmax);
				}
				else if (px->DanPai[0] >= 0 && nToTableCardsNum == 4)
				{
					arrToTable[nInsertCards++] = Test[0];
					arrToTable[nInsertCards++] = Test[1];
					arrToTable[nInsertCards++] = Test[2];
					arrToTable[nInsertCards++] = px->DanPai[0];
					//log("three and one", dp->iOTmax);
				}
				arrToTable[nInsertCards++] = -1;
			}
		}

		break;

	case 7:
		if (px->DanShun[0][0]>0)//7.连续单顺策略
		{
			int key1 = 0, key2 = 0, sign = 0, cmp = 55, Num = 1;
			//nToTableCardsNum = count(arrOnTable);
			for (i = 0; px->DanShun[i][0]>0; i++)
			{                                 //长度相等 牌值小的优先
				if (px->DanShun[i][0] == nToTableCardsNum &&
					px->DanShun[i][1] / 4 > nNumToCompare && px->DanShun[i][1] / 4 < cmp / 4)//选最小的顺
				{
					sign = i;
					cmp = px->DanShun[i][1];
					key1 = 1;
				}
			}
			if (!key1)
			{                  //如果没找到长度相等的
				for (i = 0; px->DanShun[i][0]>0; i++)
				{                                 //长度相等 牌值小的优先
					if (px->DanShun[i][0] > nToTableCardsNum)   //考虑拆牌
						for (j = 1; j <= px->DanShun[i][0] - nToTableCardsNum + 1; j++)//每位检索直到找到大于目标
						{   //1~5 = 2~6 = 6-5+1
							if (px->DanShun[i][j] / 4 > nNumToCompare &&
								px->DanShun[i][j] / 4 < cmp / 4)
							{
								sign = i;
								Num = j;
								cmp = px->DanShun[i][j];
								key2 = 1;
								break;
							}
						}
				}
			}
			if (key1) {//直接找到
				for (i = 1; i <= nToTableCardsNum; i++)
					arrToTable[nInsertCards++] = px->DanShun[sign][i];
				arrToTable[nInsertCards++] = -1;
			}
			else if (key2) {          //拆牌找到， 5 = 2~6 < 2+5
				for (i = Num; i< nToTableCardsNum + Num; i++)
					arrToTable[nInsertCards++] = px->DanShun[sign][i];
				arrToTable[nInsertCards++] = -1;
			}
		}
		break;

	case 8:
		if (px->ShS[0][0]>0)//连续双顺策略
		{
			//log("双顺\n");
			int key1 = 0, key2 = 0, sign = 0, cmp = 55, Num = 1;
			nToTableCardsNum = count(arrOnTable);
			for (i = 0; px->ShS[i][0]>0; i++)
			{
				if (px->ShS[i][0] * 2 == nToTableCardsNum &&
					px->ShS[i][1] / 4 > nNumToCompare && px->ShS[i][1] / 4 < cmp / 4)//找最小的双顺
				{
					sign = i;
					cmp = px->ShS[i][1];
					key1 = 1;
				}
			}
			if (!key1)//没找到等长双顺，拆更长的双顺
			{
				for (i = 0; px->ShS[i][0]>0; i++)
				{  //1~6 = 3~8  = 8-6 + 1 = 5~10 = 10-6 +1
					if (px->ShS[i][0] * 2 > nToTableCardsNum)
						for (j = 1; j <= px->ShS[i][0] * 2 - nToTableCardsNum + 1; j += 2)
						{
							if (px->ShS[i][j] / 4 > nNumToCompare &&
								px->ShS[i][j] / 4 < cmp / 4)
							{
								sign = i;
								Num = j;
								cmp = px->ShS[i][j];
								key2 = 1;
								break;
							}
						}
				}
			}
			if (key1) {
				for (i = 1; i <= nToTableCardsNum; i++)
					arrToTable[nInsertCards++] = px->ShS[sign][i];
				arrToTable[nInsertCards++] = -1;
			}
			else if (key2) {
				for (i = Num; i<nToTableCardsNum + Num; i++)
					arrToTable[nInsertCards++] = px->ShS[sign][i];
				arrToTable[nInsertCards++] = -1;
			}
		}
		break;

	case 9:
		if (px->SaS[0][0]>0)//连续三顺策略
		{
			int key1 = 0, key2 = 0, sign = 0, cmp = 55, Num = 1;
			nToTableCardsNum = count(arrOnTable);
			for (i = 0; px->SaS[i][0]>0; i++)
			{
				if (px->SaS[i][0] * 3 == nToTableCardsNum &&
					px->SaS[i][1] / 4 > nNumToCompare && px->SaS[i][1] / 4 < cmp / 4)//最小三顺
				{
					sign = i;
					cmp = px->SaS[i][1];
					key1 = 1;
				}
			}
			if (!key1)//没找到等长三顺，拆更长的三顺
			{
				for (i = 0; px->SaS[i][0]>0; i++)
				{  // 1~6=4~9=9-6+1=7~12=12-6+1 
					if (px->SaS[i][0] * 3 > nToTableCardsNum)
						for (j = 1; j <= px->SaS[i][0] * 3 - nToTableCardsNum + 1; j += 3)
						{
							if (px->SaS[i][j] / 4 > nNumToCompare &&
								px->SaS[i][j] / 4 < cmp / 4)
							{
								sign = i;
								Num = j;
								key2 = 1;
								cmp = px->SaS[i][j];
								break;
							}
						}
				}
			}
			if (key1) {
				for (i = 1; i <= nToTableCardsNum; i++)
					arrToTable[nInsertCards++] = px->SaS[sign][i];
				arrToTable[nInsertCards++] = -1;
			}
			else if (key2) {
				for (i = Num; i<nToTableCardsNum + Num; i++)
					arrToTable[nInsertCards++] = px->SaS[sign][i];
				arrToTable[nInsertCards++] = -1;
			}

		}
		break;

	case 10:
		if (px->SaS[0][0]>0 && (px->DanPai[0] >= 0 || px->DuiPai[0][0] >= 0))//三带一单 策略，即飞机	
		{
			int key1 = 0, key2 = 0, sign = 0, cmp = 55, Num = 1;
			int daiNum = nToTableCardsNum - nToTableCardsNum / 4 * 3;
			nToTableCardsNum = nToTableCardsNum / 4 * 3;  //记录三张的数量count(arrOnTable)
			if (px->DanPai[nToTableCardsNum / 3 - 1] >= 0 ||
				px->DuiPai[nToTableCardsNum / 3 - 1][0] >= 0)  //确定能够打出符合条件的牌
			{
				for (i = 0; px->SaS[i][0]>0; i++)//最小三顺
				{
					if (px->SaS[i][0] * 3 == nToTableCardsNum &&
						px->SaS[i][1] / 4 > nNumToCompare && px->SaS[i][1] / 4 < cmp / 4)
					{
						sign = i;
						cmp = px->SaS[i][1];
						key1 = 1;
					}
				}
				if (!key1)//没找到，拆更长的三顺
				{
					for (i = 0; px->SaS[i][0]>0; i++)
					{  // 1~6=4~9=9-6+1=7~12=12-6+1 
						if (px->SaS[i][0] * 3 > nToTableCardsNum)
							for (j = 1; j <= px->SaS[i][0] * 3 - nToTableCardsNum + 1; j += 3)
							{
								if (px->SaS[i][j] / 4 > nNumToCompare &&
									px->SaS[i][j] / 4 < cmp / 4)
								{
									sign = i;
									Num = j;
									key2 = 1;
									cmp = px->SaS[i][j];
									break;
								}
							}
					}
				}
				if (key1) {
					for (i = 1; i <= nToTableCardsNum; i++)
						arrToTable[nInsertCards++] = px->SaS[sign][i];
				}
				else if (key2) {
					for (i = Num; i<nToTableCardsNum + Num; i++)
						arrToTable[nInsertCards++] = px->SaS[sign][i];
				}
				if (key1 || key2)
				{
					int count1 = 0, count2 = 0;
					for (i = 0; px->DanPai[i] >= 0; i++) count1++;
					for (i = 0; px->DuiPai[i][0] >= 0; i++) count2++;
					//判断对方带双还是带单
					if (nToTableCardsNum / 3 == daiNum / 2) {
						//如过手上对牌数量少于要求，复原数组并放弃
						if (count2 < daiNum / 2)
						{
							for (i = 1; i <= nToTableCardsNum; i++)
								arrToTable[nInsertCards--] = -2;

						}
						else
							for (i = 0; i<daiNum / 2; i++)
							{
								arrToTable[nInsertCards++] = px->DuiPai[i][0];
								arrToTable[nInsertCards++] = px->DuiPai[i][1];
							}
					}
					else {
						for (i = 0; i<daiNum; i++)
							arrToTable[nInsertCards++] = px->DanPai[i];
					}
					arrToTable[nInsertCards++] = -1;
				}
			}
		}

		break;

	case 11://四带二
		if (px->ZhaDan[0][0] >= 0 && (px->DanPai[1] >= 0 || px->DuiPai[1][0] >= 0))
		{
			int sign = 0, key = 0, cmp = 55;
			for (i = 0; px->ZhaDan[i][0] >= 0; i++)
			{
				if (px->ZhaDan[i][0] / 4 > nNumToCompare && px->ZhaDan[i][0]<cmp)//最小炸弹
				{
					key = 1;
					cmp = px->ZhaDan[i][0];
					sign = i;
				}
			}
			if (key)
			{
				for (i = 0; i<4; i++)
					arrToTable[nInsertCards++] = px->ZhaDan[sign][i];
				int count1 = 0, count2 = 0;
				for (i = 0; px->DanPai[i] >= 0; i++) count1++;
				for (i = 0; px->DuiPai[i][0] >= 0; i++) count2++;
				if (count2 >= count1) {
					for (i = 0; i<nToTableCardsNum / 3; i++)
					{
						arrToTable[nInsertCards++] = px->DuiPai[i][0];
						arrToTable[nInsertCards++] = px->DuiPai[i][1];
					}
				}
				else {
					for (i = 0; i<nToTableCardsNum / 3; i++)
						arrToTable[nInsertCards++] = px->DanPai[i];
				}
				arrToTable[nInsertCards++] = -1;
			}

		}
		break;
	case 12:  //其他应对策略

			  //手上有炸时
			  //log("炸弹数目：",px->ZhaDan[0][0]);
		if (px->ZhaDan[0][0] >= 0)
		{
			//log("有炸弹");
			//自己是地主时
			if (dp->WarLine == 1 && (anotherHands[0] <= 4 || anotherHands[1] <= 4))
			{
				int sign = 0, cmp = 55;
				for (i = 0; px->ZhaDan[i][0] >= 0; i++)
					if (px->ZhaDan[i][0]<cmp)//选最小的炸
					{
						sign = i;//index
						cmp = px->ZhaDan[i][0];
					}
				for (i = 0; i<4; i++)
					arrToTable[nInsertCards++] = px->ZhaDan[sign][i]; //出牌 
				arrToTable[nInsertCards++] = -1;
			}
			//上家队友放弃且下家地主手牌小于等于4时
			else if (dp->WarLine == 2 && arrOnTable[0] == -1 && anotherHands[1] <= 4)
			{
				int sign = 0, cmp = 55;
				for (i = 0; px->ZhaDan[i][0] >= 0; i++)
					if (px->ZhaDan[i][0]<cmp)//选最小的炸
					{
						sign = i;//index
						cmp = px->ZhaDan[i][0];
					}
				for (i = 0; i<4; i++)
					arrToTable[nInsertCards++] = px->ZhaDan[sign][i]; //出牌 
				arrToTable[nInsertCards++] = -1;
			}
			//地主为上家且地主牌少于等于2时
			else if (dp->WarLine == 3 && anotherHands[0] <= 2)
			{
				int sign = 0, cmp = 55;
				for (i = 0; px->ZhaDan[i][0] >= 0; i++)
					if (px->ZhaDan[i][0]<cmp)//选最小的炸
					{
						sign = i;//index
						cmp = px->ZhaDan[i][0];
					}
				for (i = 0; i<4; i++)
					arrToTable[nInsertCards++] = px->ZhaDan[sign][i]; //出牌 
				arrToTable[nInsertCards++] = -1;
			}
			break;
		}
		//有王炸时
		else if (px->Rocket == 1)
		{
			//自己是地主时
			if (dp->WarLine == 1 && (anotherHands[0] <= 4 || anotherHands[1] <= 4))
			{
				arrToTable[nInsertCards++] = 52;
				arrToTable[nInsertCards++] = 53;
				arrToTable[nInsertCards++] = -1;
				break;
			}
			//上家队友放弃且下家地主手牌小于等于4时
			else if (dp->WarLine == 2 && arrOnTable[0] == -1 && anotherHands[1] <= 4)
			{
				arrToTable[nInsertCards++] = 52;
				arrToTable[nInsertCards++] = 53;
				arrToTable[nInsertCards++] = -1;
				break;
			}
			//地主为上家且地主牌少于等于2时
			else if (dp->WarLine == 3 && anotherHands[0] <= 2)
			{
				arrToTable[nInsertCards++] = 52;
				arrToTable[nInsertCards++] = 53;
				arrToTable[nInsertCards++] = -1;
				break;
			}

		}

		break;
	}
	arrToTable[nInsertCards++] = -1;
	arrToTable[nInsertCards] = -2;
}