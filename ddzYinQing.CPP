//#include "ddzv100.h"   2014 CAAI ��������
#include "ddzZSC.h"  
#include<iostream>
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
//�����Ͳ��� - ��δ�ã�  
//˼·����
double CalCardsValue(int iPlaOnHand[])	
{
	int i;
	double dSum = 100;			//��ֵ
	for(i=0;iPlaOnHand[i]>=0;i++)
	{
		dSum=dSum-5;			//����Խ��Խ��û��һ�������Ƽ�5��
		if (i >= 1 && iPlaOnHand[i - 1] / 4 == iPlaOnHand[i] / 4)
			dSum = dSum + 2;	//��������ͬ���2��
		if (i >= 4 && iPlaOnHand[i - 4] / 4 <=7
			&&iPlaOnHand[i - 4] / 4 + 1 ==iPlaOnHand[i - 3] / 4 
			&&iPlaOnHand[i - 3] / 4 + 1 ==iPlaOnHand[i - 2] / 4
			&&iPlaOnHand[i - 2] / 4 + 1 ==iPlaOnHand[i - 1] / 4 
			&&iPlaOnHand[i - 1] / 4 + 1 ==iPlaOnHand[i] / 4)
			dSum = dSum + 6;	//2�����������Ƶ�˳��6��
	}
	return dSum; 
}

/*Game ON*/
int CalBid(struct ddz * dp)				//���Ƽ��㺯����������˽������֣��򲻽С�
{
	pokersort(dp->iOnHand);
	int i,bid=1,Flag1=0,Flag2=0;  //��ʼ��bid=1�� ��ֹ�������Ҷ����е��������Ϸ�޷�����
	int sign=dp->cDir-'A';        //��¼λ��
	     
	for(i=0;dp->iOnHand[i]>=0;i++)
	{
		if(dp->iOnHand[i]>=48&&dp->iOnHand[i]<=51) Flag1++;
		else if(dp->iOnHand[i]==52||dp->iOnHand[i]==53) Flag2++;
	}
	if(Flag1==4||Flag2==2||(Flag1>=3&&Flag2==1))
		bid=3;  //���� �� �ĸ�2 �� �������������ϵ�2 ��3��
	else if((Flag1>=3&&Flag1==0)||(Flag1==2&&Flag2==1))
		bid=2;  //�������ϵ�2 �� ����������2 ��2��
	else if((Flag1==1&&Flag2==1)||(Flag1==2&&Flag2==0))
		bid=1;  //������һ��2 �� ��2 ��1��
	for(i=0;i<3;i++)
		if(dp->iBid[i]>=3||(dp->iBid[i]==bid&&i!=sign))
			bid=0;  //��ֹ���ط�
    return bid;
}

void CalPla(struct ddz * dp)			//���Ƽ��㺯��    ����Ҫ�����ƴ����� Ҫ����iToTable[]������
{
	struct PaiXing dd, * px=&dd;
	int i,j,k=0,iOnTableW[4000],iToTableW[4000];
	
	Lineup(dp); //����ȷ��

	for(i=dp->iOTmax-1;i>=0;i--)
	{
		for(j=0;dp->iOnTable[i][j]>=0;j++,k++)
			iOnTableW[k]=dp->iOnTable[i][j];
		iOnTableW[k]=-1;
		k++;
	}
	iOnTableW[k]=-2;

	Calcu(iOnTableW,dp);    //����������������
	pokersort(dp->iOnHand);
	GameInit(dp->iOnHand,px);     //��ʼ��
	Query(px);            	//�����෽����ʼ���ͷ���
	AllHand(px);            //��������
	helpdz(dp->iOnHand,iOnTableW,iToTableW,dp->HandCard,px,dp->WarLine);
	for(i=0;iToTableW[i]>=0;i++)
		dp->iToTable[i]=iToTableW[i];
	dp->iToTable[i]=-1;
}


void GetBid(struct ddz * dp)					//����������Ϣ
{	
	if(dp->sCommandIn[4]=='W')					//���������ϢΪBID WHAT
    {
        strcpy(dp->sCommandOut,"BID _0");
		dp->sCommandOut[4]=dp->cDir;
		dp->iBid[dp->cDir-'A']=CalBid(dp);
		dp->sCommandOut[5]=dp->iBid[dp->cDir-'A']+'0';		//���ý��ƺ���
		dp->sCommandOut[6]='\0';
	}
	if(dp->sCommandIn[4]>='A'&&dp->sCommandIn[4]<='C')  //������ϢΪBID **
    {
		dp->iBid[dp->sCommandIn[4]-'A']=dp->sCommandIn[5]-'0';
        strcpy(dp->sCommandOut,"OK BID");
        //���ڴ˴������ռ�������Ϣ��������ڼ���ʹ��
    }

}


void GetPla(struct ddz * dp)					//PLAY ָ�������
{
    int a,b,i,j;
	if(dp->sCommandIn[5]=='W')					//������ϢΪPLAY WHAT��Ӧ���ó��Ƽ��㺯���������
    {
        CalPla(dp);								//���ó��Ƽ��㺯���������
        strcpy(dp->sCommandOut,"PLAY _");
		dp->sCommandOut[5]=dp->cDir;			//��������Ԥ����Ϣ׼����sCommandOut����
        iTos(dp->iToTable,dp->sCommandOut);		//Ҫ��������iToTable[]�е�����ת��Ϊ�ַ������ӵ�sCommandOut��
        TakeOff(dp->iOnHand,dp->iToTable);		//�Ѵ������iToTable[] �� ���е���iOnHand[] ���޳�
        for(i=0;dp->iToTable[i]>=0;i++)			//�Ѵ������iToTable[] ���浽 iOnTable[][]
        {
            dp->iOnTable[dp->iOTmax][i]=dp->iToTable[i];
        }
        dp->iOnTable[dp->iOTmax][i]=-1;
    }
    else										//�����յ���ϢΪ������ҳ���
    {
        
        a=-1;   b=-1;   j=0;					//�����������������ƴ��浽iOnTable������
        if(dp->sCommandIn[6]=='-')
        {
            dp->iOnTable[dp->iOTmax][0]=-1;
        }
        else
        {
            for(i=6;dp->sCommandIn[i-1]!='\0';i++)
            {
                if(dp->sCommandIn[i]!=',' && dp->sCommandIn[i]!='\0')
                {
                    if(a==-1)
                        a = dp->sCommandIn[i]-'0';
                    else
                        b = dp->sCommandIn[i]-'0';
                }
                else
                {
                    if(b==-1)
                        dp->iOnTable[dp->iOTmax][j] = a;
                    else
                        dp->iOnTable[dp->iOTmax][j] = a*10+b;

                    a=-1;   b=-1;
                    j++;
                }
            }
            dp->iOnTable[dp->iOTmax][j]=-1;
        }
       //�ظ��յ�
		strcpy(dp->sCommandOut,"OK PLAY");
    }
    //��ǰ������1
    dp->iOTmax++;
}

void AIInput(struct ddz * dp)					//����ָ��
{
	cin.getline(dp->sCommandIn,80);
}

void AICal(struct ddz * dp)						//����ָ��
{
	char cShort[4];
	for(int i=0;i<3;i++)
		cShort[i]=dp->sCommandIn[i];
	cShort[3]='\0';
	if(strcmp(cShort,"DOU")==0)					//�汾��Ϣ
		GetDou(dp);
	if(strcmp(cShort,"INF")==0)					//�־���Ϣ
		GetInf(dp);
	if(strcmp(cShort,"DEA")==0)					//������Ϣ
		GetDea(dp);
	if(strcmp(cShort,"BID")==0)					//���ƹ���
		GetBid(dp);
	if(strcmp(cShort,"LEF")==0)					//������Ϣ
		GetLef(dp);
	if(strcmp(cShort,"PLA")==0)					//���ƹ���
		GetPla(dp);
	if(strcmp(cShort,"GAM")==0)					//ʤ����Ϣ
		GetGam(dp);
}

void AIOutput(struct ddz * dp)					//���ָ��
{
	cout<<dp->sCommandOut<<endl;
}

void CalOther(struct ddz * dp)					//ָ�����
{
	dp->iVoid=0;
}

int main()										//������
{
	struct ddz d, * dp=&d;
	init(dp);
	while(dp->iStatus!=0){
		AIInput(dp);							//������Ϣ
		AICal(dp);								//����������
		AIOutput(dp);							//�����Ϣ
		CalOther(dp);							//��Ͼʱ������
	}
	return 0;
}