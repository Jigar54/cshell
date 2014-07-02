#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<signal.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<fcntl.h>
int special;								//an flag integer for redirectonoperators
int ptr;				
int special2;							
pid_t globalp;								//global pid
typedef struct qu
{				
        char name[1000];
        int id;
}qu;
qu queue[100];
int top;								//queue of history used in 1st part
int pipepoints[100];

void pipes(char *parts,char *string,char *cmdparts[],int status)			//function used to handle pipes
{											//looks after pipes+pipes with redirection
        pid_t pid2;
        int sout=dup(STDOUT_FILENO);							//creating copies of stdin nd out
        int sin=dup(STDIN_FILENO);						
        char *pars[100];
        int old[2];		
        int fd=0;
        int i=0;
        int j=0;							
        char *cmd[100];
        char *cmd2[100];
        int gtr=0,lt=0;
        int pfd[2];

        parts=strtok(string,"|");
        while ( parts != NULL)						//parsing the function about |
        {
                cmdparts[j]=parts;
                parts=strtok(NULL,"|");
                j++;
        }
        cmdparts[j]='\0';						//cmdparts is the required 2d array
        int nfpipes=j;
        for (i=0;i<nfpipes;i++)					
        {							
                lt=0;
                gtr=0;
                int a,b,c;char *arr3[100];
                int pipearr[1000];
                if (strstr(cmdparts[i],">"))
                {
                        gtr=1;
                }
                if (strstr(cmdparts[i],"<"))
                {
                        lt=1;		
                }		
                if (gtr==1)
                {				
                        int gctr=0;
                        char *parts2;						
                        parts2=strtok(cmdparts[i],">");			//parsing again by > sign and cmd is required 2d array
                        while ( parts2!=NULL)
                        {
                                cmd[gctr]=parts2;
                                parts2=strtok(NULL,">");
                                gctr++;
                        }
                        cmd[gctr]='\0';
                        int gctr2=0;
                        char *parts3;
                        parts3=strtok(cmd[0]," ");
                        while (parts3!= NULL)				//parsing by spaces
                        {
                                cmd2[gctr2]=parts3;				
                                parts3=strtok(NULL," ");
                                gctr2++;
                        }
                        cmd2[gctr2]='\0';
                        cmd[1]=strtok(cmd[1]," ");
                }
                else if (lt==1)							//similar parsing if theres a < sign !
                {
                        int gctr=0;
                        char arr3[100];
                        char *parts2;
                        parts2=strtok(cmdparts[i],"<");			
                        while ( parts2!=NULL)		
                        {				
                                cmd[gctr]=parts2;
                                parts2=strtok(NULL,"<");
                                gctr++;
                        }
                        cmd[gctr]='\0';
                        parts2=strtok(cmd[0]," ");						
                        char arr2[1000];
                        int gctr2=0;
                        char *parts3;
                        parts3=strtok(cmd[0]," ");
                        while (parts3!= NULL)
                        {
                                cmd2[gctr2]=parts3;
                                parts3=strtok(NULL," ");
                                gctr2++;
                        }
                        cmd2[gctr2]='\0';
                        cmd[1]=strtok(cmd[1]," ");
                }		
                else 
                {
                        int ctr=0;
                        char *parts;
                        parts=strtok(cmdparts[i]," ");
                        while(parts!=NULL)
                        {
                                cmd[ctr]=parts;
                                ctr++;
                                parts=strtok(NULL," ");
                        }
                        cmd[ctr]='\0';
                }
                int f=0;
                if (i +1 < nfpipes)
                {
                        pipe(pfd);					//using pipe to store input and output word
                }
                pid2=fork();
                if (i>0 && i<nfpipes-1)				//not end point	
                {
                        dup2(old[0],0);
                        close(old[0]);					//duplicating the input as 1sts output
                        close(old[1]);
                }
                else if(i+1==nfpipes)					//last word
                {
                        dup2(old[0],0);			 	
                        close(old[1]);
                        close(old[0]);
                }
                if (pid2==0)					
                {				//child

                        if (i==0)
                        {					
                                dup2(pfd[1],1);			//output duplication nt stdout
                                close(pfd[1]);
                                close(pfd[0]);
                        }
                        else if(i>0 && i<nfpipes-1)
                        {
                                dup2(pfd[1],1);
                                close(pfd[1]);
                                close(pfd[0]);	
                        }	
                        if ( gtr==0 && lt == 0)					//normal pipes execution
                        {
                                int ans = execvp(cmd[0],cmd);				//jst execute as changing destination and source done
                                if (ans=-1)
                                {
                                        printf("SRY COMMAND NOT FOUND\n");
                                }
                                exit(1);
                        }
                        else if ( lt == 1 )					//case of input redirection
                        {
                                int fp;
                                FILE *f1;
                                printf("1.%s\n",cmd2[0]);	
                                fp=open(cmd[1],O_RDONLY);			//open file
                                dup2(fp,0);					//changing input source
                                int ans=execvp(cmd2[0],cmd);
                                if (ans < 0)
                                {

                                        perror("Sorry Command Not Found\n");
                                }
                                close(fp);
                        }
                        else if (gtr==1 )
                        {
                                int fd;	
                                FILE *f2;
                                //output redirection case
                                fd=open( cmd[1],O_CREAT | O_WRONLY, 0600);		//opening dest. file
                                dup2(fd,1);					
                                int ans=execvp(cmd2[0],cmd);				
                                if (ans < 0 )
                                {
                                        perror("Sorry Command Not Found\n");
                                }
                                close(fd);
                                gtr=0;
                        }
                }
                else 
                {
                        waitpid (pid2,&status,0);
                        if (i +1 < nfpipes )			
                        {						//present pipe is the previous pipe for next loop
                                old[0]=pfd[0];
                                old[1]=pfd[1];
                        }
                }
        }
        dup2 (sout,1);
        dup2(sin,0);						//sin and sout dup'ed as stdin and stdout previously
        close(sout);						//reduping dem to stdout and stdin
        close(sin);
        close(old[0]);
        close(old[1]);
        close(pfd[1]);
        close(pfd[0]);
        special2=0;
        return;
}

int parse(char *parts,char *istring,char *cmdparts[])				//parse function
{										//It splits the argument(command) by spaces
        int j=0;								//and stores it in an array of pointers
        int cmdargc;								//each pointer has a string stored in it
        char del[]=" \t";
        char *part;
        parts=strtok(istring,del);
        while (parts!= NULL)
        {
                cmdparts[j]=parts;
                if (!strcmp(parts,">"))						//output redirection
                {	
                        if (special==3)
                        {
                                special=4;
                                cmdparts[j]='\0';				//setting null chrcter
                                ptr=j;	
                        }
                        else
                        {
                                special=2;
                                cmdparts[j]='\0';
                                ptr=j;
                        }
                }
                else if (!strcmp(parts,"<"))				//input redirection
                {
                        special=3;
                        cmdparts[j]='\0';
                        ptr=j;
                }
                j++;
                parts=strtok(NULL,del);
        }
        if (j!=0 && *cmdparts[j-1]=='&')						//identifying a background process
        {
                special=1;
                cmdparts[j-1]=NULL;
        }
        cmdparts[j++]=NULL;
        cmdargc=j;
        return cmdargc;
}

void redirect(char *istring,char *cmdparts[],int status)
{
        int j=0;
        char *parts;									//and stores it in an array of pointers
        int cmdargc;								//each pointer has a string stored in it
        char del[]=" \t";
        char *part;
        parts=strtok(istring,del);
        while (parts!= NULL)
        {
                cmdparts[j]=parts;
                if (!strcmp(parts,">"))
                {	
                        if (special==3)							//parsing:- checking which type of redireciton
                        {
                                special=4;
                                cmdparts[j]='\0';
                                ptr=j;	
                        }
                        else
                        {
                                special=2;
                                cmdparts[j]='\0';
                                ptr=j;
                        }
                }
                else if (!strcmp(parts,"<"))
                {
                        special=3;
                        cmdparts[j]='\0';
                        ptr=j;
                }
                j++;
                parts=strtok(NULL,del);
        }
        if (j!=0 && *cmdparts[j-1]=='&')						//identifying a background process
        {
                special=1;
                cmdparts[j-1]=NULL;
        }
        cmdparts[j++]=NULL;
        pid_t pid,pid2;
        int sout=dup(STDOUT_FILENO);
        int sin=dup(STDIN_FILENO);
        int fp=0;
        int fd=0;
        if (special==2)						// ouput redirection case
        { 
                fp=open(cmdparts[ptr+1],O_CREAT | O_WRONLY,0777 );
                dup2(fp,1);						//chaning stdout to a file 
        }
        else if (special==3)							//input redirection case
        {
                fp=open(cmdparts[ptr+1],O_RDONLY);
                dup2(fp,0);							//changing stdin to a file
        }
        else if (special==4)							//output + input redirection case
        {
                fp=open(cmdparts[ptr-1],O_RDONLY);
                dup2(fp,0);
                fd=open(cmdparts[ptr+1],O_CREAT | O_WRONLY,0777 );
                dup2(fd,1);
        }
        pid=fork();			//fork processes into child and parent
        globalp=pid;
        if (pid<0)			//error in forking
        {
                perror("ERROR DIDNT FORK PROPERLY\n");
                _exit(0);
        }
        if (pid==0)
        {						//child process->executes the commands

                int val;
                val=execvp(cmdparts[0],cmdparts);
                if (val==-1)
                {
                        if (!strcmp("^",cmdparts[0]));				//if no command then ^ is the flag
                        else
                        {	
                                perror("COMMAND NOT FOUND");
                        }
                        _exit(0);
                }
                else 
                {
                        exit(EXIT_SUCCESS);
                }
        }
        else
        {
                if (special==1)					//for background process
                {
                        strcpy(queue[top].name,cmdparts[0]);
                        queue[top].id=pid;				//store them in a queue
                        top++;						//dont wait!
                        special=0;					
                }
                else 
                {
                        waitpid(pid,&status,WUNTRACED);			
                        if (WIFSTOPPED(status))				//for functioning of ctrl z
                        {						//only if ctrl z put them in queue
                                strcpy(queue[top].name,cmdparts[0]);	//as now they run in background
                                queue[top].id=pid;
                                top++;
                        }
                }
                if (fp>0)
                        close(fp);
                special=0;
                dup2(sout,1);
                dup2(sin,0);
                close(sout);					//closing and reduplicating the stdout and stdin to normal values
                close(sin);	
        }
        return ;
}

int checkpipes(char *cmdparts[])
{
        int j=0;
        int k=0;
        int l=0;
        pipepoints[l++]=-1;
        while (cmdparts[j]!=NULL)
        {
                if (!strcmp(cmdparts[j],"|"))
                {
                        pipepoints[l++]=j;
                        cmdparts[j]=NULL;
                }
                j++;
        }
        cmdparts[j] = NULL;
        return l;
}


void conc(char a1[],char a2[])						//concatenation function(not needed)
{
        int len1,len2;
        int i;
        len1=strlen(a1);
        len2=strlen(a2);
        for (i=len1;i<len1+len2;i++)
        {
                a1[i]=a2[i-len1];
        }
        return ;
}

void sighand(int a)							//signal handler function for ctrl z
{
        if (a==SIGTSTP)
        {
                kill(globalp,SIGTSTP);
        }
}
char s[1000];
void change(char *cmdparts[])								//funciton for change directory
{
        char *tmp='\0';
        char cd[1000];
        char cd2[1000];
        if (cmdparts[1]==NULL||!strcmp(cmdparts[1],"~") ||  (!strcmp(cmdparts[1],"~/")))
        {
                if (chdir(s))							//if argument 1 says we need to return to the homedirectory
                {
                        perror("ERROR!");					//error msg to be displayed if no directory found
                }
                else
                {
                        return;
                }
        }
        else
        {
                strcpy(cd,cmdparts[1]);
        }
        tmp=getcwd(tmp,100);							//changing the directory
        strcpy(cd2,tmp);
        strcat(cd2,"/");
        strcat(cd2,cmdparts[1]);
        if (chdir(cd2))
                perror("ERROR!!");
        else
                return;
}


int main()
{
        pid_t mainpid;
        mainpid=getpid();
        top=0;
        char arr4[1000];
        int flag=0;
        special2=0;
        special=0;
        printf("\n\n\n\n\n\n                ------------------------------------------------------------------------------------------------------------\n                                                  WELCOME TO JIGAR'S TERMINAL!!!\n                ------------------------------------------------------------------------------------------------------------\n\n\n\n\n\n");
        getcwd(s,100);
        while (1)								//main loop 
        {
                char *cmdparts[1000];
                int k=0;
                for (k=0;k<100;k++)
                {
                        cmdparts[k]=(char *)malloc(sizeof(char)*1000);
                }
                char arr1[10000];
                char arr2[1000];
                char arr3[1000];
                char aa[100];
                char *parts;
                char istring[1000];
                int status;
                char c;
                int q;
                int cmdargc;
                int i=0,j=0;
                int len;
                int rval;
                rval=waitpid(-1,&status,WNOHANG);			//checking whether the background process terminated or not
                if (rval>0)						//if terminated then returns >0 value
                {
                        printf("PROCESS %d STOPPED\n",rval);
                        int w;
                        for (w=0;w<top;w++)
                        {
                                if (queue[w].id==rval)				//removing processfrom the queue
                                        break;
                        }
                        if (top-1>0)
                        {
                                for (;w<top-1;w++)
                                {
                                        queue[w].id=queue[w+1].id;		//shifting processes in the queue
                                        strcpy(queue[w].name,queue[w+1].name);  //can be done by liked lists too
                                }
                        }
                        top--;
                }
                getcwd(arr1,10000);						//current working directory
                gethostname(arr2,1000);						//get machine name
                strcpy(arr3,"~");
                char *ka;
                ka=strstr(arr1,s);
                if (ka==NULL)
                {
                        printf("%s@%s:%s> ",getlogin(),arr2,arr1);		//prompt!
                }
                else
                {
                        ka=ka+strlen(s);
                        strcat(arr3,ka);
                        printf("<%s@%s:%s> ",getlogin(),arr2,arr3);		//prompt for home
                }

                while ((c=getchar())!='\n')				//input the string(command)
                {
                        istring[i]=c;
                        i++;
                }
                if (i==0)
                        istring[i++]='^';					//in case no input given
                istring[i++]='\0';
                len=i;
                i=0;

                char copy[100];
                strcpy(copy,istring);
                cmdargc=parse(parts,istring,cmdparts);			//parse ^^ mentioned above
                if (!(strcmp(cmdparts[0],"cd")))
                {
                        change(cmdparts);
                }

                else if(strstr(copy,"|"))
                {
                        char new[100];
                        pipes(new,copy,cmdparts,status);
                }
                else if ((strstr(copy,">")) || (strstr(copy,"<")) && (!strstr(copy,"|")))
                        redirect(copy,cmdparts,status);
                else if (!strcmp(cmdparts[0],"pinfo"))			//if user defined fuction pinfo
                {
                        if (cmdparts[1]==NULL)
                        {
                                int w=0;
                                size_t wt=0;
                                char *tmp;
                                char main[100000];
                                char add[1000];
                                char add1[100];
                                sprintf(main,"%d",mainpid);			//go in /proc/pid/status and get info frm there
                                strcpy(add,"/proc/");
                                strcat(add,main);
                                strcpy(add1,add);
                                strcat(add,"/status");
                                FILE *fd;
                                if (fd=fopen(add,"r"))
                                {
                                        getline(&tmp,&wt,fd);
                                        printf("%s",tmp);
                                        getline(&tmp,&wt,fd);
                                        printf("%s",tmp);
                                        getline(&tmp,&wt,fd);
                                        getline(&tmp,&wt,fd);
                                        printf("%s",tmp);
                                        int ctr=0;
                                        while (1)
                                        {
                                                getline(&tmp,&wt,fd);
                                                if(!strncmp(tmp,"VmSize",6))
                                                {
                                                        printf("%s",tmp);
                                                        break;
                                                }
                                                if (ctr==15)
                                                {
                                                        printf("Vmsize not given in status file :P!\n");
                                                        break;
                                                }
                                        }
                                        int byte;
                                        strcat(add1,"/exe");		///proc/pid/exe is a file which has symbolic link of process
                                        char t[100];
                                        byte=readlink(add1,t,sizeof(t));
                                        t[byte]='\0';
                                        printf("Executable Path: %s\n",t);
                                }
                                else
                                {
                                        printf("PATH DOESNT EXIST\n");
                                }
                        }
                        else
                        {
                                int w=0;				//pinfo pid (process almost same)
                                size_t wt=0;				
                                char *tmp;
                                char main[100000];
                                char add[1000];
                                char add1[1000];
                                strcpy(add,"/proc/");
                                strcat(add,cmdparts[1]);
                                strcpy(add1,add);
                                strcat(add,"/status");
                                FILE *fd;
                                if (fd=fopen(add,"r"))
                                {
                                        getline(&tmp,&wt,fd);
                                        printf("%s",tmp);
                                        getline(&tmp,&wt,fd);
                                        printf("%s",tmp);
                                        getline(&tmp,&wt,fd);
                                        getline(&tmp,&wt,fd);
                                        printf("%s",tmp);
                                        int ctr=0;
                                        while (1)
                                        {
                                                ctr++;
                                                getline(&tmp,&wt,fd);
                                                if(!strncmp(tmp,"VmSize:",6))
                                                {
                                                        printf("%s",tmp);
                                                        break;
                                                }
                                                if (ctr==15)
                                                {
                                                        printf("Vmsize not given in status file :P!\n");
                                                        fclose(fd);
                                                        break;
                                                }
                                        }
                                        int byte;
                                        strcat(add1,"/exe");
                                        char t[100];
                                        byte=readlink(add1,t,sizeof(t));
                                        t[byte]='\0';
                                        printf("Executable Path: %s\n",t);
                                }
                                else
                                {
                                        printf("PATH DOESNT EXIST\n");
                                }
                        }

                }
                else if (signal(SIGTSTP,sighand)==SIG_ERR);		//calling funciton for ctrl z
                else if (!strcmp(cmdparts[0],"jobs"))
                {
                        int w;
                        for (w=0;w<top;w++)					//listing all processes stored in a queue
                        {							//background processes listed
                                if (queue[w].id!=-999)
                                        printf("[%d] Process name:%s pid:%d\n",w+1,queue[w].name,queue[w].id);
                        }
                }
                else if (!strcmp(cmdparts[0],"overkill"))
                {
                        int w;
                        for (w=0;w<top;w++)
                        {	
                                kill(queue[w].id,SIGTERM);			//kill all background processes
                        }	
                        top=0;
                        printf("All background processes killed\n");
                }
                else if (!strcmp(cmdparts[0],"kjob"))
                {
                        if (cmdparts[2]==NULL || cmdparts[1]==NULL)
                        {
                                printf("PLEASE GIVE THE SECOND ARGUMENT TOO\n");
                        }
                        else
                        {
                                int w=0;
                                for (w=0;w<top;w++)
                                {
                                        if (atoi(cmdparts[1])==w+1)
                                        {
                                                kill(queue[w].id,atoi(cmdparts[2]));		//kill a specific process with serial number given
                                                break;
                                        }
                                }
                                if (w==top)
                                        printf("The Process with serial number %d does not exist!\n",atoi(cmdparts[1]));
                                else {
                                        if (top-1>0)
                                        {
                                                for (;w<top-1;w++)
                                                {
                                                        queue[w].id=queue[w+1].id;
                                                        strcpy(queue[w].name,queue[w+1].name);
                                                }
                                        }
                                        top--;
                                }

                        }
                        printf("%d",top);
                        printf("%s\n",queue[0].name);
                }
                else if (!strcmp(cmdparts[0],"fg"))
                {
                        int w=0;
                        for (w=0;w<top;w++)
                        {
                                if (atoi(cmdparts[1])==w+1)
                                {						
                                        kill(queue[w].id,SIGCONT);		//kill process from background
                                        wait(queue[w].id,SIGCONT);		//move to foreground
                                        break;
                                }
                        }
                        if (w==top)
                                printf("The Process with serial number %d does not exist!\n",atoi(cmdparts[1]));
                        else {

                                if (top-1>0)
                                {
                                        for (;w<top-1;w++)
                                        {
                                                queue[w].id=queue[w+1].id;		//shifting array nd deleting processes from queue
                                                strcpy(queue[w].name,queue[w+1].name);		
                                        }
                                }
                                top--;
                        }
                }
                else if(!strcmp(cmdparts[0],"quit"))
                {
                        printf("\n\n\n\n\n\n                ------------------------------------------------------------------------------------------------------------\n                                                  THANK YOU FOR USING MY TERMINAL! :)\n                ------------------------------------------------------------------------------------------------------------\n\n\n\n\n\n");
                        exit(0);
                }

                else 
                {
                        pid_t pid,pid2;
                        int sout=dup(STDOUT_FILENO);
                        int sin=dup(STDIN_FILENO);
                        int fp=0;
                        int fd=0;
                        if (special==2)
                        { 
                                fp=open(cmdparts[ptr+1],O_CREAT | O_WRONLY,0777 );
                                dup2(fp,1);
                        }
                        else if (special==3)
                        {
                                fp=open(cmdparts[ptr+1],O_RDONLY);
                                dup2(fp,0);
                        }
                        else if (special==4)
                        {
                                fp=open(cmdparts[ptr-1],O_RDONLY);
                                dup2(fp,0);
                                fd=open(cmdparts[ptr+1],O_CREAT | O_WRONLY,0777 );
                                dup2(fd,1);
                        }
                        else if (special2==1)
                        {

                        }
                        pid=fork();			//fork processes into child and parent
                        globalp=pid;
                        if (pid<0)			//error in forking
                        {
                                perror("ERROR DIDNT FORK PROPERLY\n");
                                _exit(0);
                        }
                        if (pid==0)
                        {						//child process->executes the commands

                                int val;
                                val=execvp(cmdparts[0],cmdparts);
                                if (val==-1)
                                {
                                        if (!strcmp("^",cmdparts[0]));
                                        else
                                        {	
                                                perror("COMMAND NOT FOUND");
                                        }
                                        _exit(0);
                                }
                                else 
                                {
                                        exit(EXIT_SUCCESS);
                                }
                        }
                        else
                        {
                                if (special==1)					//for background process
                                {
                                        strcpy(queue[top].name,cmdparts[0]);
                                        queue[top].id=pid;				//store them in a queue
                                        top++;						//dont wait!
                                        special=0;					
                                }
                                else 
                                {
                                        waitpid(pid,&status,WUNTRACED);			
                                        if (WIFSTOPPED(status))				//for functioning of ctrl z
                                        {						//only if ctrl z put them in queue
                                                strcpy(queue[top].name,cmdparts[0]);	//as now they run in background
                                                queue[top].id=pid;
                                                top++;
                                        }
                                }
                                if (fp>0)
                                        close(fp);
                                special=0;
                                dup2(sout,1);
                                dup2(sin,0);
                                close(sout);
                                close(sin);
                        }
                }
                int l;
        }
        return 0;
}

