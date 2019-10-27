#include <reg52.h>
#include <stdio.h>
#include <intrins.h>
#include <lcd.h>
#include <math.h>
sbit ad9850_w_clk    =P2^2;  //P2.2�ڽ�ad9850��w_clk��/PIN7
sbit ad9850_fq_up    =P2^1;  //P2.1�ڽ�ad9850��fq_up��/PIN8
sbit ad9850_rest     =P2^0;  //P2.0�ڽ�ad9850��rest��/PIN12
sbit ad9850_bit_data =P1^7;  //P1.7�ڽ�ad9850��D7��/PIN25
sbit Key1 = P3^1;
sbit Key2 = P3^0;
uchar code table[]="Frequency:MHz";
uchar err[]="ERROR";
int FValue = 0,temp = 0;
int num = 0;
void checkK1();
void checkK2();
//P1Ϊ8λ���ݿ�
//***************************************************//
//              ad9850��λ(����ģʽ)                 //
//---------------------------------------------------//
void init(){
	num = 0;
}
void ad9850_reset()
{
ad9850_w_clk=0;
ad9850_fq_up=0;
//rest�ź�
ad9850_rest=0;
ad9850_rest=1;
ad9850_rest=0;
}
//***************************************************//
//              ad9850��λ(����ģʽ)                 //
//---------------------------------------------------//
void ad9850_reset_serial()
{
ad9850_reset();
//w_clk�ź�
ad9850_w_clk=0;
ad9850_w_clk=1;
ad9850_w_clk=0;
//fq_up�ź�
ad9850_fq_up=0;
ad9850_fq_up=1;
ad9850_fq_up=0;
}
//***************************************************//
//          ��ad9850��д����������(����)             //
//---------------------------------------------------//
void ad9850_wr_parrel(unsigned char w0,double frequence)
{
unsigned char w;
long int y;
double x;
//����Ƶ�ʵ�HEXֵ
x=4294967295/125;//�ʺ�125M����
//���ʱ��Ƶ�ʲ�Ϊ180MHZ���޸ĸô���Ƶ��ֵ����λMHz ������
frequence=frequence/1000000;
frequence=frequence*x;
y=frequence;
//дw0����
w=w0;   
P1=w;      //w0
ad9850_w_clk=1;
ad9850_w_clk=0;
//дw1����
w=(y>>24);
P1=w;      //w1
ad9850_w_clk=1;
ad9850_w_clk=0;
//дw2����
w=(y>>16);
P1=w;      //w2
ad9850_w_clk=1;
ad9850_w_clk=0;
//дw3����
w=(y>>8);
P1=w;      //w3
ad9850_w_clk=1;
ad9850_w_clk=0;
//дw4����
w=(y>>=0);
P1=w;      //w4
ad9850_w_clk=1;
ad9850_w_clk=0;
//����ʼ��
ad9850_fq_up=1;
ad9850_fq_up=0;
}
//***************************************************//
//          ��ad9850��д����������(����)             //
//---------------------------------------------------//
void ad9850_wr_serial(unsigned char w0,double frequence)
{
unsigned char i,w;
long int y;
double x;
//����Ƶ�ʵ�HEXֵ
x=4294967295/125;//�ʺ�125M����
//���ʱ��Ƶ�ʲ�Ϊ180MHZ���޸ĸô���Ƶ��ֵ����λMHz  ������
frequence=frequence/1000000;
frequence=frequence*x;
y=frequence;
//дw4����
w=(y>>=0);
for(i=0;i<8;i++)
{
ad9850_bit_data=(w>>i)&0x01;
ad9850_w_clk=1;
ad9850_w_clk=0;
}
//дw3����
w=(y>>8);
for(i=0;i<8;i++)
{
ad9850_bit_data=(w>>i)&0x01;
ad9850_w_clk=1;
ad9850_w_clk=0;
}
//дw2����
w=(y>>16);
for(i=0;i<8;i++)
{
ad9850_bit_data=(w>>i)&0x01;
ad9850_w_clk=1;
ad9850_w_clk=0;
}
//дw1����
w=(y>>24);
for(i=0;i<8;i++)
{
ad9850_bit_data=(w>>i)&0x01;
ad9850_w_clk=1;
ad9850_w_clk=0;
}
//дw0����
w=w0;   
for(i=0;i<8;i++)
{
ad9850_bit_data=(w>>i)&0x01;
ad9850_w_clk=1;
ad9850_w_clk=0;
}
//����ʼ��
ad9850_fq_up=1;
ad9850_fq_up=0;
}

void display()
{
	uint i;
	lcdWriteCom(0x80+0x00);
	for(i=0;i<10;i++)
	{
	  lcdWriteDate(table[i]);
	}
}
void display2()
{
	uint i;
	lcdWriteCom(0xc0+0x08);
	for(i=10;i<13;i++)
	{
	  lcdWriteDate(table[i]);
	}
}
void initFreq(){
	FValue = num;
}
void display3()
{
	initFreq();
	lcdWriteCom(0xc0+0x04);
	    temp = FValue/1000;
	    FValue = FValue%1000;
	lcdWriteDate(temp+0x30);
	lcdWriteCom(0xc0+0x05);
	    temp = FValue/100;
	    FValue = FValue%100;
	lcdWriteDate(temp+0x30);
		lcdWriteCom(0xc0+0x06);
		temp = FValue/10;
		FValue = FValue%10;
	lcdWriteDate(temp+0x30);
		lcdWriteCom(0xc0+0x07);
	lcdWriteDate(FValue+0x30);		
}
void display4(){
	uint i;
	lcdInit();
	lcdWriteCom(0xc0+0x04);
	for(i=0;i<5;i++)
	{
	  lcdWriteDate(err[i]);
	}
}
void error(){
	for(;;){
		ad9850_reset_serial();
		ad9850_wr_serial(0x00,num);
	  display4();
	}
}
void delay(uint c)
{
	uint a,b;
	for(;c>0;c--)
		for(b=199;b>0;b--)
			for(a=1;a>0;a--);
}
void initScan(){
		ad9850_reset_serial();
		ad9850_wr_serial(0x00,num);
	  display();
		display3();
		display2();
}
void checkK1(){
	if(Key1==0)
	{
		//delay(10);
		num += 100;
	}
	else if(Key1 == 0){
	  num = 0;
		checkK1();
	};
}
void checkK2(){
	if(Key2==0)
	{
		//delay(10);
		num -= 100;
	}
}
void KeyScan()
{
	initScan();
	if(num >= 0 && num <= 9900){
		checkK1();
	  checkK2();
	}
	else {
		error();
	}
}
void main()
{
	init();
	lcdInit();
	for(;;){
		KeyScan();
	}
}





