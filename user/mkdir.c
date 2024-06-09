#include <lib.h>

int main(int argc, char **argv) {
	int f;
    if(argc<=2){
        f=open(argv[1],O_RDONLY);
        if(f>=0)
        {
            printf("mkdir: cannot create directory '%s': File exists\n", argv[1]);
            close(f);
            return f;
        }
        f=open(argv[1],O_MKDIR);
        if(f<0){
            printf("mkdir: cannot create directory '%s': No such file or directory", argv[1]);
            return f;
        }else{
            close(f);
        }
        return 0;
    }
    
    if(strcmp(argv[1],"-p")==0)
    {
        int len=strlen(argv[2]);
        char fadir[len];
        for(int i=0;i<len;i++)
        {
            if(argv[2][i]=='/')
            {
                if(i)
                {
                    fadir[i]='\0';
                    f=open(fadir,O_MKDIR);
                }
            }
            fadir[i]=argv[2][i];
        }
        f=open(argv[2],O_MKDIR);        
    }

	return 0;
}
