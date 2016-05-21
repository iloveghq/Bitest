#pragma once

#include "resource.h"		// 主符号

struct Gabor_Result     //存贮结构体
{
	Gabor_Result()
	{
		memset(Event_Array,0,sizeof(Event_Array));
		memset(Event_Loss,0,sizeof(Event_Loss));
		memset(Reflect_Type,0,sizeof(Reflect_Type));
		memset(Event_Hight,0,sizeof(Event_Hight));
		memset(Line_k,0,sizeof(Line_k));
		memset(Total_Loss,0,sizeof(Total_Loss));
		//memset(Event_Array_End,0,sizeof(Event_Array_End));
		Trail_End=10;
		Valid_Num=0;
	}
	int Reflect_Type[100];  //事件类型	//Event Code
	int Event_Array[100];  //事件位置  //Event Propagation Time
	float  Event_Loss[100];       //损耗//Event Loss
	float Event_Hight[100]; //事件点反射dB
	float Line_k[100];//区段斜率
	float  Total_Loss[100];  //总损耗、累计损耗
	float  Reflect;   //发射
	int Valid_Num;    //有效长度
	float Trail_End;//尾端位置
	char Event_type[100];// A：用户自加；M:    E:光纤末端；F:软件自行查找；O:超出测量范围的光纤末端 D:
	//int Event_Array_End[100];
};//结构体的定义和位置随意


