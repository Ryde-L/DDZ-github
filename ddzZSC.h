

/*�����������Ͳ���*/
#include "ddzv100.h"

/*����Ч���Ƶ���ǰ�� */
void ReSort(int cards[])
{                        
    int i,j;
	int check[21];
	for(i=0;i<21;i++) check[i]=-2;
	for(i=0,j=0;i<21;i++) if(cards[i]>=0)
       check[j++]=cards[i];
    for(i=0;i<21;i++) cards[i]=check[i];
}


/*ÿ���ֵ��Լ�����ʱ��ʼ�������࣬���¼���*/
void GameInit(int iOnHand[],struct PaiXing *px) 
{
	int i,j;
		for(i=0;i<21;i++) px->ShouPai[i]=-2;
	for(i=0;iOnHand[i]>=0;i++)
		px->ShouPai[i]=iOnHand[i]; //����¼��
	/***************************
	 ����Ϊ���и������͵ĳ�ʼ��
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
			if(j==0) px->DanShun[i][j]=0;     //��˳�ĳ��ȳ�ʼ��Ϊ0
			else px->DanShun[i][j]=-2;
		}
	for(i=0;i<3;i++)                          //˫˳�ĳ��ȳ�ʼ��Ϊ0
		for(j=0;j<21;j++)
		{
			if(j==0) px->ShS[i][j]=0;
			else px->ShS[i][j]=-2;
		}
	for(i=0;i<2;i++)                          //��˳�ĳ��ȳ�ʼ��Ϊ0
		for(j=0;j<19;j++)
		{
			if(j==0) px->SaS[i][j]=0;
			else px->SaS[i][j]=-2;
		}
	px->Time=0;        //ʣ������
}


/*��������
//��ѯ�Ƿ��л��*/
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
	if(flag==2){         //ȷ�ϲ�ѯ�����
		px->Rocket=1;
		px->ShouPai[JL[0]]=-2;//���λ�õ�������-2
		px->ShouPai[JL[1]]=-2;
		return 1;
	}
	else
		return 0;

}


/*��ѯը�� ��AAAA*/
bool BomB(int index,struct PaiXing *px)
{         
	int poker = px->ShouPai[index];
	//����4����ͬ��Ϊը��
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
	{                      //��¼�������� ը�����ֵĿ�ȱλ��
			j=i;
			break;
	}
	//ԭfor (i = 0; i < index; i++)
	for (i = 0; i<=index; i++) if (PP / 4 == px->ShouPai[i] / 4 && px->ShouPai[i] >= 0)
		flag++;            //�ж�����
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


/* �Ƿ��㹻���ŵ�ֵ�� TwoPieces*/
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


/*�Ƿ��㹻���ŵ�ֵ��
	@param��int PP �������px->ShouPai[index]/4;
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


/*ThirdShun��˳
	@param ��index	ShouPai��ʼ���±�
	@return ������һ�����ٸ��ƻ�0
*/
int  ThirShun(int index,struct PaiXing *px)
{                               
	int i,sign=0,time=0;
	int PP=px->ShouPai[index];
	for(i=0;i<2;i++) if(px->SaS[i][0]<0)
	{                           //��ȱλ��
			sign=i;
			break;
	}
	while(1)
	{                           //�����������Ž����ж�
		if(ThrP(PP/4 - time,px)&&PP<48){
			++time;
		}
		else 
			break;
	}
	if(time >=2)
	{                           //�����򴢴�
		int piece=0;            //��¼����
		px->SaS[sign][0]=time;     //0λ��¼���ٸ�AAA
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


/*˫˳
@return���鵽�˶������� ��AABBCCΪ6��
*/
int  TwoShun(int index,struct PaiXing *px) 
{                               //
	int i,sign=0,time=0;
	int PP=px->ShouPai[index];
	for(i=0;i<3;i++) if(px->ShS[i][0]<=0)
	{                           //��ȱλ��
			sign=i;
			break;
	}
	while(1)
	{                           //�������Ķ��Ž����ж�
		if(TwoP(PP/4 - time,px)&&px->ShouPai[index]<48){
			++time;
		}
		else 
			break;
	}
	if(time >=3)
	{                           //�����򴢴�
		int piece=0;            //��¼����
		px->ShS[sign][0]=time;     //0λ��¼���ٶ�
		while(piece<time*2)
		{ 
			int flag=0;
			for(i=0;i<=index&&flag!=2;i++)
			{                   //ÿ�γ鵽һ�Խ���ѭ��
				if(PP/4 - time + 1 + piece/2 == px->ShouPai[i]/4)
				{
					px->ShS[sign][piece+1]=px->ShouPai[i];
					px->ShouPai[i]=-2;
					flag++;
					piece++;
				}
			}
		}
		return piece;  //��������
	}
	else 
		return 0;
}


/* ��ѯ��˳*/
int  DS(int index,struct PaiXing *px)
{                              
	int i,sign=0,time=0,flag=1;
	int PP=px->ShouPai[index];
	for(i=0;i<4;i++) if(px->DanShun[i][0]<5)
	{                           //��ȱλ��
			sign=i;
			break;
	}
	
	while(flag)
	{                           //�����ж��Ƿ����5 ,46,40,39,32,29
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


/*�����ж�
����
*/
bool SZ(int index,struct PaiXing *px)
{                                
	int i,sign=0,flag=2;
	int PP=px->ShouPai[index];
	for(i=0;i<6;i++) if(px->SanZhang[i][0]<0)
	{                           //��ȱλ��
			sign=i;
			break;
	}
	if(ThrP(PP/4,px))
	{
		for(i=0;i<=index&&flag>=0;i++) if(PP/4 == px->ShouPai[i]/4)
		{                      //����
			px->SanZhang[sign][flag--]=px->ShouPai[i];
			px->ShouPai[i]=-2;
		}
		return 1;
	}
	else
		return 0;
}


/*���Է���
������˳������ѯ���ϵ�����
���>ը��>��˳>˫˳>��˳>����>����>����
*/
void Query(struct PaiXing *px)  
{                               
	bool key=1;
	int i,piece=0;
	ReSort(px->ShouPai);
	pokersort(px->ShouPai);
	for(i=0;i<21;i++) if(px->ShouPai[i]>=0&&px->ShouPai[i]>px->ShouPai[piece])
		piece=i;  //��¼��ǰ��ѯ�����ֵ�ĵ���λ�ã���������
	
	while(1)
	{
		key=0;   //�����·��������ж�
	    ReSort(px->ShouPai);
		pokersort(px->ShouPai);
		for(i=0;i<21;i++)
			if(px->ShouPai[i]>=0)
			{ 
				key=1;
				break;
			}
		if(key==0) break;  //�ж������Ƿ����
		
		
	///////////////////////////////////////////////////////////////
        int pp;
		if(Rocket(px)){
                                   //�ж����
			         piece-=2;
			         continue;  
		}
		else if(BomB(piece,px)){
                                   //�ж�ը��
			piece-=4;             
			continue;              
		}
	    if(pp=ThirShun(piece,px)){      
			                       //��˳
			piece-=pp;
			continue;
		}
        if(pp=TwoShun(piece,px)){      
			                       //˫˳
			piece-=pp;
			continue;
		}
        if(pp=DS(piece,px)){       
			                       //��˳
			piece-=pp;
			continue;
		}
		if(SZ(piece,px)){          //����
			piece-=3;    
			continue;
		}
		else if(TwoP(px->ShouPai[piece]/4,px))
		{                          //����
			int sign,flag=0;      
			int PP=px->ShouPai[piece];
			for(i=0;i<10;i++) if(px->DuiPai[i][0]<0)
			{                           //��ȱλ��
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
		{                              //���Ŵ���
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
	pokersort(px->DanPai);  //����������;

	int j,temp1,temp2;      //����������
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


/*��������*/
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
�����������Ƽ���
	@param��int cards[] ������

	=========================
	���ʣ�ֻ����iOnTable[0]��iOnTable[1]��������
*/
void Calcu(int cards[],struct ddz *dp)
{                         
	int k=0,key=2;
	for(int i=0;key;i++)//�δ�ѭ��
	{
		if(cards[i]==-1)//��iOnTable����
		{
			key--;
			k++;			
		}
		else 
			dp->HandCard[k]--;//�������k��������һ
	}
	//cout << dp->HandCard[0] << " ,"<<dp->HandCard[1] << endl;
}


/*  ȷ����������Լ���λ��
	1.��ũ AI ũ�� 2.���� AI �ء� 3.���� AI �ѡ�
*/
void Lineup(struct ddz *dp)
{                           
    //�ҵ�λ�� cDir ������λ�� cLandlord
	int i;

	if(dp->cDir==dp->cLandlord)     
		dp->WarLine=1; //����
	else if(dp->cDir<dp->cLandlord||(dp->cDir=='C'&&dp->cLandlord=='A'))         
	    dp->WarLine=2; //�������Լ�����AB BC CA 
	else if(dp->cDir>dp->cLandlord||(dp->cDir=='A'&&dp->cLandlord=='C'))            
		dp->WarLine=3; //�������Լ�����CA AB BC
}
