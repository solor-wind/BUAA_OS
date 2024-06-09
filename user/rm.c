#include <lib.h>

int main(int argc, char **argv) {
	int f;

    if(argc<=2)
    {
        f=open(argv[1],O_RDONLY);
        if(f<0){
            printf("rm: cannot remove '%s': No such file or directory\n",argv[1]);
            return f;
        }
        close(f);
        f=open(argv[1],O_TYPE);
        if(f!=FTYPE_REG){
            printf("rm: cannot remove '%s': Is a directory\n",argv[1]);
            return -1;
        }
        f=remove(argv[1]);
    }
    else if(strcmp(argv[1],"-r")==0)
    {
        for(int i=2;i<argc;i++)
        {
            f=open(argv[i],O_RDONLY);
            if(f<0){
                printf("rm: cannot remove '%s': No such file or directory\n",argv[i]);
                return f;
            }
            close(f);
        }
        for(int i=2;i<argc;i++)
        {
            f=remove(argv[i]);
        }
    }
    else if(strcmp(argv[1],"-rf")==0)
    {
        for(int i=2;i<argc;i++)
        {
            f=remove(argv[i]);
        }
    }
	return 0;
}
