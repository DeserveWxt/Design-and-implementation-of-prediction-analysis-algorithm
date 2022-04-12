#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct{
	char formula[200];					//产生式
}grammarElement;

grammarElement gramOldSet[200];			//原始文法的产生式集
grammarElement gramNewSet[200];			//转换后的非递归文法的产生式集
grammarElement gramnewset[200];			//展开后的产生式集

char c = '@';
int NUM;								//原始文法产生式数量
int newNUM = 0;							//转换后的非递归文法的产生式数量
int newnum = 0;							//展开后的文法的产生式的数量
int ter_NUM;							//终结符数量
int non_ter_NUM;						//非终结符数量
int all_NUM;							//所有符号数量
char terSymbol[200];					//终结符
char non_terSymbol[200];				//非终结符
char allSymbol[400];					//所有符号
char firstSET[100][100];				//各产生式右部的FIRST集
char followSET[100][100];				//各产生式左部的FOLLOW集
int M[200][200];						//预测分析表

void GramInit(grammarElement gramOldSet[])							//文法初始化
{
	char s[4] = "end";
	NUM = 1;
	printf_s("请输入文法：（空字符请使用“$”代替，	end结束）\n");
	for (;;)
	{
		scanf_s("%s", &gramOldSet[NUM].formula, 200);
		if (!(strcmp(gramOldSet[NUM].formula, s)))
		{
			NUM--;
			break;
		}
		NUM++;
	}
	printf_s("\n");
}

void PrintGram(grammarElement gramOldSet[], int n)					//输出文法
{
	for (int i = 1; i <= n; i++)
		printf_s("%d\t%s\n", i,&gramOldSet[i].formula);
	printf_s("\n");
}

void ExtSymbol(grammarElement gramOldSet[],int NUM)					//提取所有符号
{
	ter_NUM = 0;
	non_ter_NUM = 0;
	all_NUM = 0;
	int k;
	for (int i = 1; i <= NUM; i++)
	{
		for (int j = 0; gramOldSet[i].formula[j] != '\0'; j++)
		{
			if (j == 1 || j == 2 || gramOldSet[i].formula[j] == '|')
				continue;
			if (gramOldSet[i].formula[j] >= 'A' && gramOldSet[i].formula[j] <= 'Z')
			{
				for (k = 1; k <= non_ter_NUM; k++)
					if (gramOldSet[i].formula[j] == non_terSymbol[k])
						break;
				if (k == non_ter_NUM + 1)
				{
					non_terSymbol[++non_ter_NUM] = gramOldSet[i].formula[j];
					allSymbol[++all_NUM] = gramOldSet[i].formula[j];
				}
			}
			else
			{
				for (k = 1; k <= ter_NUM; k++)
					if (gramOldSet[i].formula[j] == terSymbol[k])
						break;
				if (k == ter_NUM + 1)
				{
					terSymbol[++ter_NUM] = gramOldSet[i].formula[j];
					allSymbol[++all_NUM] = gramOldSet[i].formula[j];
				}
			}
		}
	}
}

void PrintSymbol(char c[], int len)									//输出符号
{
	for (int i = 1; i <= len; i++)
		printf_s("%c\t", c[i]);
	printf_s("\n");
}

void ConvertToNonRecursive(grammarElement gramOldSet[], int i)		//将直接左递归文法转换成非递归文法
{
	int tag;
	tag = 0;
	for (int j = 3; gramOldSet[i].formula[j] != '\0'; j++)
	{
		if (gramOldSet[i].formula[3] == gramOldSet[i].formula[0] || (gramOldSet[i].formula[j - 1] == '|' && gramOldSet[i].formula[j] == gramOldSet[i].formula[0]))
		{
			tag = 1;
			break;
		}
	}
	if (tag == 0)
		strcpy_s(gramNewSet[++newNUM].formula, strlen(gramOldSet[i].formula) + 1, gramOldSet[i].formula);
	else
	{
		c = c + 1;
		for (int k = 0; k <= non_ter_NUM; k++)
		{
			for (int m = 1; m <= non_ter_NUM; m++)
			{
				if (c == non_terSymbol[m])
				{
					c = c + 1;
					break;
				}
			}
		}
		newNUM += 1;
		gramNewSet[newNUM].formula[0] = gramOldSet[i].formula[0];
		gramNewSet[newNUM].formula[1] = gramOldSet[i].formula[1];
		gramNewSet[newNUM].formula[2] = gramOldSet[i].formula[2];
		int length = 2;
		for (int k = 3; gramOldSet[i].formula[k] != '\0'; k++)
		{
			if ((k == 3 && gramOldSet[i].formula[k] != gramOldSet[i].formula[0]) || (gramOldSet[i].formula[k] != gramOldSet[i].formula[0] && gramOldSet[i].formula[k - 1] == '|'))
			{
				while (gramOldSet[i].formula[k] != '|' && gramOldSet[i].formula[k] != '\0')
				{
					gramNewSet[newNUM].formula[++length] = gramOldSet[i].formula[k];
					k++;
				}
				k--;
				gramNewSet[newNUM].formula[++length] = c;
				gramNewSet[newNUM].formula[++length] = '|';
			}
		}
		gramNewSet[newNUM].formula[length] = '\0';
		newNUM += 1;
		gramNewSet[newNUM].formula[0] = c;
		gramNewSet[newNUM].formula[1] = '-';
		gramNewSet[newNUM].formula[2] = '>';
		int length1 = 2;
		for (int k = 3; gramOldSet[i].formula[k] != '\0'; k++)
		{
			if ((k == 3 && gramOldSet[i].formula[k] == gramOldSet[i].formula[0]) || (gramOldSet[i].formula[k] == gramOldSet[i].formula[0] && gramOldSet[i].formula[k - 1] == '|'))
			{
				k++;
				while (gramOldSet[i].formula[k] != '|' && gramOldSet[i].formula[k] != '\0')
				{
					gramNewSet[newNUM].formula[++length1] = gramOldSet[i].formula[k];
					k++;
				}
				gramNewSet[newNUM].formula[++length1] = c;
				gramNewSet[newNUM].formula[++length1] = '|';
			}
		}
		gramNewSet[newNUM].formula[++length1] = '$';
		gramNewSet[newNUM].formula[++length1] = '\0';
	}
}

void ConvertToNonRecursion(grammarElement gramOldSet[])				//将原始文法转换成非递归文法
{
	char temp[200] = { 0 };
	int length;
	int i, j, k, m, n;
	for (i = 1; i <= non_ter_NUM; i++)
	{
		for (j = 1; j <= newNUM; j++)
		{
			length = 0;
			for (k = 0; gramOldSet[i].formula[k] != '\0'; k++)
			{
				if ((k == 3 && gramOldSet[i].formula[k] == gramNewSet[j].formula[0]) || (k > 3 && gramOldSet[i].formula[k] == gramNewSet[j].formula[0] && gramOldSet[i].formula[k - 1] == '|'))
				{
					for (m = 3; gramNewSet[j].formula[m] != '\0'; m++)
					{
						if (gramNewSet[j].formula[m] != '|')
							temp[length++] = gramNewSet[j].formula[m];
						else
						{
							n = k + 1;
							while (gramOldSet[i].formula[n] != '|' && gramOldSet[i].formula[n] != '\0')
							{
								temp[length++] = gramOldSet[i].formula[n];
								n++;
							}
							temp[length++] = '|';
						}
					}
				}
				else
					temp[length++] = gramOldSet[i].formula[k];
			}
			temp[length] = '\0';
			strcpy_s(gramOldSet[i].formula, strlen(temp)+1, temp);
		}
		ConvertToNonRecursive(gramOldSet, i);
	}
}

void first(grammarElement gramNewSet[], int newNUM)					//求first集
{
	char temp[100];
	int length;
	int lengthf;
	int tag;
	for (int i = 1; i <= newNUM; i++)
	{
		length = 0;
		lengthf = 0;
		firstSET[i][0] = gramNewSet[i].formula[0];
		for (int j = 3; gramNewSet[i].formula[j] != '\0'; j++)
		{
			if (j == 3 || gramNewSet[i].formula[j - 1] == '|')
			{
				if (gramNewSet[i].formula[j] >= 'A' && gramNewSet[i].formula[j] <= 'Z')
				{
					tag = 0;
					for (int k = 1; k <= length; k++)
					{
						if (temp[k] == gramNewSet[i].formula[j])
						{
							tag = 1;
							break;
						}
					}
					if (tag == 0)
					{
						temp[++length] = gramNewSet[i].formula[j];
					}
				}
				else
				{
					tag = 0;
					for (int l = 1; l <= lengthf; l++)
					{
						if (firstSET[i][l] == gramNewSet[i].formula[j])
						{
							tag = 1;
							break;
						}
					}
					if (tag == 0)
					{
						firstSET[i][++lengthf] = gramNewSet[i].formula[j];
					}
				}
			}
		}
		while (length != 0)
		{
			for (int j = 1; j <= newNUM; j++)
			{
				if (temp[length] == gramNewSet[j].formula[0])
				{
					length--;
					for (int k = 3; gramNewSet[j].formula[k] != '\0'; k++)
					{
						if (k == 3 || gramNewSet[j].formula[k - 1] == '|')
						{
							if (gramNewSet[j].formula[k] >= 'A' && gramNewSet[j].formula[k] <= 'Z')
							{
								tag = 0;
								for (int l = 1; l <= length; l++)
								{
									if (temp[l] == gramNewSet[j].formula[k])
									{
										tag = 1;
										break;
									}
								}
								if (tag == 0)
								{
									temp[++length] = gramNewSet[j].formula[k];
								}
							}
							else
							{
								tag = 0;
								for (int m = 1; m <= lengthf; m++)
								{
									if (firstSET[i][m] == gramNewSet[j].formula[k])
									{
										tag = 1;
										break;
									}
								}
								if (tag == 0)
								{
									firstSET[i][++lengthf] = gramNewSet[j].formula[k];
								}
							}
						}
					}
					break;
				}
			}
		}
	}
}

void follow(grammarElement gramNewSet[], int newNUM)				//求follow集
{
	int length;
	int tag;
	int tag1;
	int tag0=1;
	for (int i = 1; i <= newNUM; i++)
		followSET[i][0] = gramNewSet[i].formula[0];
	for (int i = 1; i <= newNUM;i++)
	{
		tag = 0;
		for (int j = 1; j <= newNUM; j++)
		{
			for (int k = 3; gramNewSet[j].formula[k] != '\0'; k++)
			{
				if (gramNewSet[i].formula[0] == gramNewSet[j].formula[k])
				{
					tag = 1;
					break;
				}
			}
			break;
		}
		if (tag == 0)
			followSET[i][1] = '#';
	}
	for (int i = 1; i <= newNUM; i++)
	{
		length = 0;
		if (followSET[i][1] == '#')
			length = 1;
		for (int j = 1; j <= newNUM; j++)
		{
			for (int k = 3; gramNewSet[j].formula[k] != '\0'; k++)
			{
				if (gramNewSet[j].formula[k] == gramNewSet[i].formula[0])
				{
					if (gramNewSet[j].formula[k + 1] >= 'A' && gramNewSet[j].formula[k + 1] <= 'Z')
					{
						for (int l = 1; l <= newNUM; l++)
						{
							if (firstSET[l][0] == gramNewSet[j].formula[k + 1])
							{
								tag = 0;
								for (int m = 1; firstSET[l][m] != '\0'; m++)
								{
									if (firstSET[l][m] == '$')
									{
										tag = 1;
										continue;
									}
									else
									{
									tag1 = 0;
										for (int n = 1; n <= length; n++)
										{
											if (followSET[i][n] == firstSET[l][m])
											{
												tag1 = 1;
												break;
											}
										}
										if (tag1 == 0)
										{
											followSET[i][++length] = firstSET[l][m];
										}
									}
								}
								if (tag == 1)
								{
									for (int m = 1; followSET[j][m] != '\0'; m++)
									{
										tag1 = 0;
										for (int n = 1; n <= length; n++)
										{
											if (followSET[i][n] == followSET[j][m])
											{
												tag1 = 1;
												break;
											}
										}
										if (tag == 0)
										{
											followSET[i][++length] = followSET[j][m];
										}
									}
								}
								break;
							}
						}
					}
					else if (gramNewSet[j].formula[k + 1] == '|' || gramNewSet[j].formula[k + 1] == '\0')
					{
						for (int l = 1; l <= newNUM; l++)
						{
							if (followSET[l][0] == gramNewSet[j].formula[0])
							{
								for (int m = 1; followSET[l][m] != '\0'; m++)
								{
									tag1 = 0;
									for (int n = 1; n <= length; n++)
									{
										if (followSET[i][n] == followSET[l][m])
										{
											tag1 = 1;
											break;
										}
									}
									if (tag1 == 0)
									{
										followSET[i][++length] = followSET[l][m];
									}
								}
							}
						}
					}
					else
					{
						tag1 = 0;
						for (int l = 1; l <= length; l++)
						{
							if (followSET[i][l] == gramNewSet[j].formula[k + 1])
							{
								tag1 = 1;
								break;
							}
						}
						if (tag1 == 0)
						{
							followSET[i][++length] = gramNewSet[j].formula[k + 1];
						}	
					}
				}
			}
		}
	}
}

void print_f(char a[100][100], int newNUM, int tag)					//输出first集或follow集
{
	for (int i = 1; i <= newNUM; i++)
	{
		if(tag==0)
			printf_s("first(%c)={", a[i][0]);
		else
			printf_s("follow(%c)={", a[i][0]);
		for (int j = 1; a[i][j] != '\0'; j++)
		{
			printf_s("%c", a[i][j]);
			if (a[i][j + 1] != '\0')
				printf_s(",");
		}
		printf_s("}\n");
	}
}

void open(grammarElement gramNewSet[], int newNUM)					//将文法展开
{
	newnum = 0;
	int length;
	for (int i = 1; i <= newNUM; i++)
	{
		for (int j = 3; gramNewSet[i].formula[j] != '\0'; j++)
		{
			if (j == 3 || gramNewSet[i].formula[j] == '|')
			{
				newnum++;
				length = 2;
				gramnewset[newnum].formula[0] = gramNewSet[i].formula[0];
				gramnewset[newnum].formula[1] = gramNewSet[i].formula[1];
				gramnewset[newnum].formula[2] = gramNewSet[i].formula[2];
			}
			if (gramNewSet[i].formula[j] == '|')
				continue;
			gramnewset[newnum].formula[++length] = gramNewSet[i].formula[j];
		}
	}
}

void PreAnaTable()													//构造预测分析表
{
	M[0][0] = ' ';
	for (int i = 1; i <= newNUM; i++)
		M[i][0] = gramNewSet[i].formula[0];
	for (int i = 1; i <= ter_NUM; i++)
			M[0][i] = terSymbol[i];
	for (int i = 1; i <= newnum; i++)
	{
		if (gramnewset[i].formula[3] < 'A' || gramnewset[i].formula[3]>'Z')
		{
			for (int j = 1; j <= newNUM; j++)
			{
				for (int k = 1; k <= ter_NUM; k++)
				{
					if (M[j][0] == gramnewset[i].formula[0] && M[0][k] == gramnewset[i].formula[3])
					{
						if (M[j][k] == 0)
							M[j][k] = i;
						else
							M[j][k] = -1;
					}
				}
			}
		}
		else
		{
			int tag;
			for (int j = 1; j <= newNUM; j++)
			{
				if (gramnewset[i].formula[0] == M[j][0])
					tag = j;
			}
			for (int l = 1; l <= newNUM; l++)
			{
				if (firstSET[l][0] = gramnewset[i].formula[3])
				{
					for (int m = 1; firstSET[l][m] != '\0'; m++)
					{
						for (int n = 1; n <= ter_NUM; n++)
						{
							if (firstSET[l][m] == M[0][n])
							{
								if (M[tag][n] == 0)
									M[tag][n] = i;
								else
									M[tag][n] = -1;
							}
						}
					}
					break;
				}
			}
		}
	}
}

void printPreAnaTable()												//输出预测分析表
{
	for (int i = 0; i <= newNUM; i++)
	{
		for (int j = 0; j<=ter_NUM; j++)
		{
			if (M[i][j] == '$')
				M[i][j] = '#';
			if(i==0||j==0)
				printf("%c\t", M[i][j]);
			else
				printf("%d\t", M[i][j]);
		}
		printf_s("\n");
	}
}

int main()
{
	GramInit(gramOldSet);							//文法初始化

	printf_s("原始文法：\n");
	PrintGram(gramOldSet, NUM);						//输出原始文法

	ExtSymbol(gramOldSet,NUM);						//提取所有符号

	printf_s("终结符：");
	PrintSymbol(terSymbol, ter_NUM);				//输出所有终结符

	printf_s("非终结符：");
	PrintSymbol(non_terSymbol, non_ter_NUM);		//输出所有非终结符

	printf_s("所有符号：");
	PrintSymbol(allSymbol, all_NUM);				//输出所有符号
	printf_s("\n");

	ConvertToNonRecursion(gramOldSet);				//将原始文法转换成非递归文法
	
	printf_s("转换后的非递归文法：\n");
	PrintGram(gramNewSet, newNUM);					//输出转换后的非递归文法

	ExtSymbol(gramNewSet,newNUM);					//提取所有符号

	printf_s("终结符：");
	PrintSymbol(terSymbol, ter_NUM);				//输出所有终结符

	printf_s("非终结符：");
	PrintSymbol(non_terSymbol, non_ter_NUM);		//输出所有非终结符

	printf_s("所有符号：");
	PrintSymbol(allSymbol, all_NUM);				//输出所有符号
	printf_s("\n");

	first(gramNewSet, newNUM);						//求first集
	print_f(firstSET, newNUM, 0);					//输出first集
	printf_s("\n");

	
	follow(gramNewSet, newNUM);						//求follow集
	print_f(followSET, newNUM, 1);					//输出follow集
	printf_s("\n");

	open(gramNewSet, newNUM);						//将文法展开
	PrintGram(gramnewset, newnum);					//输出展开的文法
	printf_s("\n");

	PreAnaTable();									//构造预测分析表
	printPreAnaTable();								//输出预测分析表

	return 0;
}