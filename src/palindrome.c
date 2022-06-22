#include<stdio.h>
int main()
{
	int n;
	scanf("%d", &n);
	int i = 0;
	int n1 = n;
	while (n > 0) {
		i = i * 10;
		i += n % 10;
		n /= 10;
	}
	if (i == n1) {
		printf("Y");
	} else {
		printf("N");
	}
	return 0;
}
