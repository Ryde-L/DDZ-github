

/*非评估搜索型策略*/
#include "ddzv100.h"

/*将有效手牌叠置前方 */
void ReSort(int cards[])
{                        
    int i,j;
	int check[21];
	for(i=0;i<21;i++) check[i]=-2;
	for(i=0,j=0;i<21;i++) if(cards[i]>=0)
       check[j++]=cards[i];
    for(i=0;i<21;i++) cards[i]=check[i];
}


/*每次轮到自己出牌时初始化手牌类，重新计算*/
void GameInit(int iOnHand[],struct PaiXing *px) 
{
	int i,j;
		for(i=0;i<21;i++) px->ShouPai[i]=-2;
	for(i=0;iOnHand[i]>=0;i++)
		px->ShouPai[i]=iOnHand[i]; //手牌录入
	/***************************
	 下面为手中各个牌型的初始化
	***************************/
	for(i=0;i<5;i++)
		for(j=0;j<4;j++)
			px->ZhaDan[i][j]=-2;
	px->Rocket=0;
	for(i=0;i<21;i++) px->DanPai[i]=-2;
	for(i=0;i<10;i++)
		for(j=0;j<2;j++)
			px->DuiPai[i][j]=-2;
	for(i=0;i<6;i++)
		for(j=0;j<3;j++)
			px->SanZhang[i][j]=-2;
	for(i=0;i<4;i++)
		for(j=0;j<15;j++)
		{
			if(j==0) px->DanShun[i][j]=0;     //单顺的长度初始化为0
			else px->DanShun[i][j]=-2;
		}
	for(i=0;i<3;i++)                          //双顺的长度初始化为0
		for(j=0;j<21;j++)
		{
			if(j==0) px->ShS[i][j]=0;
			else px->ShS[i][j]=-2;
		}
	for(i=0;i<2;i++)                          //三顺的长度初始化为0
		for(j=0;j<19;j++)
		{
			if(j==0) px->SaS[i][j]=0;
			else px->SaS[i][j]=-2;
		}
	px->Time=0;        //剩余手数
}


/*辅助策略
//查询是否有火箭*/
bool Rocket(struct PaiXing *px)
{                              
	int flag=0,JL[2];
	for(int i=0,j=0;i<21;i++)
	{
		if(px->ShouPai[i]==52||px->ShouPai[i]==53){
				flag++;
				JL[j]=i;
				j++;
		}
	}
	if(flag==2){         //确认查询到火箭
		px->Rocket=1;
		px->ShouPai[JL[0]]=-2;//火箭位置的手牌置-2
		px->ShouPai[JL[1]]=-2;
		return 1;
	}
	else
		return 0;

}


/*查询炸弹 例AAAA*/
bool BomB(int index,struct PaiXing *px)
{         
	int poker = px->ShouPai[index];
	//连续4张相同即为炸弹
	if (index >= 3)
		if (poker / 4 == px->ShouPai[index - 1] / 4 && poker / 4 == px->ShouPai[index - 2] / 4
			&& poker / 4 == px->ShouPai[index - 3] / 4){
			px->ShouPai[index] = -2;
			px->ShouPai[index - 1] = -2;
			px->ShouPai[index - 2] = -2;
			px->ShouPai[index - 3] = -2;
			return 1;
		}
	return 0;
	/*int i,j,k=0,flag=0;
	int PP=px->ShouPai[index];
	for(i=0;i<5;i++) if(px->ZhaDan[i][0]==-2)
	{                      //记录牌型类中 炸弹部分的空缺位置
			j=i;
			break;
	}
	//原for (i = 0; i < index; i++)
	for (i = 0; i<=index; i++) if (PP / 4 == px->ShouPai[i] / 4 && px->ShouPai[i] >= 0)
		flag++;            //判定条件
	if(flag==4){
		
		for (i = 0; i <= index; i++) if (PP / 4 == px->ShouPai[i] / 4)
		{
			px->ZhaDan[j][k]=px->ShouPai[i];
			px->ShouPai[i]=-2;
			k++;
		}
		return 1;
	}
	else 
		return 0;*/
}


/* 是否足够两张等值牌 TwoPieces*/
bool TwoP(int PP,struct PaiXing *px)
{                               
	int flag=0;
	for(int i=0;i<21;i++) if(PP==px->ShouPai[i]/4&&px->ShouPai[i]>=0)
		flag++;
	if(flag>=2)
		return 1;
	else 
	  return 0;
} 


/*是否足够三张等值牌
	@param：int PP 传入的是px->ShouPai[index]/4;
*/
bool ThrP(int PP,struct PaiXing *px)
{                               
	int flag=0;
	for(int i=0;i<21;i++) if(PP==px->ShouPai[i]/4 && px->ShouPai[i]>=0)
		flag++;
	if(flag>=3)
		return 1;
	else 
	  return 0;
}


/*ThirdShun三顺
	@param ：index	ShouPai开始的下标
	@return ：返回一共多少个牌或0
*/
int  ThirShun(int index,struct PaiXing *px)
{                               
	int i,sign=0,time=0;
	int PP=px->ShouPai[index];
	for(i=0;i<2;i++) if(px->SaS[i][0]<0)
	{                           //空缺位置
			sign=i;
			break;
	}
	while(1)
	{                           //对连续的三张进行判断
		if(ThrP(PP/4 - time,px)&&PP<48){
			++time;
		}
		else 
			break;
	}
	if(time >=2)
	{                           //按升序储存
		int piece=0;            //记录张数
		px->SaS[sign][0]=time;     //0位记录多少个AAA
		while(piece<time*3)
		{
			int flag=0;
			for (i = 0; i <= index&&flag != 3; i++)
			{
				if(PP/4 - time +1 + piece/3 == px->ShouPai[i]/4)
				{
					px->SaS[sign][piece+1]=px->ShouPai[i];
					px->ShouPai[i]=-2;
					flag++;
					piece++;
				}
			}
		}
		return piece;
	}
	else
		return 0;
}


/*双顺
@return：查到了多少张牌 例AABBCC为6张
*/
int  TwoShun(int index,struct PaiXing *px) 
{                               //
	int i,sign=0,time=0;
	int PP=px->ShouPai[index];
	for(i=0;i<3;i++) if(px->ShS[i][0]<=0)
	{                           //空缺位置
			sign=i;
			break;
	}
	while(1)
	{                           //对连续的对张进行判断
		if(TwoP(PP/4 - time,px)&&px->ShouPai[index]<48){
			++time;
		}
		else 
			break;
	}
	if(time >=3)
	{                           //按升序储存
		int piece=0;            //记录张数
		px->ShS[sign][0]=time;     //0位记录多少对
		while(piece<time*2)
		{ 
			int flag=0;
			for(i=0;i<=index&&flag!=2;i++)
			{                   //每次抽到一对结束循环
				if(PP/4 - time + 1 + piece/2 == px->ShouPai[i]/4)
				{
					px->ShS[sign][piece+1]=px->ShouPai[i];
					px->ShouPai[i]=-2;
					flag++;
					piece++;
				}
			}
		}
		return piece;  //返回张数
	}
	else 
		return 0;
}


/* 查询单顺*/
int  DS(int index,struct PaiXing *px)
{                              
	int i,sign=0,time=0,flag=1;
	int PP=px->ShouPai[index];
	for(i=0;i<4;i++) if(px->DanShun[i][0]<5)
	{                           //空缺位置
			sign=i;
			break;
	}
	
	while(flag)
	{                           //首先判断是否大于5 ,46,40,39,32,29
		flag=0;
		for(i=0;i<=index;i++)
        {
          if((PP/4 - time == px->ShouPai[i]/4)&&PP<48){
		    	++time;
			    flag=1;
			    break;
             }
          else
               flag=0;
		}
		if(!flag)
			break;
	}
	if(time >=5){
		int piece=1;
		px->DanShun[sign][0]=time;
		while(piece<=time)
		{
			for(i=0;i<=index;i++) if(PP/4 - time + piece == px->ShouPai[i]/4)
			{
				px->DanShun[sign][piece]=px->ShouPai[i];
				px->ShouPai[i]=-2;
				break;
			}
			piece++;
		}
		return px->DanShun[sign][0];
	}
	else 
		return 0;
}


/*辅助判断
三张
*/
bool SZ(int index,struct PaiXing *px)
{                                
	int i,sign=0,flag=2;
	int PP=px->ShouPai[index];
	for(i=0;i<6;i++) if(px->SanZhang[i][0]<0)
	{                           //空缺位置
			sign=i;
			break;
	}
	if(ThrP(PP/4,px))
	{
		for(i=0;i<=index&&flag>=0;i++) if(PP/4 == px->ShouPai[i]/4)
		{                      //升序
			px->SanZhang[sign][flag--]=px->ShouPai[i];
			px->ShouPai[i]=-2;
		}
		return 1;
	}
	else
		return 0;
}


/*策略方案
按下列顺序规则查询手上的牌型
火箭>炸弹>三顺>双顺>单顺>三张>对子>单张
*/
void Query(struct PaiXing *px)  
{                               
	bool key=1;
	int i,piece=0;
	ReSort(px->ShouPai);
	pokersort(px->ShouPai);
	for(i=0;i<21;i++) if(px->ShouPai[i]>=0&&px->ShouPai[i]>px->ShouPai[piece])
		piece=i;  //记录当前查询最大牌值的单牌位置，用其搜索
	
	while(1)
	{
		key=0;   //用于下方的手牌判定
	    ReSort(px->ShouPai);
		pokersort(px->ShouPai);
		for(i=0;i<21;i++)
			if(px->ShouPai[i]>=0)
			{ 
				key=1;
				break;
			}
		if(key==0) break;  //判定手牌是否分完
		
		
	///////////////////////////////////////////////////////////////
        int pp;
		if(Rocket(px)){
                                   //判定火箭
			         piece-=2;
			         continue;  
		}
		else if(BomB(piece,px)){
                                   //判定炸弹
			piece-=4;
			continue;              
		}
	    if(pp=ThirShun(piece,px)){      
			                       //三顺
			piece-=pp;
			continue;
		}
        if(pp=TwoShun(piece,px)){      
			                       //双顺
			piece-=pp;
			continue;
		}
        if(pp=DS(piece,px)){       
			                       //单顺
			piece-=pp;
			continue;
		}
		if(SZ(piece,px)){          //三张
			piece-=3;    
			continue;
		}
		else if(TwoP(px->ShouPai[piece]/4,px))
		{                          //对牌
			int sign,flag=0;      
			int PP=px->ShouPai[piece];
			for(i=0;i<10;i++) if(px->DuiPai[i][0]<0)
			{                           //空缺位置
				sign=i;
				break;
			}
			for(i=0;i<21;i++) if(PP/4 == px->ShouPai[i]/4)
			{
				if(flag==2)
					break;
				px->DuiPai[sign][flag]=px->ShouPai[i];
				px->ShouPai[i]=-2;
				flag++;
				
			}
			piece-=2;
			continue;
		}
		else
		{                              //单排处理
			int j=0;
			for(i=0;i<21;i++) if(px->DanPai[i]==-2)
			{
				j=i;
				break;
			}
			px->DanPai[j]=px->ShouPai[piece];
			px->ShouPai[piece]=-2;
			piece--;
		}
	}
	pokersort(px->DanPai);  //单牌排升序;

	int j,temp1,temp2;      //对牌排升序
	for(i=0;px->DuiPai[i][0]>=0;i++)         
		for(j=i+1;px->DuiPai[j][0]>=0;j++)
			if(px->DuiPai[i][0]>px->DuiPai[j][0])
			{
				temp1=px->DuiPai[i][0];
				temp2=px->DuiPai[i][1];
				px->DuiPai[i][0]=px->DuiPai[j][0];
				px->DuiPai[j][0]=temp1;
				px->DuiPai[i][1]=px->DuiPai[j][1];
				px->DuiPai[j][1]=temp2;
			}
}


/*牌桌策略*/
void AllHand(struct PaiXing *px)
{
	int i;
	px->Time=0;
	if(px->Rocket) px->Time++; 
	for(i=0;px->ZhaDan[i][0]>=0;i++)    px->Time++; 
	for(i=0;px->DanPai[i]>=0;i++)       px->Time++;
	for(i=0;px->DuiPai[i][0]>=0;i++)    px->Time++;
	for(i=0;px->SanZhang[i][0]>=0;i++)  px->Time++;
	for(i=0;px->DanShun[i][0]>0;i++)    px->Time++;
	for(i=0;px->ShS[i][0]>0;i++)        px->Time++;
	for(i=0;px->SaS[i][0]>0;i++)        px->Time++;

}


/*
另外两家手牌计算
	@param：int cards[] 桌上牌

	=========================
	疑问：只能算iOnTable[0]和iOnTable[1]的牌数？
*/
void Calcu(int cards[],struct ddz *dp)
{                         
	int k=0,key=2;
	for(int i=0;key;i++)//2次大循环
	{
		if(cards[i]==-1)//即iOnTable换行
		{
			key--;
			k++;			
		}
		else 
			dp->HandCard[k]--;//其他玩家k的牌数减一
	}	
}


/*  确定地主相对自己的位置
	1.【农 AI 农】 2.【友 AI 地】 3.【地 AI 友】
*/
void Lineup(struct ddz *dp)
{                           
    //我的位置 cDir 地主的位置 cLandlord

	if(dp->cDir==dp->cLandlord)     
		dp->WarLine=1; //地主
	else if(dp->cDir<dp->cLandlord||(dp->cDir=='C'&&dp->cLandlord=='A'))         
	    dp->WarLine=2; //地主在自己后手AB BC CA 
	else if(dp->cDir>dp->cLandlord||(dp->cDir=='A'&&dp->cLandlord=='C'))            
		dp->WarLine=3; //地主在自己先手CA AB BC
}
