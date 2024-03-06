#include <stdio.h>
int main() {
	int n;
	scanf("%d", &n);
	int flag=1,len=0;
	int arr[10];
	while(n){
		arr[len++]=n%10;
		n/=10;
	}
	for(int i=0;i<len;i++){
		if(arr[i]!=arr[len-i-1]){
			flag=0;
			break;
		}
	}
	if (flag) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
