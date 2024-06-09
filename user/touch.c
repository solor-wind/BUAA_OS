#include <lib.h>

int main(int argc, char **argv) {
	int f;

    // int len=strlen(argv[1]);
    // char fadir[len];
    // strcpy(fadir,argv[1]);
    // for(int i=len-1;i>=0;i--)
    // {
    //     if(fadir[i]=='/')
    //     {
    //         fadir[i]='\0';
    //         break;
    //     }
    // }

    f=open(argv[1],O_CREAT);
    if(f<0){
        printf("touch: cannot touch '%s': No such file or directory\n", argv[1]);
        return f;
    }

    //f=open(fadir,O_CREAT);
	return 0;
}
