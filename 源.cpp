/* MOV M1 立即寻址 1 M1=32
MOV M2 立即寻址 1     M2=33
MOV X1 M1                  X1=64
MOV X2 M2                  X2=65
MOV X3 M3                  X3=66
MOV X4 0                     X4=67
ADD 间接寻址 X1 间接寻址 X3           
ADD 间接寻址 X2 间接寻址 X3            
INC X1
INC X2
INC X3
INC X4
JZ 立即寻址 X4  10 跳回
OUT*/
#include<cstdio>
#include<iostream>
#include<fstream>
#include<cmath>
#include<cstring>
#define MAX_N 512//存储器行数
#define MAX_M 32//指令长度为一个字长
using namespace std;
static char Memory[MAX_N][MAX_M];//存储器
static char Register[16][MAX_M];//通用寄存器
static char MAR[4];//操作数地址寄存器
static char MDR[2][MAX_M];//操作数寄存器
static char ACC[MAX_M];
static char Rx[MAX_M];//变址寄存器
static char Rd[MAX_M];//基址寄存器
static char IR[MAX_M];//指令寄存器
static char SP[MAX_M];//栈顶地址寄存器
static char CL[MAX_M];//移位寄存器
static int PC = 0;
static int PSW;
static int ZF;
static int address[2];
//指令格式：6 4 9 4 9
int Transform_1(char ch[], int length) //二进制串转十进制数值
{
	int decimal_number = 0;
	int i;
	int j = 0;
	for (i = length - 1; i >= 0; i--)
	{
		if (ch[i] == '1')
		{
			decimal_number = decimal_number + (int)pow(2, j);
		}
		j++;
	}
	return decimal_number;
}

void Transform_2(int number, char ch[], int length)//十进制数转化为二进制串
{
	length = length - 1;
	while (number != 0)
	{
		int remainder = number % 2;
		number = number / 2;
		if (remainder == 1)
		{
			ch[length] = '1';
		}
		length--;
	}
}

void Addressing(int distinguish)
{
	int addressing_mode = Transform_1(MAR, 4);
	//printf("%d", addressing_mode);
	int i;
	char temp[9];
	if (distinguish == 1)
	{
		for (i = 0; i < 9; i++)
		{
			temp[i] = Memory[PC][10 + i];
		}
	}
	else
	{
		for (i = 0; i < 9; i++)
		{
			temp[i] = Memory[PC][23 + i];
		}
	}
	int value, value_1, value_2;
	switch (addressing_mode)
	{
	case 1://立即寻址
		for (i = 0; i < 23; i++)
			MDR[distinguish - 1][i] = '0';
		for (i = 23; i < 32; i++)
		{
			MDR[distinguish - 1][i] = temp[i - 23];
		}
		break;
	case 2://直接寻址
		value = Transform_1(temp, 9);
		address[distinguish - 1] = value;
		for (i = 0; i < 32; i++)
		{
			MDR[distinguish - 1][i] = Memory[value][i];
		}
		break;
	case 3://间接寻址
		value_1 = Transform_1(temp, 9);
		for (int j = 0; j < 32; j++)
			cout << Memory[value_1][j];
		value_2 = Transform_1(Memory[value_1], 32);
		address[distinguish - 1] = value_2;
		for (i = 0; i < 32; i++)
		{
			MDR[distinguish - 1][i] = Memory[value_2][i];
		}
		break;
	case 4://寄存器寻址
		value = Transform_1(temp, 9);
		address[distinguish - 1] = value;
		for (i = 0; i < 32; i++)
		{
			MDR[distinguish - 1][i] = Register[value][i];
		}
		break;
	case 5://寄存器间接寻址
		value_1 = Transform_1(temp, 9);
		value_2 = Transform_1(Register[value_1], 32);
		address[distinguish - 1] = value_2;
		for (i = 0; i < 32; i++)
		{
			MDR[distinguish - 1][i] = Memory[value_2][i];
		}
		break;
	case 6://隐含寻址
		break;
	case 7://基址寻址
		value_1 = Transform_1(Rd, 32);
		value_2 = Transform_1(temp, 9);
		address[distinguish - 1] = value_2;
		value_2 = value_1 + value_2;
		for (i = 0; i < 32; i++)
		{
			MDR[distinguish - 1][i] = Memory[value_2][i];
		}
		break;
	case 8://变址寻址
		value_1 = Transform_1(Rx, 32);
		value_2 = Transform_1(temp, 9);
		address[distinguish - 1] = value_2;
		value_2 = value_1 + value_2;
		for (i = 0; i < 32; i++)
		{
			MDR[distinguish - 1][i] = Memory[value_2][i];
		}
		break;
	case 9://相对寻址
		value_1 = PC;
		value_2 = Transform_1(temp, 9);
		address[distinguish - 1] = value_2;
		value_2 = value_1 + value_2;
		for (i = 0; i < 32; i++)
		{
			MDR[distinguish - 1][i] = Memory[value_2][i];
		}
		break;
	}
}

void ADD()
{
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	int addition_number_1 = Transform_1(MDR[0], 32);
	int addition_number_2 = Transform_1(MDR[1], 32);
	addition_number_1 = addition_number_1 + addition_number_2;
	Transform_2(addition_number_1, Memory[address[0]], 32);
}
void SUB()
{
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	int minuend = Transform_1(MDR[0], 32);
	int reduction = Transform_1(MDR[1], 32);
	minuend = minuend - reduction;
	Transform_2(minuend, Memory[address[0]], 32);
}
void MOV()
{
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	for (i = 0; i < 32; i++)
	{
		MDR[0][i] = MDR[1][i];
		Memory[address[0]][i] = MDR[1][i];
		cout << Memory[address[0]][i];
	}
	cout << endl;
}
void XCHG()
{
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	int value_1 = Transform_1(MDR[0], 32);
	int value_2 = Transform_1(MDR[1], 32);
	int value_3 = value_1;
	value_1 = value_2;
	value_2 = value_3;
	Transform_2(value_1, Memory[address[0]], 32);
	Transform_2(value_2, Memory[address[1]], 32);
}
void POP()
{
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	int value = Transform_1(SP, 32);
	for (i = 0; i < 32; i++)
	{
		Memory[address[0]][i] = Memory[value][i];
	}
	value = value + 2;
	Transform_2(value, SP, 32);
}
void PUSH()
{
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	int value = Transform_1(SP, 32);
	value = value - 2;
	for (i = 0; i < 32; i++)
	{
		Memory[value][i] = Memory[address[0]][i];
	}
	Transform_2(value, SP, 32);
}
void INC()
{
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	int addition_number_1 = Transform_1(MDR[0], 32);
	addition_number_1 = addition_number_1 + 1;
	Transform_2(addition_number_1, Memory[address[0]], 32);
}
void DEC()
{
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	int minuend = Transform_1(MDR[0], 32);
	minuend = minuend - 1;
	Transform_2(minuend, Memory[address[0]], 32);
}
void MUL()
{
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	int multiplication_number_1 = Transform_1(MDR[0], 32);
	int multiplication_number_2 = Transform_1(MDR[1], 32);
	multiplication_number_2 = multiplication_number_1 * multiplication_number_2;
	Transform_2(multiplication_number_2, Memory[address[1]], 32);
}
void DIV()
{
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	int divisor = Transform_1(MDR[0], 32);
	int dividend = Transform_1(MDR[1], 32);
	dividend = dividend / divisor;
	Transform_2(dividend, Memory[address[1]], 32);
}
void CMP()
{
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	int compared_number_1 = Transform_1(MDR[0], 32);
	int compared_number_2 = Transform_1(MDR[1], 32);
	PSW = compared_number_1 - compared_number_2;
}
void TEST()
{
	int i;
	char test[MAX_M] = {};
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	for (i = 0; i < MAX_M; i++)
	{
		if (MDR[0][i] == 1 && MDR[1][i] == 1)
			test[i] = 1;
		else
			test[i] = 0;
	}
	for (i = 0; i < MAX_M; i++)
	{
		if (test[i] == 1)
		{
			ZF = 0;
			break;
		}
	}
}
void OR()
{
	for (int i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (int i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	for (int i = 0; i < 32; i++)
	{
		MDR[0][i] = MDR[0][i] | MDR[0][i];
		Memory[address[0]][i] = MDR[0][i];
	}
}
void AND()
{
	for (int i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (int i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	for (int i = 0; i < 32; i++)
	{
		MDR[0][i] = MDR[0][i] & MDR[1][i];
		Memory[address[0]][i] = MDR[0][i];
	}
}
void XOR()
{
	for (int i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (int i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	for (int i = 0; i < 32; i++)
	{
		MDR[0][i] = MDR[0][i] ^ MDR[1][i];
		Memory[address[0]][i] = MDR[0][i];
	}
}
void NOT()
{
	for (int i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (int i = 0; i < 32; i++)
	{
		MDR[0][i] = !MDR[0][i];
		Memory[address[0]][i] = MDR[0][i];
	}
}
void SAL()//SAL 1000 111111111 11111(最大32 取5位） 
{
	for (int i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	char temp[5];
	for (int i = 19; i < 24; i++)
	{
		temp[i - 19] = Memory[PC][i];
	}
	int cl = Transform_1(temp, 5);
	for (int i = 0; i < 32; i++)
	{
		if (i < 32 - cl)
		{
			MDR[0][i] = MDR[0][i + cl];
		}
		else
		{
			MDR[0][i] = 0;
		}
		Memory[address[0]][i] = MDR[0][i];
	}
}
void SHL()//SHL 1000 111111111 11111
{
	for (int i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	char temp[5];
	for (int i = 19; i < 24; i++)
	{
		temp[i - 19] = Memory[PC][i];
	}
	int cl = Transform_1(temp, 5);
	for (int i = 0; i < 32; i++)
	{
		if (i < 32 - cl)
		{
			MDR[0][i] = MDR[0][i + cl];
		}
		else
		{
			MDR[0][i] = 0;
		}
		Memory[address[0]][i] = MDR[0][i];
	}

}
void SHR() {
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	int number_1 = Transform_1(MDR[0], 32);
	int number_2 = Transform_1(MDR[1], 32);
	number_1 = number_1 << number_2;
	Transform_2(number_1, Memory[address[0]], 32);
}
void SAR() {
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	int number_1 = Transform_1(MDR[0], 32);
	int number_2 = Transform_1(MDR[1], 32);
	number_1 = number_1 << number_2;
	Transform_2(number_1, Memory[address[0]], 32);
}
void JZ() {
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	int number_1 = Transform_1(MDR[0], 32);
	int number_2 = Transform_1(MDR[1], 32);
	if (number_1 != number_2)
		PC = PC - 6;
}
void OUT() {
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	int number_2 = Transform_1(MDR[1], 32);
	for (i = 0; i < number_2; i++) {
		int j = 0;
		for (j = 0; j < 4; j++)
		{
			MAR[j] = Memory[PC][6 + j];
		}
		Addressing(1);
		int number_1 = Transform_1(MDR[address[0]], 32);
		printf("%d\n", number_1);
		address[0]++;
	}
}
void IN() {
	int i;
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][6 + i];
	}
	Addressing(1);
	for (i = 0; i < 4; i++)
	{
		MAR[i] = Memory[PC][19 + i];
	}
	Addressing(2);
	Transform_2(MAR[0], Memory[address[1]], 32);
}
int main() {
	PC = 0;
	ifstream file("data.txt");
	int datalen = 0, i = 0, j = 0;
	while (!file.eof()) {
		file >> Memory[i][datalen++];
		if (datalen == MAX_M)
		{
			i++;
			datalen = 0;
		}
	}
	
		
	for (i = 14; i < 512; i++)
	{
		for (j = 0; j < 32; j++)
		{
			Memory[i][j] = '0';
		}
	}
	/*for (i = 0; i < 14; i++) {
		for (j = 0; j < 32; j++)
			cout << Memory[i][j];
		cout << endl;
	}*/
	
	while (PC <MAX_N) {

		/*for (int j = 0; j < MAX_M; j++) {
			IR[j] = Memory[PC][j];
			printf("%c", IR[j]);
		}
		printf("\n");*/
		for (i = 0; i < 2; i++)
			for (j = 0; j < 32; j++)
				MDR[i][j] = 0;
		char ch[6];
		int ir;
		for (int j = 0; j < 6; j++)
			ch[j] = IR[j];
		ir = Transform_1(ch, 6);
		switch (ir)
		{
		case 0:MOV(); break;
		case 1:XCHG(); break;
		case 2:PUSH(); break;
		case 3:POP(); break;
		case 4:ADD(); break;
		case 5:SUB(); break;
		case 6:MUL(); break;
		case 7:DIV(); break;
		case 8:INC(); break;
		case 9:DEC(); break;
		case 10:CMP(); break;
		case 11:TEST(); break;
		case 12:OR(); break;
		case 13:AND(); break;
		case 14:XOR(); break;
		case 15:NOT(); break;
		case 16:SAL(); break;
		case 17:SHL(); break;
		case 18:SAR(); break;
		case 19:SHR(); break;
		case 20:JZ(); break;
		case 21:IN(); break;
		case 22:OUT(); break;
		default:break;
		}
		if (ir != 20)
			PC++;
		if (ir == 22)
			break;
	}
}
