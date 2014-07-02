#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<signal.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
int special;
pid_t globalp;
typedef struct qu
{
	char name[1000];
	int id;
}qu;
qu queue[100];
int top;
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
		j++;
		parts=strtok(NULL,del);
	}
	if (j!=0 && *cmdparts[j-1]=='&')						//identifying a background process
	{
		//		strcpy(queue[top].name,cmdparts[0]);
		//		queue[top].id=getpid();
		//		top++;
		//	printf("check\n");
		special=1;
		cmdparts[j-1]=NULL;
	}
	//	if (j==1 && !strcmp(cmdparts[0],"cd"))
	//		strcpy(cmdparts[j],"~");
	cmdparts[j++]=NULL;
	//	printf("%s\n",cmdparts[0]);
	cmdargc=j;
	//	printf ("%d\n",cmdargc);
	//		if (cmdparts[1]==NULL)
	//			printf("SDAS");
	return cmdargc;
}

void conc(char a1[],char a2[])						//concatenation function(not needed)
{
	int len1,len2;
	int i;
	//	printf("A1:%s\n A2:%s\n",a1,a2);
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
	int flag2=0;
	char arr4[1000];
	int flag=0;
	printf("\n\n\n\n\n\n                ------------------------------------------------------------------------------------------------------------\n                                                  WELCOME TO JIGAR'S TERMINAL!!!\n                ------------------------------------------------------------------------------------------------------------\n\n\n\n\n\n");
	getcwd(s,100);
	while (1)								//main loop 
	{
		signal(SIGINT,SIG_IGN);						//signal of ctrl d and ctrl c ignored
		signal(SIGQUIT,SIG_IGN);
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
		char c;
		int status;
		int q;
		int cmdargc;
		int i=0,j=0;
		int len;
		int rval;
		rval=waitpid(-1,&status,WNOHANG);			//checking whether the background process terminated or not
		//		printf ("%d",rval);
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
		//	printf("%s\n",s);
		//	printf("%s\n",arr1);
		char *ka;
		ka=strstr(arr1,s);
		//	printf("%s",ka);
		//	printf("%s\n",arr1);
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
		cmdargc=parse(parts,istring,cmdparts);			//parse ^^ mentioned above
		//*cmdparts[0]='\0';
		//	{
		
////		printf("PARSE%d",j);
		if (!(strcmp(cmdparts[0],"cd")))
		{	
			change(cmdparts);
		}
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
				//			printf("%s",add);
				FILE *fd;
				if (fd=fopen(add,"r"))
				{
					getline(&tmp,&wt,fd);
					printf("%s",tmp);
					getline(&tmp,&wt,fd);
					printf("%s",tmp);
					getline(&tmp,&wt,fd);
					//	printf("Process Name: %s\n",tmp);
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
				//			printf("%s",add);
				FILE *fd;
				if (fd=fopen(add,"r"))
				{
					getline(&tmp,&wt,fd);
					printf("%s",tmp);
					getline(&tmp,&wt,fd);
					printf("%s",tmp);
					getline(&tmp,&wt,fd);
					//	printf("Process Name: %s\n",tmp);
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
			//		printf("%s %d\n",queue[top-1].name,top);	
			//		printf("%s\n",queue[0].name);
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
				kill(queue[w].id,9);			//kill all background processes
			}
			if (top!=0)
			{
				top=1;
				printf("All background processes killed\n");
			}
		}
		else if (!strcmp(cmdparts[0],"kjob"))
		{
			if (cmdparts[2]==NULL)
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
				//	top--;
				}

			}
//			printf("%d",top);
//			printf("%s\n",queue[0].name);
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
			//printf("\n\n\n                                 THANK YOU FOR USING MY TERMINAL                       \n\n\n");
			exit(0);
		}
		else 
		{
			//		printf("YES\n");
			pid_t pid,pid2;
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
//					else
					{	perror("COMMAND NOT FOUND");
					}
					_exit(0);
				}
//				//				printf("PROCESS WITH PID %d COMPLETE\n",pid);
				else 
				{
					exit(EXIT_SUCCESS);
				}
			}
			else
			{
//				printf("d%d",j);
				//			printf("WAITING\n");
				if (special==1)					//for background process
				{
					int w=0;
					for (w=0;w<top;w++)
					{
						printf("%s\n",queue[top].name);
					}
//					printf("%d\n",top);
//					printf("%s\n",cmdparts[0]);
					strcpy(queue[top].name,cmdparts[0]);
					queue[top].id=pid;				//store them in a queue
					top++;						//dont wait!
					special=0;					
//					printf("JAA%d",j);
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
			}	
		}
		int l;
	}
	return 0;
}

