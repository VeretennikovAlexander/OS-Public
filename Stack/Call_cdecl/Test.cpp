#include <stdio.h>

int __cdecl f1(int a1, int a2)
{
	int v1;
	char c[100];
	v1 = c[12];
	v1 += a1;
	return v1;
};

int main(char* args)
{
	f1(1001,1002);

	return 0;
}
