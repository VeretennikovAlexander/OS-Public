#include <stdio.h>
#include <stdint.h>

uint64_t f1(uint64_t n)
{
	if (n <= 0) return 1;
	return n + f1(n-1);
};

int main(char* args)
{
	//87000 - не работает при типе 32-бита.
	//12 байт на вызов по рекурсии.
	//86000 * 12 = 1032000

	//65000 - не работает при типе 64-бита.
	//16 байт на вызов по рекурсии.
	//64000 * 16 = 1024000

	uint64_t r = f1(64000);
	printf("%I64d",r);
	return 0;	
}
