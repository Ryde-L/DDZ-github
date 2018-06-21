#include <stdio.h> //8.17�ö���
#include <string.h>
#include <iostream>
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
using namespace std;

struct ddz
{
	int  iStatus;				//����״̬-1����,0����,1��ʼ
	char sCommandIn[80];		//ͨ����������
	char sCommandOut[80];		//ͨ���������
	int iOnHand[21];            //�����ƣ�����ֵ��ʼ��Ϊ-1��
	int iOnTable[162][21];      //�Գ������飨����ֵ��ʼ��Ϊ-2��ÿ����һ���ƣ���-1��β��Pass��Ϊ-1
	int iToTable[21];           //Ҫ������

	char sVer[80];			    //Э��汾��
	char sName[80];		        //����ѡ�ֳƺ�
	char cDir;			        //��ҷ�λ���
	char cLandlord;	            //������ҷ�λ���
	char cWinner;	            //ʤ���߷�λ���
	int iBid[3];                //���ƹ���
	int iBmax;                  //��ǰ��������ֵ��{0,1,2,3}
	int iOTmax;                 //��ǰ��������
	int iRoundNow;              //��ǰ�ִ�
	int iRoundTotal;            //���ܾ���
	int iLevelUp;	            //����ѡ����
	int iScoreMax;	            //ת���÷ֽ���
	int iVoid;			        //��Ͼ���м������
	int iTurnNow;               //��ǰ�ִ�
	int iTurnTotal;             //������

	int HandCard[2];            //�����������е�ʣ������
	int WarLine;                //����ȷ�� 1.��ũ AI ũ�� 2.���� AI �ء� 3.���� AI �ѡ�
};
struct PaiXing{
	int ShouPai[21];       //�ݴ����� 
	bool Rocket;             //���޻�� 1  52 53
	int ZhaDan[5][4];      //����ը�� 2
	int DanPai[21];        //���� 3
	int DuiPai[10][2];     //���� 4
	int SanZhang[6][3];    //���� 5
	                       //����һ ������  6 
	int DanShun[4][15];    //���浥˳  0λΪ��˳�ж����� 7
	int ShS[3][21]; //˫˳ 0Ϊ���ٶ� 8
	int SaS[2][19]; //��˳ �����������ѯ���������� 9   
	// ��˳���� 0Ϊ���� 10
	// �Ĵ�2 �Ĵ�1 �ж� 11  
	int Time;    //ʣ������
};
void FreeToTable(struct PaiXing *px,int ToTable[],int &Num,int Line,int Hands[])    //����
{                           //�����ֳ���ʱ ��˳>˫˳>��˳>����>����>���� 
	int i,sign=0,cmp;
	if(px->SaS[0][0]>0)
	{
		cmp=px->SaS[0][1]/4;     //��С�Ƴ���
		for(i=0;px->SaS[i][0]>0;i++)
		{
			if( cmp > px->SaS[i][1]/4 ){                                 
				cmp=px->SaS[i][1]/4;
				sign=i;
			}
		}
		if(px->SaS[sign][px->SaS[sign][0]*3]<40||px->SaS[sign][0]>=4)
		{                              //���һ������ҪС��K �������� AAABBBCCCDDD
			int count=0;//�������жϴ���
			for(i=0;px->DanPai[i]>=0;i++) count++;	
			if(count>=px->SaS[sign][0])
			{                               //�ȴ������,�ж��ٴ����٣����ܴ��Ĳ�������
				for(i=0;i<count;i++)
					ToTable[Num++]=px->DanPai[i];
				for(i=1;px->SaS[sign][i]>=0;i++)
					ToTable[Num++]=px->SaS[sign][i];
			}
			else if(count < px->SaS[sign][0] && count>=2)
			{
				for(i=0;i<count;i++)
					ToTable[Num++]=px->DanPai[i];
				for(i=1;i<count*3;i++)
					ToTable[Num++]=px->SaS[sign][i];
			}
			else 
				for(i=1;px->SaS[sign][i]>=0;i++)
					ToTable[Num++]=px->SaS[sign][i];
			ToTable[Num++]=-1;
		}
	}

	if(px->ShS[0][0]>0)
	{
		sign=0;
		cmp=px->ShS[0][1]/4;      
		for(i=0;px->ShS[i][0]>0;i++)
		{
			if( cmp > px->ShS[i][1]/4 ){                                 
				cmp=px->ShS[i][1]/4;
				sign=i;
			}
		}
		if(px->ShS[sign][px->ShS[sign][0]*2]<44||px->ShS[sign][0]>=5)
		{                            //���һ����С��A �������� AABBCCDDEE
			for(i=1;px->ShS[sign][i]>=0;i++)
				ToTable[Num++]=px->ShS[sign][i];
			ToTable[Num++]=-1;
		}
	}

	if(px->DanShun[0][0]>0)
	{
		sign=0;
		cmp=px->DanShun[0][1]/4;   //��С�Ƴ���
		for(i=0;px->DanShun[i][0]>0;i++)
		{
			if((cmp > px->DanShun[i][1]/4)||
				(cmp == px->DanShun[i][1]/4 && px->DanShun[sign][0]/4 < px->DanShun[i][0]/4))
			{//��С�Ƹ�С �� ��С����ͬ&&������ ������                                  
				cmp=px->DanShun[i][1]/4;
				sign=i;
			}
		}
		if(px->DanShun[sign][px->DanShun[sign][0]]<44||px->DanShun[sign][0]>=7 || px->Time<=3)
		{                             //���һ����С��A �������� 3456789 ������С��3
			for(i=1;px->DanShun[sign][i]>=0;i++)
				ToTable[Num++]=px->DanShun[sign][i];
			ToTable[Num++]=-1;
		}
	}

	if(px->SanZhang[0][0]>=0)
	{
		sign=0;
		cmp=px->SanZhang[0][0];
		for(i=0;px->SanZhang[i][0]>=0;i++) if(cmp > px->SanZhang[i][0])
		{                         
			cmp= px->SanZhang[i][0];
			sign=i;
		}
		if(px->SanZhang[sign][0] <40||
			(px->Time<=3&&(px->DanPai[0]>=0||px->DuiPai[0][0]>=0)) )
		{                                       //KKK AAA 222 ���� 
			for(i=0;px->SanZhang[sign][i]>=0;i++)
				ToTable[Num++]=px->SanZhang[sign][i];
			//����е��ƺͶ��� ����������� AA22  A 2����
			int count1=0,count2=0;
			for(i=0;px->DanPai[i]>=0&&(px->DanPai[i]<44||px->Time<4);i++) count1++;
			for(i=0;px->DuiPai[i][0]>=0&&(px->DuiPai[i][0]<44||px->Time<4);i++) count2++;
			if(count2>=count1 && count2!=0) 
			{
				ToTable[Num++]=px->DuiPai[0][0];
				ToTable[Num++]=px->DuiPai[0][1];
			}
			else if(count1>count2) ToTable[Num++]=px->DanPai[0];
			ToTable[Num++]=-1;
		}
	}
	if(px->DuiPai[0][0]>=0 && (px->DuiPai[0][0]< 44||px->Time<=2))
	{  //���K�Ķ����ݲ����� ��AA 22��
		int count1=0,count2=0;
		for(i=0;px->DanPai[i]>=0;i++) count1++;
		for(i=0;px->DuiPai[i][0]>=0;i++) count2++;
		if(count2<count1&&px->DuiPai[0][0]/4 - px->DanPai[0]/4 <=4)  // ���ƱȽ��ٵ�ʱ���ȳ�
		{
			if(px->SanZhang[0][0]>=0&&px->SanZhang[0][0]<48)
				sign=1;
			else sign=0;

			ToTable[Num++]=px->DuiPai[sign][0];
			ToTable[Num++]=px->DuiPai[sign][1];
			ToTable[Num++]=-1;
		}
		else if(count2==count1==1)
		{
			if(px->DuiPai[0][0]<px->DanPai[0])
			{
				ToTable[Num++]=px->DuiPai[sign][0];
				ToTable[Num++]=px->DuiPai[sign][1];
				ToTable[Num++]=-1;
			}
		}
	}
	if(px->DanPai[0]>=0 && (px->Time>3||px->SanZhang[0][0]<0) ){
		ToTable[Num++]=px->DanPai[0];
		ToTable[Num++]=-1;
	}
///////////////////////////����Ϊ��������ͽԲ���������ʱʱ���ǲ���/////////////////////////////////////////

	if(px->SaS[0][0]>0)
	{                                          //��˳
		cmp=px->SaS[0][1]/4;     //��С�Ƴ���
		for(i=0;px->SaS[i][0]>0;i++)
		{
			if( cmp > px->SaS[i][1]/4 ){                                 
				cmp=px->SaS[i][1]/4;
				sign=i;
			}
		}
		int count=0;//�������жϴ���
		for(i=0;px->DanPai[i]>=0;i++) count++;  
		if(count>=px->SaS[sign][0])
		{                               //�ȴ������,�ж��ٴ�����
			for(i=0;i<count;i++)
				ToTable[Num++]=px->DanPai[i];
			for(i=1;px->SaS[sign][i]>=0;i++)
				ToTable[Num++]=px->SaS[sign][i];
		}
		else if(count < px->SaS[sign][0] && count>=2)
		{
			for(i=0;i<count;i++)
				ToTable[Num++]=px->DanPai[i];
			for(i=1;i<count*3;i++)
				ToTable[Num++]=px->SaS[sign][i];
		}
		else 
			for(i=1;px->SaS[sign][i]>=0;i++)
				ToTable[Num++]=px->SaS[sign][i];
		ToTable[Num++]=-1;
	}

	if(px->ShS[0][0]>0)
	{                                 //˫˳
		sign=0;
		cmp=px->ShS[0][1]/4;        //��С�Ƴ���
		for(i=0;px->ShS[i][0]>0;i++)
		{
			if( cmp > px->ShS[i][1]/4 ){                                 
				cmp=px->ShS[i][1]/4;
				sign=i;
			}
		}
		for(i=1;px->ShS[sign][i]>=0;i++)
			ToTable[Num++]=px->ShS[sign][i];
		ToTable[Num++]=-1;
	}

	if(px->DanShun[0][0]>0)
	{                                 //��˳
		sign=0;
		cmp=px->DanShun[0][1]/4;   //��С�Ƴ���
		for(i=0;px->DanShun[i][0]>0;i++)
		{
			if((cmp > px->DanShun[i][1]/4)||
				(cmp == px->DanShun[i][1]/4 && px->DanShun[sign][0]/4 < px->DanShun[i][0]/4))
			{//��С�Ƹ�С �� ��С����ͬ&&������ ������                                  
				cmp=px->DanShun[i][1]/4;
				sign=i;
			}
		}
		for(i=1;px->DanShun[sign][i]>=0;i++)
			ToTable[Num++]=px->DanShun[sign][i];
		ToTable[Num++]=-1;
	}

	if(px->SanZhang[0][0]>=0){                     //����
		sign=0;                
		cmp=px->SanZhang[0][0];
		for(i=0;px->SanZhang[i][0]>=0;i++) if(cmp > px->SanZhang[i][0])
		{                         
			cmp= px->SanZhang[i][0];
			sign=i;
		}
		for(i=0;px->SanZhang[sign][i]>=0;i++)
			ToTable[Num++]=px->SanZhang[sign][i];

		int count1=0,count2=0;
		for(i=0;px->DanPai[i]>=0&&(px->DanPai[i]<44||px->Time<=3);i++) count1++;
		for(i=0;px->DuiPai[i][0]>=0&&(px->DuiPai[i][0]<44||px->Time<=3);i++) count2++;

		if(count2>=count1 && count2!=0) 
		{
			ToTable[Num++]=px->DuiPai[0][0];
			ToTable[Num++]=px->DuiPai[0][1];
		}
		else if(count1>count2) ToTable[Num++]=px->DanPai[0];
		ToTable[Num++]=-1;
	}
	if(px->Rocket&&px->Time<=2)
	{
		ToTable[Num++]=52;
		ToTable[Num++]=53;
	    ToTable[Num++]=-1;
	}
	if(px->ZhaDan[0][0]>=0&&px->Time<=3){
		for(i=0;i<4;i++)
			ToTable[Num++]=px->ZhaDan[0][i];
		ToTable[Num++]=-1;
	}

	if(px->DuiPai[0][0]>=0){  //����
		ToTable[Num++]=px->DuiPai[0][0];
		ToTable[Num++]=px->DuiPai[0][1];
		ToTable[Num++]=-1;
	}
	if(px->DanPai[0]>=0){  //����
		ToTable[Num++]=px->DanPai[0];
		ToTable[Num++]=-1;
	}	
	
	
}
int  count(int cards[])//������Ч����
{
	int iCount=0;
	while(cards[iCount]>=0)
		iCount++;
	return iCount;
}
void copycards(int arrOnHandCopy[], int arrOnHand[])
{
	int i=0;
	for(i=0;arrOnHand[i]>=0;i++)
		arrOnHandCopy[i]=arrOnHand[i];
	arrOnHandCopy[i]=-1;
}
void mainsort(int cards[])//��totableһ���ţ����ţ����ţ�һ�����򣬷���ȷ���������ͣ����Ƚϴ�С
{
	int card_num = 0;/*�Ƶ�����*/
	int i,j,k=0,l;  
	int sorted=0;  
	int fitcount = 0;//������¼�������еĴ���  
	int sortcards[100];
	int jilu[20]={-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4};
	int JL=0;
	int JLX=0;

	for (i=0;cards[i]>-1;i++)//��ѯ�����Ƶ�����
		card_num++;

	for(i=4;i>0;i--)//���ҳ�һ���������ƣ�������д��sortcards�����У�Ȼ����Ѱ�����ŵģ�һ������	  
	{
		fitcount = 1;  
		for(j=0;j<card_num;j++)
		{		  
			if (j==card_num-1)
			{
				if(cards[j]/4!=cards[j-1]/4)
					if(fitcount==i)
					{  				
						sortcards[card_num-1]=cards[j];
					}       	
			}
			else
			{
				k=0;
				for(JLX=0; jilu[JLX]>0;  JLX++)
				{
					if(jilu[JLX]/4==cards[j]/4)
						k=-1;
				}
				if(cards[j]/4==cards[j+1]/4 && k==0) 
					fitcount++;
				else 
					fitcount=1;						

				if(fitcount==i && k==0)
				{  
					if(fitcount>1)
					{
						j++;  
					}
					for(k=0;k<i;k++)
					{  
						sortcards[sorted++]=cards[j-i+1+k];
					} 
					jilu[JL++]=sortcards[sorted-1];//��ֹ�Ѿ����й����ŵ��ƣ��ٲμӶ��ŵ�����
					fitcount=1;  
				}       
			}  
		}
	}
	sortcards[card_num]=-1;
	for (l=0;l<=card_num;l++)//�����кõ��ƣ�������ԭ����������	
		cards[l]=sortcards[l];
} 
void pokersort(int cards[])//��cards��С��������
{
	int i,j;
	int temp;//����ֵ���м����

	for(i=0;cards[i]>=0;i++)
	{
		for(j=i+1;cards[j]>=0;j++)
			if(cards[i]>cards[j])
			{
				temp=cards[i];
				cards[i]=cards[j];
				cards[j]=temp;
			}
	}
}
int  cardtype(int nCardsArr[])
{
	int i = 0;
	int nFlag = 0;
	int nLength = 0;

	pokersort(nCardsArr);
	mainsort(nCardsArr);
	
	for (i=0;nCardsArr[i]>-1;i++)//��ѯ�����Ƶ�����
		nLength++;
	//cout<<"cards"<<nCardsArr[0]<<" "<<nCardsArr[1]<<" "<<nCardsArr[2];
	//����������Ϊ��
	if (nLength == 0)
		return 0;

	//���
	if (nLength == 2 && nCardsArr[0] == 52 && nCardsArr[1] == 53)//�Ƶ�����Ϊ������Ϊ������С��
		return 1;

	//ը��
	if (nLength == 4 	//�Ƶ�����Ϊ���������Ƶĵ������
		&& nCardsArr[0]/4 == nCardsArr[1]/4 && nCardsArr[1]/4 == nCardsArr[2]/4 && nCardsArr[2]/4 == nCardsArr[3]/4)
		return 2;
	/************************************************************************/
	/* �����Ҫ������33334444���Ϳ��Խ�����ע��ȥ��                                                                     */
	/************************************************************************/
// 	if (nLength == 8
// 		&& nCardsArr[0]/4 == nCardsArr[1]/4 && nCardsArr[1]/4 == nCardsArr[2]/4 && nCardsArr[2]/4 == nCardsArr[3]/4
// 		&& nCardsArr[3]/4+1 == nCardsArr[4]/4	&& nCardsArr[7]/4 != 12//����������������Ƶ������һ�������Ʋ�Ϊ��
// 		&& nCardsArr[4]/4 == nCardsArr[5]/4 && nCardsArr[5]/4 == nCardsArr[6]/4 && nCardsArr[6]/4 == nCardsArr[7]/4)
// 		return 2;
// 	if (nLength == 12
// 		&& nCardsArr[0]/4 == nCardsArr[1]/4 && nCardsArr[1]/4 == nCardsArr[2]/4 && nCardsArr[2]/4 == nCardsArr[3]/4
// 		&& nCardsArr[3]/4+1 == nCardsArr[4]/4 && nCardsArr[11]/4 != 12	//����������������Ƶ������һ�������Ʋ�Ϊ��
// 		&& nCardsArr[4]/4 == nCardsArr[5]/4 && nCardsArr[5]/4 == nCardsArr[6]/4 && nCardsArr[6]/4 == nCardsArr[7]/4
// 		&& nCardsArr[7]/4+1 == nCardsArr[8]/4
// 		&& nCardsArr[8]/4 == nCardsArr[9]/4 && nCardsArr[9]/4 == nCardsArr[10]/4 && nCardsArr[10]/4 == nCardsArr[11]/4)
// 		return 2;

	//����
	if (nLength == 1)	//�Ƶ�����Ϊһ
		return 3;

	//����
	if (nLength == 2										//�Ƶ�����Ϊ�� 
		&& nCardsArr[0]/4 == nCardsArr[1]/4)	//�����Ƶ�����ͬ 
		return 4;

	//����
	if(nLength == 3																							    //�Ƶ�����Ϊ��
		&& nCardsArr[0]/4 == nCardsArr[1]/4 && nCardsArr[1]/4 == nCardsArr[2]/4)	//�����Ƶ�����ͬ 
		return 5;

	//����һ
	if (nLength == 4																								//�Ƶ�����Ϊ��
		&& nCardsArr[0]/4 == nCardsArr[1]/4 && nCardsArr[1]/4 == nCardsArr[2]/4	//ǰ�����Ƶ�����ͬ 
		&& nCardsArr[3]/4!=nCardsArr[1]/4)															    //����������ǰ�����Ʋ�ͬ��	
		return 6;

	//������
	if (nLength == 5 
		&& nCardsArr[0]/4 == nCardsArr[1]/4 && nCardsArr[1]/4 == nCardsArr[2]/4	//ǰ�����Ƶ�����ͬ 
		&& nCardsArr[3]/4 == nCardsArr[4]/4	)															//�������Ƶ�����ͬ
		return 6;

	//��˳
	int nFlagOfSingleShun = 1;
	for (i=0;i<nLength-1;i++)
	{
		if (nCardsArr[i]/4 == nCardsArr[i+1]/4-1	//ǰһ�������һ���Ƶ������һ
			&& nLength>=5								//�Ƶĳ�������ڵ�����
			&& nCardsArr[nLength-1]<48)			//�Ƶ����������ܳ���A��������Ϊ����С����������														
			continue;
		else
		{
			nFlagOfSingleShun = 0;
			break;
		}
	}
	if (nFlagOfSingleShun)
		return 7;

	//˫˳
	nFlag = 0;
	//�Ƶ���������ڵ�������Ϊż�������һ���ƵĴ�С��С�ڶ�
	if (nLength>=6 && nLength%2 == 0 && nCardsArr[nLength-1]<48)   
		for (i=0;i<nLength/2;i++)
		{ 
			if (nCardsArr[2*i]/4 == nCardsArr[2*i+1]/4)//�������ŵ�����ͬ
				nFlag++;
			else
				break;
			if (i!=nLength/2-1 && nCardsArr[2*i]/4 == nCardsArr[2*(i+1)]/4-1)//���һ���Ƶ����Ƶ������һ
				nFlag++;
			else//�������������ƣ���������ʹ���������������һ������Ϊ���Ƽ���
				if (i == nLength/2-1)
					nFlag++;
				else
					break;
		}
	if (nFlag == nLength)
		return 8;

	//��˳
	nFlag = 0;	
	if (nLength>=6 && nLength%3 == 0)							//�Ƶ������������,��Ϊ���ı���
		for (i=0;i<nLength/3;i++)
		{																				//���������������
			if (nCardsArr[i*3]/4 == nCardsArr[i*3+1]/4 && nCardsArr[i*3+1]/4 == nCardsArr[i*3+2]/4)
				nFlag++;
			else 
				break;
			if (nCardsArr[i*3]/4 == nCardsArr[3*(i+1)]/4-1)	//��������Ƶ����Ƶ������һ,�Ҳ�Ϊ���������
				nFlag += 2;  
			else
				if (i == nLength/3-1)
					nFlag += 2;
				else 
					break;
		}			 
	if (nFlag == nLength)
		return 9;

	//�ɻ�����
	nFlag = 0;	
	if (nLength>=8 && nLength%4 == 0)								//�Ƶ���������ڰˣ���Ϊ�ĵı���
		for (i=0;i<nLength/4;i++)											//���ȳ����ļ�Ϊ������˳���Ƶ�����
		{																				//���������������
			if (nCardsArr[i*3]/4 == nCardsArr[i*3+1]/4 && nCardsArr[i*3+1]/4 == nCardsArr[i*3+2]/4)
				nFlag++;
			else 
				break;
			if (nCardsArr[i*3]/4 == nCardsArr[3*(i+1)]/4-1)	//��������Ƶ����Ƶ������һ,�Ҳ�Ϊ���������
				nFlag += 2;  
			else
				if (i == nLength/4-1)
					nFlag += 2;
				else 
					break;
		}			 
	if ((nLength-nFlag) == nFlag/3)//��˳�ƵĲ�ͬ���������뵥����Ŀ��ͬ
		return 10;

	//�ɻ���˫	
	nFlag = 0;	
	int temp = 0;

	if (nLength>=10 && nLength%5 == 0)							//�Ƶ����������ʮ,��Ϊ��ı���
		for (i=0;i<nLength/5;i++)
		{																				//���������������
			if (nCardsArr[i*3]/4 == nCardsArr[i*3+1]/4 && nCardsArr[i*3+1]/4 == nCardsArr[i*3+2]/4)
				nFlag++;
			else 
				break;
			if (nCardsArr[i*3]/4 == nCardsArr[3*(i+1)]/4-1)	//��������Ƶ����Ƶ������һ,�Ҳ�Ϊ���������
				nFlag += 2;  
			else
				if (i == nLength/5-1)
					nFlag += 2;
				else 
					break;
		}			 
	if ((nLength-nFlag)%2 == 0 && (nLength-nFlag)/2 == nFlag/3)//�����ĸ�������������ͬ�ĵ����ĸ������
	{  
		temp = nFlag;//��¼����������ͬ��������
		for (i=0;i<temp/3;i++)//��������ͬ�ĵ����ĸ�����Ϊ��������ĸ���
		{
			if (nCardsArr[2*i+temp]/4 == nCardsArr[2*i+1+temp]/4)
				nFlag+=2;
		}
	}
	if (nFlag == nLength)
		return 10;

	//�Ĵ�����
	if (nLength == 6					//�Ƶ�����Ϊ��
		&& nCardsArr[0]/4 == nCardsArr[1]/4 && nCardsArr[1]/4 == nCardsArr[2]/4 && nCardsArr[2]/4 == nCardsArr[3]/4  //ǰ���������
		&& nCardsArr[4]/4 != nCardsArr[5]/4)//�����Ų���Ϊ����
		return 11;

	//�Ĵ���˫
	if (nLength == 8					//�Ƶ�����Ϊ��
		&& nCardsArr[0]/4 == nCardsArr[1]/4 && nCardsArr[1]/4 == nCardsArr[2]/4&& nCardsArr[2]/4 == nCardsArr[3]/4 //ǰ���������
		&& nCardsArr[4]/4 == nCardsArr[5]/4 && nCardsArr[6]/4 == nCardsArr[7]/4
		&& nCardsArr[7]!=53		 //������Ų���Ϊ��ը
		)
		return 11;
	return -1;
}
void init(struct ddz * dp)					//���ݳ�ʼ��
{
	int i,j;
	dp->iStatus=1;							//��ʼ
	dp->iBmax=0;							//���Ʒ���
	dp->iOTmax=0;							//����������

	dp->iBid[0] = 0;
	dp->iBid[1] = 0;
	dp->iBid[2] = 0;
	for(i=0;i<162;i++)						//��ʼ��������
        	for(j=0;j<21;j++)
			dp->iOnTable[i][j]=-2;
	for(i=0;i<21;i++)						//��ʼ������
		dp->iOnHand[i]=-2;

	dp->HandCard[0]=17;    //��ʼ��������
	dp->HandCard[1]=17;
}
void iTos(int iToTable[],char sCommandOut[])	//��iToTable[]�е�����ת��ΪsCommandOut[]�ַ���
{
	int i;
	char t[4];
	if(iToTable[0]==-1)
    {
        t[0]='-';
        t[1]='1';
        t[3]='\0';
        strcat(sCommandOut,t);
    }
    else
    {
        for(i=0;iToTable[i]>=0;i++)
        {
            if(iToTable[i]>=10)
            {
                t[0]=iToTable[i]/10+'0';
                t[1]=iToTable[i]%10+'0';
                t[2]=',';
                t[3]='\0';
            }
            else
            {
                t[0]=iToTable[i]%10+'0';
                t[1]=',';
                t[2]='\0';
            }
            strcat(sCommandOut,t);
        }
    }

	sCommandOut[strlen(sCommandOut)-1]='\0';
}
void TakeOff(int iOnHand[],int iToTable[])	//��iToTable[]�е��ƴ�iOnHand[]���޳�
{
	int i,j,k;
	for(i=0;iToTable[i]>=0;i++)
		for(j=0;iOnHand[j]>=0;j++)
			if(iToTable[i]==iOnHand[j])
            {
				for(k=j+1;iOnHand[k]>=-1&&k<21;k++)
					iOnHand[k-1]=iOnHand[k];
				break;
			}
}


void GetDou(struct ddz * dp)					//�汾��Ϣ
{
	int i;
	for(i=0; dp->sCommandIn[i]!='\0'; i++)
		dp->sVer[i]=dp->sCommandIn[i];
	dp->sVer[i]='\0';
	strcpy(dp->sCommandOut,"ZSC_DDZ");
}

void GetInf(struct ddz * dp)					//�ִ���Ϣ������:����INFO 1/4 1/9 9 2450 ���OK INFO
{
	int a,b,c,d;
	dp->iTurnNow    =dp->sCommandIn[ 5]-'0';	//��ǰ�ִ�
	dp->iTurnTotal  =dp->sCommandIn[ 7]-'0';	//������
	dp->iRoundNow   =dp->sCommandIn[ 9]-'0';	//��ǰ�ִ�
	dp->iRoundTotal =dp->sCommandIn[11]-'0';	//�ܾ���
	dp->iLevelUp    =dp->sCommandIn[13]-'0';	//����ѡ����
	a = dp->sCommandIn[15]-'0';
	b = dp->sCommandIn[16]-'0';
	c = dp->sCommandIn[17]-'0';
	d = dp->sCommandIn[18]-'0';
	dp->iScoreMax = a*1000+b*100+c*10+d;	//�÷ֽ���
	strcpy(dp->sCommandOut,"OK INFO");
}

void GetDea(struct ddz * dp)		//������Ϣ
{
	int i,j=0,t;
	dp->cDir=dp->sCommandIn[5];     //��ȡ����AI��λ���

	//��DEAL A0,4,5,��51,53�е������� ������iOnHand[]��
	dp->iOnHand[0]=0;
	dp->iOnHand[1]=-1;
	for(i=6;dp->sCommandIn[i]!='\0';i++)
    {
		if(dp->sCommandIn[i]!=',')
			dp->iOnHand[0]=dp->iOnHand[0]*10+dp->sCommandIn[i]-'0';
		else
		{
			for(j=21;j>=1;j--)
				dp->iOnHand[j]=dp->iOnHand[j-1];
			dp->iOnHand[0]=0;
		}
	}
	for(i=0;i<9;i++)
    {
		t=dp->iOnHand[i];
		dp->iOnHand[i]=dp->iOnHand[16-i];
		dp->iOnHand[16-i]=t;
	}
	strcpy(dp->sCommandOut,"OK DEAL");  //�ظ���Ϣ
    pokersort(dp->iOnHand);  //iOnHand[]��С��������

}

void GetLef(struct ddz * dp)			 //���������Ϣ
{
    int i,j=17;
    dp->cLandlord=dp->sCommandIn[9];     //ȷ��������
	
	if(dp->sCommandIn[9]==dp->cDir)
    {
		for(j=21;j>=1;j--)
			dp->iOnHand[j]=dp->iOnHand[j-1];
		dp->iOnHand[0]=0;
		for(i=10;dp->sCommandIn[i]!='\0';i++)
		{
			if(dp->sCommandIn[i]!=',')
			dp->iOnHand[0]=dp->iOnHand[0]*10+dp->sCommandIn[i]-'0';
			else
			{
				for(j=21;j>=1;j--)
					dp->iOnHand[j]=dp->iOnHand[j-1];
				dp->iOnHand[0]=0;
			}
		}
	}
	else 
	{
		int cmp = dp->cLandlord - dp->cDir;  //�������Լ������λ�ã�����0 ����1
		if(cmp==-1||cmp==2)
			dp->HandCard[0]+=3;  
		else if(cmp==1||cmp==-2)
			dp->HandCard[1]+=3;         //  ���Ƽ���+=3
	}
	strcpy(dp->sCommandOut,"OK LEFTOVER");
	pokersort(dp->iOnHand);					//iOnHand[]��С��������
}


void GetGam(struct ddz * dp)				//GAMEOVER ָ���
{
    dp->cWinner=dp->sCommandIn[9];			//ʤ���߷�λ���
	strcpy(dp->sCommandOut,"OK GAMEOVER");
	//cout<<dp->sCommandOut<<endl;
	if(dp->iRoundNow==dp->iRoundTotal)		//��ǰ������ÿ�־����ʱ
		dp->iStatus=0;						//����״̬������Ϊ����
	else									//����
		init(dp);							//����״̬������Ϊ���¿�ʼ
}
void helpdz(int arrOnHand[],int arrOnTable[],int arrToTable[],int Hands[],struct PaiXing *px,int Line)//��arrToTable�з������п�������-1���,-2����
{
	int nCardToPlay;//ȷ����������������
	int nInsertCards = 0;//��arrToTable�в�����Գ�����
	int nNumToCompare;//�Է������Ƶĵ�һ�ţ����ԱȽϴ�С
	int nToTableCardsNum = 0;//���ȥ�ĳ���
	int i = 0;
	int j = 0;
	if (arrOnTable[0]==-1 && arrOnTable[1]==-1 || arrOnTable[0]==-2)//���ҷ�������������
	{
		nCardToPlay = 0;	
	}
	else  //Line����ȷ�� 1.��ũ AI ũ�� 2.���� AI �ء� 3.���� AI �ѡ�
		if(arrOnTable[0] == -1)//һ�ҷ��������͸������ּ��ж�
		{
			//cout<<arrOnTable[0]<<" "<<arrOnTable[1]<<" "<<arrOnTable[2]<<endl;
			if(arrOnTable[1]>=0 && arrOnTable[1]<4 && arrOnTable[2]<0)
			{
				nCardToPlay=3;  //mainsort��ջ�����⣬�ռ䲻�㱻���ã���ʱ�Ѵ˶����⻯
				nNumToCompare=0;
			}
			else if(Line==3&& (px->Time>3 ||(Hands[0]==1&& px->DanPai[1]<48 && px->DanPai[i]>=0) )) 
			{
				nCardToPlay=12;  //���������Ҫ
			}
			else
			{
				//pokersort(arrOnTable+1);
				//mainsort(arrOnTable+1);
				nCardToPlay = cardtype(arrOnTable+1);
				nNumToCompare = arrOnTable[1];
				nToTableCardsNum = count(arrOnTable+1);
				if (nNumToCompare < 52)
					nNumToCompare = nNumToCompare/4;
			}
		}
		else//���͸������ּ��ж�
		{
			if(Line==2 && Hands[0]<=6 && Hands[1]>2) nCardToPlay=12;
			else
			{
				nCardToPlay = cardtype(arrOnTable);
				nNumToCompare = arrOnTable[0];
				nToTableCardsNum = count(arrOnTable);
				if (nNumToCompare < 52)
					nNumToCompare = nNumToCompare/4;
			}
		}

		bool over=0;
		int arrOnHandCopy[22];
		int n = 0;
		int nDelCards = 0;//ɾ���������ظ����ƣ�ֻ�������е�һ��
		int nCardsLength = 0;//����֪�������Ƶ������Ƿ���ϣ���ĵ�˳������ͬ
		int nCanInsert = 0;
		
		switch(nCardToPlay)
		{
		case 0:
			FreeToTable(px,arrToTable,nInsertCards,Line,Hands);
			break;

		case 1://���			
			break;

		case 2://ը��
			if(px->ZhaDan[0][0]>=0)
			{
				int sign=0,cmp=55;
				for(i=0;px->ZhaDan[i][0]>=0;i++) 
					if(px->ZhaDan[i][0]>nNumToCompare &&
						px->ZhaDan[i][0]<cmp)
					{
						sign=i;
						cmp=px->ZhaDan[i][0];
					}
				for(i=0;i<4;i++)
					arrToTable[nInsertCards++] = px->ZhaDan[sign][i];  
				arrToTable[nInsertCards++] = -1;   
			}
			break;

		case 3://����       ���ݴ���ǿ�Ʋ�������  ����һ����·   ����ing************************************************************
			if(px->DanPai[0]>=0 || px->DuiPai[0][0]>=0 || px->SanZhang[0][0]>=0)
			{                  //���ǲ���
				bool key=0;
				for(i=0;px->DanPai[i]>=0;i++) if( px->DanPai[i]/4 > nNumToCompare && px->DanPai[i]/4-nNumToCompare<=4)
				{
					arrToTable[nInsertCards++] = px->DanPai[i];  
					arrToTable[nInsertCards++] = -1;    
					key=1;
				}
				//��Line��ȷ���Ƿ������� ���A�����ſ��Բ�
				if(!key&&px->DanPai[0]<0&& (px->SanZhang[0][0]>=44&&px->Time<=3||
				  (
				    ((Line==1&&(Hands[0]<=2||Hands[1]<=2)) ||
					 (Line==2&&Hands[1]<=2)||
					 (Line==3&&Hands[0]<=2)) )  ))
				{
					key=0;  //����ȷ�ϻ�ȡ�ɳ���
					for(i=0;px->SanZhang[i][0]>=0;i++) if( px->SanZhang[i][0]/4 > nNumToCompare )
					{
						arrToTable[nInsertCards++] = px->SanZhang[i][0];  
						arrToTable[nInsertCards++] = -1;   
						key=1;
						break;
					}
				}
				//���2�Ķ��ƿ��Բ�
				if(!key &&px->DanPai[0]<0&&( px->DuiPai[0][0]>=48 &&px->Time<=3||
					((Line==1&&(Hands[0]<=2||Hands[1]<=2)) ||
					 (Line==2&&Hands[1]<=2)||
					 (Line==3&&Hands[0]<=2)) ))
				{
					for(i=0;px->DuiPai[i][0]>=0;i++) if( px->DuiPai[i][0]/4 > nNumToCompare && px->DuiPai[i][0]>47 )
					{
						arrToTable[nInsertCards++] = px->DuiPai[i][0];  
						arrToTable[nInsertCards++] = -1;   
						break;
					}
				}
			}
			break;

		case 4://����   ���ݴ���ǿ�Ʋ�������  ����һ����·   ����ing************************************************************
			if( nNumToCompare<12) // ���ϵ���С�ڶ�2
			{
				bool key=0;
				for(i=0;px->DuiPai[i][0]>=0;i++) if(px->DuiPai[i][0]/4 > nNumToCompare &&
					px->DuiPai[i][0]/4 - nNumToCompare <=4)
				{
					arrToTable[nInsertCards++] = px->DuiPai[i][0]; 
					arrToTable[nInsertCards++] = px->DuiPai[i][1]; 
					arrToTable[nInsertCards++] = -1; 
					key=1;
					break;
				}
				if(!key && px->DuiPai[0][0]<0&&(px->SanZhang[0][0]>=44&&px->Time<=3||
				  (
				    ((Line==1&&(Hands[0]<=2||Hands[1]<=2)) ||
					 (Line==2&&Hands[1]<=2)||
					 (Line==3&&Hands[0]<=2)) )  ))
				{
					for(i=0;px->SanZhang[i][0]>=0;i++) if( px->SanZhang[i][0]/4 > nNumToCompare && px->SanZhang[i][0]>=44 )
					{
						arrToTable[nInsertCards++] = px->SanZhang[i][0]; 
						arrToTable[nInsertCards++] = px->SanZhang[i][1]; 
						arrToTable[nInsertCards++] = -1;   
						key=1;
						break;
					} //RECORD  (  ����****************************************************************************************************8
				}
			}
			break;

		case 5://���Ų���
			if(px->SanZhang[0][0]>=0)
			{
				int Test[3]={55,-1,-1},key=0;   //key�鿴�Ƿ��ܳ���
				for(i=0;px->SanZhang[i][0]>=0;i++)
				{                                      //���� ������������Ѱ����С�ɴ���
					if(px->SanZhang[i][0]/4 > nNumToCompare && px->SanZhang[i][0]<Test[0])
					{
						Test[0] = px->SanZhang[i][0];
						Test[1] = px->SanZhang[i][1];
						Test[2] = px->SanZhang[i][2];
						px->SanZhang[i][0]=55;
						key=1;
					}

				}
				if(key)
				{
					arrToTable[nInsertCards++] = Test[0];
					arrToTable[nInsertCards++] = Test[1];
					arrToTable[nInsertCards++] = Test[2];
					arrToTable[nInsertCards++] = -1;
				}
			}
			break;

		case 6: //������ ������
			if(px->SanZhang[0][0]>=0 && (px->DuiPai[0][0]>=0 && px->DanPai[0]>=0))
			{
				int Test[3]={55,-1,-1},key=0;
				for(i=0;px->SanZhang[i][0]>=0;i++)
				{                                     
					Test[0]=55;
					if(px->SanZhang[i][0]/4 > nNumToCompare && px->SanZhang[i][0]<Test[0])
					{
						Test[0] = px->SanZhang[i][0];
						Test[1] = px->SanZhang[i][1];
						Test[2] = px->SanZhang[i][2];
						px->SanZhang[i][0]=55;
						key=1;
					}

				}
				if(key)
				{     
					arrToTable[nInsertCards++] = Test[0];
					arrToTable[nInsertCards++] = Test[1];
					arrToTable[nInsertCards++] = Test[2];
					if(px->DuiPai[0][0]>=0 && nToTableCardsNum==5)
					{
						arrToTable[nInsertCards++]=px->DuiPai[0][0];
						arrToTable[nInsertCards++]=px->DuiPai[0][1];
					}
					else if(px->DanPai[0]>=0 && nToTableCardsNum==4) arrToTable[nInsertCards++]=px->DanPai[0];
					arrToTable[nInsertCards++] = -1;
				}
			}

			break;

		case 7:
			if(px->DanShun[0][0]>0)//������˳����
			{
				int key1=0,key2=0,sign=0,cmp=55,Num=1;
				//nToTableCardsNum = count(arrOnTable);
				for(i=0;px->DanShun[i][0]>0;i++)
				{                                 //������� ��ֵС������
					if(px->DanShun[i][0]==nToTableCardsNum && 
						px->DanShun[i][1]/4 > nNumToCompare && px->DanShun[i][1]/4 < cmp/4)
					{
						sign=i;
						cmp=px->DanShun[i][1];
						key1=1;
					}
				}
				if(!key1)
				{                  //���û�ҵ�������ȵ�
					for(i=0;px->DanShun[i][0]>0;i++)
					{                                 //������� ��ֵС������
						if(px->DanShun[i][0] > nToTableCardsNum )   //���ǲ���
							for(j=1; j<= px->DanShun[i][0]-nToTableCardsNum + 1 ;j++)
							{   //1~5 = 2~6 = 6-5+1
								if(px->DanShun[i][j]/4 > nNumToCompare && 
									px->DanShun[i][j]/4 < cmp/4)
								{
									sign=i;
									Num=j;
									cmp=px->DanShun[i][j];
									key2=1;
									break;
								}
							}
					}
				}
				if(key1){
					for(i=1;i<=nToTableCardsNum;i++)
						arrToTable[nInsertCards++] = px->DanShun[sign][i];
					arrToTable[nInsertCards++]=-1;
				}
				else if(key2){          // 5 = 2~6 < 2+5
					for(i=Num;i< nToTableCardsNum + Num ;i++)
						arrToTable[nInsertCards++] = px->DanShun[sign][i];
					arrToTable[nInsertCards++]=-1;
				}
			}
			break;

		case 8:
			if(px->ShS[0][0]>0)//����˫˳����
			{
				int key1=0,key2=0,sign=0,cmp=55,Num=1;
				nToTableCardsNum = count(arrOnTable);
				for(i=0;px->ShS[i][0]>0;i++)
				{
					if(px->ShS[i][0]*2==nToTableCardsNum &&
						px->ShS[i][1]/4 > nNumToCompare && px->ShS[i][1]/4 < cmp/4)
					{
						sign=i;
						cmp=px->ShS[i][1];
						key1=1;
					}
				}
				if(!key1)
				{
					for(i=0;px->ShS[i][0]>0;i++)
					{  //1~6 = 3~8  = 8-6 + 1 = 5~10 = 10-6 +1
						if(px->ShS[i][0]*2 > nToTableCardsNum)
							for(j=1;j<= px->ShS[i][0]*2-nToTableCardsNum +1;j+=2)
							{
								if(px->ShS[i][j]/4 > nNumToCompare && 
									px->ShS[i][j]/4 < cmp/4)
								{
									sign=i;
									Num=j;
									cmp=px->ShS[i][j];
									key2=1;
									break;
								}
							}
					}
				}
				if(key1){
					for(i=1;i<=nToTableCardsNum;i++)
						arrToTable[nInsertCards++]=px->ShS[sign][i];
					arrToTable[nInsertCards++]=-1;
				}
				else if(key2){
					for(i=Num;i<nToTableCardsNum+Num;i++)
						arrToTable[nInsertCards++]=px->ShS[sign][i];
					arrToTable[nInsertCards++]=-1;
				}	
			}
			break;

		case 9:
			if(px->SaS[0][0]>0)//������˳����
			{
				int key1=0,key2=0,sign=0,cmp=55,Num=1;
				nToTableCardsNum = count(arrOnTable);
				for(i=0;px->SaS[i][0]>0;i++)
				{
					if(px->SaS[i][0]*3==nToTableCardsNum &&
						px->SaS[i][1]/4 > nNumToCompare && px->SaS[i][1]/4 < cmp/4)
					{
						sign=i;
						cmp=px->SaS[i][1];
						key1=1;
					}
				}
				if(!key1)
				{
					for(i=0;px->SaS[i][0]>0;i++)
					{  // 1~6=4~9=9-6+1=7~12=12-6+1 
						if(px->SaS[i][0]*3 > nToTableCardsNum)
							for(j=1;j<=px->SaS[i][0]*3-nToTableCardsNum+1;j+=3)
							{
								if(px->SaS[i][j]/4 > nNumToCompare &&
									px->SaS[i][j]/4 < cmp/4)
								{
									sign=i;
									Num=j;
									key2=1;
									cmp=px->SaS[i][j];
									break;
								}
							 }
					 }
				 }
				if(key1){
					for(i=1;i<=nToTableCardsNum;i++)
						arrToTable[nInsertCards++]=px->SaS[sign][i];
					arrToTable[nInsertCards++]=-1;
				}
				else if(key2){
					for(i=Num;i<nToTableCardsNum+Num;i++)
						arrToTable[nInsertCards++]=px->SaS[sign][i];
					arrToTable[nInsertCards++]=-1;
				}	

			}
			break;

		case 10:
			if(px->SaS[0][0]>0&&(px->DanPai[0]>=0||px->DuiPai[0][0]>=0))//����һ������
			{
				int key1=0,key2=0,sign=0,cmp=55,Num=1;
				nToTableCardsNum = nToTableCardsNum / 4 * 3;  //��¼���ŵ�����count(arrOnTable)
				if(px->DanPai[ nToTableCardsNum/3 -1 ]>=0 ||
					px->DuiPai[ nToTableCardsNum/3 -1][0]>=0)  //ȷ���ܹ����������������
				{
					for(i=0;px->SaS[i][0]>0;i++)
					{
						if(px->SaS[i][0]*3==nToTableCardsNum &&
							px->SaS[i][1]/4 > nNumToCompare && px->SaS[i][1]/4 < cmp/4)
						{
							sign=i;
							cmp=px->SaS[i][1];
							key1=1;
						}
					}
					if(!key1)
					{
						for(i=0;px->SaS[i][0]>0;i++)
						{  // 1~6=4~9=9-6+1=7~12=12-6+1 
							if(px->SaS[i][0]*3 > nToTableCardsNum)
								for(j=1;j<=px->SaS[i][0]*3-nToTableCardsNum+1;j+=3)
								{
									if(px->SaS[i][j]/4 > nNumToCompare &&
										px->SaS[i][j]/4 < cmp/4)
									{
										sign=i;
										Num=j;
										key2=1;
										cmp=px->SaS[i][j];
										break;
									}
								}
						}
					}
					if(key1){
						for(i=1;i<=nToTableCardsNum;i++)
							arrToTable[nInsertCards++]=px->SaS[sign][i];
					}
					else if(key2){
						for(i=Num;i<nToTableCardsNum+Num;i++)
							arrToTable[nInsertCards++]=px->SaS[sign][i];
					}	
					if(key1||key2)
					{  //���źͶ���˭�����������ȱ�������ͬ������
						int count1=0,count2=0;
						for(i=0;px->DanPai[i]>=0;i++) count1++;
						for(i=0;px->DuiPai[i][0]>=0;i++) count2++;
						if(count2>=count1){
							for(i=0;i<nToTableCardsNum/3;i++)
							{
								arrToTable[nInsertCards++]=px->DuiPai[i][0];
								arrToTable[nInsertCards++]=px->DuiPai[i][1];
							}
						}
						else {
							for(i=0;i<nToTableCardsNum/3;i++)
								arrToTable[nInsertCards++]=px->DanPai[i];
						}
						arrToTable[nInsertCards++]=-1;
					}
				}
			}

			break;

		case 11://�Ĵ���
			if(px->ZhaDan[0][0]>=0&&(px->DanPai[1]>=0||px->DuiPai[1][0]>=0))
			{
				int sign=0,key=0,cmp=55;
				for(i=0;px->ZhaDan[i][0]>=0;i++)
				{
					if(px->ZhaDan[i][0]/4 > nNumToCompare && px->ZhaDan[i][0]<cmp)
					{
						key=1;
						cmp=px->ZhaDan[i][0];
						sign=i;
					}
				}
				if(key)
				{
					for(i=0;i<4;i++) 
						arrToTable[nInsertCards++]=px->ZhaDan[sign][i];
					int count1=0,count2=0;
						for(i=0;px->DanPai[i]>=0;i++) count1++;
						for(i=0;px->DuiPai[i][0]>=0;i++) count2++;
						if(count2>=count1){
							for(i=0;i<nToTableCardsNum/3;i++)
							{
								arrToTable[nInsertCards++]=px->DuiPai[i][0];
								arrToTable[nInsertCards++]=px->DuiPai[i][1];
							}
						}
						else {
							for(i=0;i<nToTableCardsNum/3;i++)
								arrToTable[nInsertCards++]=px->DanPai[i];
						}
						arrToTable[nInsertCards++]=-1;
				}

			}
			break;
		case 12:  //ս���Է���
			break;
		}
		arrToTable[nInsertCards++] = -1;
		arrToTable[nInsertCards] = -2;
}