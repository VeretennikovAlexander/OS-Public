#include <stdio.h>
#include <stdint.h>

uint64_t f1(uint64_t n)
{
	if (n <= 0) return 1;
	return n + f1(n-1);
};

int main(char* args)
{
	//87000 - �� �������� ��� ���� 32-����.
	//12 ���� �� ����� �� ��������.
	//86000 * 12 = 1032000

	//65000 - �� �������� ��� ���� 64-����.
	//16 ���� �� ����� �� ��������.
	//64000 * 16 = 1024000

	uint64_t r = f1(64000);
	printf("%I64d",r);
	return 0;	
}
