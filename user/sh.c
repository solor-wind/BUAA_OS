#include <args.h>
#include <lib.h>

#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"
#define HISTFILESIZE 20
int hist;//该写哪条指令了
char histcmd[HISTFILESIZE][1024];
int lasthist=-1;//上一条指令在哪
char lastcmd[1024];//当前指令

void runcmd(char* s);
/* Overview:
 *   Parse the next token from the string at s.
 *
 * Post-Condition:
 *   Set '*p1' to the beginning of the token and '*p2' to just past the token.
 *   Return:
 *     - 0 if the end of string is reached.
 *     - '<' for < (stdin redirection).
 *     - '>' for > (stdout redirection).
 *     - '|' for | (pipe).
 *     - 'w' for a word (command, argument, or file name).
 *
 *   The buffer is modified to turn the spaces after words into zero bytes ('\0'), so that the
 *   returned token is a null-terminated string.
 */
int _gettoken(char *s, char **p1, char **p2) {
	*p1 = 0;
	*p2 = 0;
	if (s == 0) {
		return 0;
	}

	while (strchr(WHITESPACE, *s)) {
		*s++ = 0;
	}
	
	if (*s == 0) {
		return 0;
	}

	if (strchr(SYMBOLS, *s)) {
		int t = *s;
		*p1 = s;
		*s++ = 0;
		*p2 = s;
		return t;
	}

	*p1 = s;
	while (*s && !strchr(WHITESPACE SYMBOLS, *s)) {
		if(*s=='\"'){
			char tmp[1024];
			strcpy(tmp,s+1);
			int i=0,j=0,flag=0;
			for(;tmp[j];j++){
				if(tmp[j]=='\"'&&!flag){
					flag=j;
					continue;
				}else{
					s[i++]=tmp[j];
				}
			}
			s[i]='\0';
			s+=flag-1;
		}else if(*s=='`'){
			char cmd[1024];
			char cmdans[1024];
			char after[1024];
			for(int i=1;s[i];i++){
				if(s[i]=='`'){
					cmd[i-1]='\0';
					strcpy(after,s+i+1);
					break;
				}else{
					cmd[i-1]=s[i];
				}
			}
			int p[2];
			pipe(p);
			int r=fork();
			if(r==0)
			{
				dup(p[1],1);
				close(p[0]);
				close(p[1]);
				runcmd(cmd);
				exit();
			}
			else
			{
				close(p[1]);
				int len,pos=0;
				char tmp[1024];
				while((len=read(p[0],tmp,1024))>0){
					tmp[len]='\0';
					strcpy(cmdans+pos,tmp);
					pos+=len;
				}
				close(p[0]);
			}
			strcpy(s,cmdans);
			strcpy(s+strlen(cmdans),after);
			s+=strlen(cmdans)-1;
		}
		s++;
	}
	*p2 = s;
	return 'w';
}

int gettoken(char *s, char **p1) {
	static int c, nc;
	static char *np1, *np2;

	if (s) {
		nc = _gettoken(s, &np1, &np2);
		return 0;
	}
	c = nc;
	*p1 = np1;
	nc = _gettoken(np2, &np1, &np2);
	return c;
}

#define MAXARGS 128

int parsecmd(char **argv, int *rightpipe) {
	int argc = 0;
	while (1) {
		char *t;
		int fd, r;
		int c = gettoken(0, &t);
		char ch;
		switch (c) {
		case 0:
			return argc;
		case 'w':
			if (argc >= MAXARGS) {
				debugf("too many arguments\n");
				exit();
			}
			argv[argc++] = t;
			break;
		case '<':
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: < not followed by word\n");
				exit();
			}
			// Open 't' for reading, dup it onto fd 0, and then close the original fd.
			// If the 'open' function encounters an error,
			// utilize 'debugf' to print relevant messages,
			// and subsequently terminate the process using 'exit'.
			/* Exercise 6.5: Your code here. (1/3) */
			fd=open(t,O_RDONLY);
			if(fd<0)
			{
				debugf("syntax error: < can't open file\n");
				exit();
			}
			dup(fd,0);
			close(fd);

			//user_panic("< redirection not implemented");

			break;
		case '>':
			ch=gettoken(0, &t);
			if (ch != 'w'&&ch!='>') {
				debugf("syntax error: > not followed by word\n");
				exit();
			}
			// Open 't' for writing, create it if not exist and trunc it if exist, dup
			// it onto fd 1, and then close the original fd.
			// If the 'open' function encounters an error,
			// utilize 'debugf' to print relevant messages,
			// and subsequently terminate the process using 'exit'.
			/* Exercise 6.5: Your code here. (2/3) */
			if(ch=='>'){
				ch=gettoken(0, &t);
				if (ch != 'w') {
					debugf("syntax error: > not followed by word\n");
					exit();
				}
				fd=open(t,O_RWR|O_WRONLY);
			}else{
				fd=open(t,O_WRONLY|O_TRUNC);
			}
			if(fd<0)
			{
				debugf("syntax error: > can't open file\n");
				exit();
			}
			dup(fd,1);
			close(fd);

			//user_panic("> redirection not implemented");

			break;
		case '|':;
			/*
			 * First, allocate a pipe.
			 * Then fork, set '*rightpipe' to the returned child envid or zero.
			 * The child runs the right side of the pipe:
			 * - dup the read end of the pipe onto 0
			 * - close the read end of the pipe
			 * - close the write end of the pipe
			 * - and 'return parsecmd(argv, rightpipe)' again, to parse the rest of the
			 *   command line.
			 * The parent runs the left side of the pipe:
			 * - dup the write end of the pipe onto 1
			 * - close the write end of the pipe
			 * - close the read end of the pipe
			 * - and 'return argc', to execute the left of the pipeline.
			 */
			int p[2];
			/* Exercise 6.5: Your code here. (3/3) */
			pipe(p);
			r=fork();
			*rightpipe=r;
			if(r==0)
			{
				dup(p[0],0);
				close(p[0]);
				close(p[1]);
				return parsecmd(argv, rightpipe);
			}
			else
			{
				dup(p[1],1);
				close(p[1]);
				close(p[0]);
				return argc;
			}


			//user_panic("| not implemented");

			break;
		case ';':
			r=fork();
			*rightpipe=r;
			if(r==0)
			{
				return argc;
			}
			else
			{
				wait(r);
				return parsecmd(argv, rightpipe);
			}
			break;
		}
	}

	return argc;
}

void runcmd(char *s) {
	gettoken(s, 0);

	char *argv[MAXARGS];
	int rightpipe = 0;
	int argc = parsecmd(argv, &rightpipe);
	if (argc == 0) {
		return;
	}
	argv[argc] = 0;

	if(strcmp(argv[0],"history")==0){
		int f=open(".mosh_history",O_RDONLY);
		char tmp[10240];
		readn(f,tmp,10240);
		printf("%s\n",tmp);
		exit();
	}

	int child = spawn(argv[0], argv);
	close_all();
	if (child >= 0) {
		wait(child);
	} else {
		debugf("spawn %s: %d\n", argv[0], child);
	}
	if (rightpipe) {
		wait(rightpipe);
	}
	exit();
}

void readline(char *buf, u_int n) {
	int r;
	for (int i = 0; i < n; i++) {
		if ((r = read(0, buf + i, 1)) != 1) {
			if (r < 0) {
				debugf("read error: %d\n", r);
			}
			exit();
		}
		if (buf[i] == '\b' || buf[i] == 0x7f) {
			if (i > 0) {
				i -= 2;
			} else {
				i = -1;
			}
			if (buf[i] != '\b') {
				printf("\b");
			}
		}
		if (buf[i] == '\r' || buf[i] == '\n') {
			buf[i] = 0;
			return;
		}
		if(i>=2&&buf[i-2]==27&&buf[i-1]==91&&buf[i]==65){
			i-=2;
			printf("%c%c%c",27,91,66);
			for(int j=0;j<i;j++){
				printf("\b \b");
			}
			if(lasthist==-1){
				strcpy(lastcmd,buf);
				lastcmd[i]='\0';
			}
			if(lasthist==-1){
				if(hist==0){
					if(histcmd[HISTFILESIZE-1][0]!='\0'){
						lasthist=HISTFILESIZE-1;
					}else{
						lasthist=0;//已经是最前面的指令
					}
				}else{
					lasthist=hist-1;
				}
			}else{
				if(lasthist==0&&histcmd[HISTFILESIZE-1][0]=='\0'){
					;//已经是最前面的指令
				}else{
					if(lasthist==hist){
						;//已经是最前面的指令
					}else{
						lasthist--;
						if(lasthist<0){
							lasthist=HISTFILESIZE-1;
						}
					}
				}
			}
			strcpy(buf,histcmd[lasthist]);
			if(buf[strlen(buf)-1]=='\n'){
				buf[strlen(buf)-1]='\0';
			}
			printf("%s", buf);
			i=strlen(buf)-1;
			continue;
		}
		else if(i>=2&&buf[i-2]==27&&buf[i-1]==91&&buf[i]==66){
			i-=2;
			//printf("%c%c%c",27,91,65);
			for(int j=0;j<i;j++){
				printf("\b \b");
			}
			if(lasthist==-1){
				strcpy(lastcmd,buf);
				lastcmd[i]='\0';
			}
			if(lasthist==-1){
				lasthist==-2;
			}else{
				if((hist==0&&lasthist==HISTFILESIZE-1)||(hist&&lasthist==hist-1)){
					lasthist=-2;
				}else{
					lasthist++;
					if(lasthist>=HISTFILESIZE){
						lasthist=0;
					}
					if(histcmd[lasthist][0]=='\0'){
						lasthist=-2;
					}
				}
			}
			if(lasthist<0){
				strcpy(buf,lastcmd);
			}else{
				strcpy(buf,histcmd[lasthist]);
			}
			if(buf[strlen(buf)-1]=='\n'){
				buf[strlen(buf)-1]='\0';
			}
			printf("%s", buf);
			i=strlen(buf)-1;
			continue;
		}
	}
	debugf("line too long\n");
	while ((r = read(0, buf, 1)) == 1 && buf[0] != '\r' && buf[0] != '\n') {
		;
	}
	buf[0] = 0;
}

char buf[1024];

void usage(void) {
	printf("usage: sh [-ix] [script-file]\n");
	exit();
}

void wirte_history(){
	int tmpflag=0;
	for(int i=0;buf[i];i++){
		if(buf[i]!='\t'||buf[i]!='\r'||buf[i]!='\n'){
			tmpflag=1;
		}
	}
	if(!tmpflag){
		return;
	}

	int f=open(".mosh_history",O_RDONLY);
	if(f<0){
		f=open(".mosh_history",O_CREAT);
	}
	close(f);

	if(hist<HISTFILESIZE&&histcmd[hist][0]=='\0'){//还没满
		f=open(".mosh_history",O_RWR|O_WRONLY);
		strcpy(histcmd[hist],buf);
		int len=strlen(buf);
		histcmd[hist][len]='\n';
		histcmd[hist][len+1]='\0';
		write(f,histcmd[hist],strlen(histcmd[hist]));
		close(f);
		hist++;
		if(hist==HISTFILESIZE){
			hist=0;
		}
	}
	else{
		strcpy(histcmd[hist],buf);
		int len=strlen(buf);
		histcmd[hist][len]='\n';
		histcmd[hist][len+1]='\0';

		f=open(".mosh_history",O_WRONLY|O_TRUNC);
		hist++;
		if(hist==HISTFILESIZE){
			hist=0;
		}
		int flag=hist;
		write(f,histcmd[hist],strlen(histcmd[hist]));
		close(f);
		f=open(".mosh_history",O_RWR|O_WRONLY);
		while(1){
			hist++;
			if(hist==HISTFILESIZE){
				hist=0;
			}
			if(hist==flag){
				break;
			}
			write(f,histcmd[hist],strlen(histcmd[hist]));
		}
	}
	lasthist=-1;
	lastcmd[0]='\0';
}

int main(int argc, char **argv) {
	int r;
	int interactive = iscons(0);
	int echocmds = 0;
	printf("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	printf("::                                                         ::\n");
	printf("::                     MOS Shell 2024                      ::\n");
	printf("::                                                         ::\n");
	printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	ARGBEGIN {
	case 'i':
		interactive = 1;
		break;
	case 'x':
		echocmds = 1;
		break;
	default:
		usage();
	}
	ARGEND

	if (argc > 1) {
		usage();
	}
	if (argc == 1) {
		close(0);
		if ((r = open(argv[0], O_RDONLY)) < 0) {
			user_panic("open %s: %d", argv[0], r);
		}
		user_assert(r == 0);
	}
	for (;;) {
		if (interactive) {
			printf("\n$ ");
		}
		// while(read(0,buf,sizeof buf)<0);
		// if(buf[0]==27&&buf[1]==91&&buf[2]==65){
		// 	printf("%c%c%c", 27, 91, 66);
		// 	printf("\b \b");
		// 	printf("ok\n");
		// 	continue;
		// }
		readline(buf, sizeof buf);
		
		wirte_history();

		if (buf[0] == '#') {
			continue;
		}
		if (echocmds) {
			printf("# %s\n", buf);
		}
		if ((r = fork()) < 0) {
			user_panic("fork: %d", r);
		}
		if (r == 0) {
			for(int i=0;buf[i];i++)
			{
				if(buf[i]=='#')
				{
					buf[i]='\0';
					break;
				}
			}
			runcmd(buf);
			exit();
		} else {
			wait(r);
		}
	}
	return 0;
}
