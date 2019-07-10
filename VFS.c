#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<io.h>
#define MAXINODE 50
#define READ 1
#define WRITE 2
#define MAXFILESIZE 1024
#define REGULAR 1
#define SPECIAL 2

#define START 0
#define CURRENT 1
#define END 2
  
typedef struct superblock
{
	int totalinodes;
	int freeinodes;
}SUPERBLOCK,*PSUPERBLOCK;
typedef struct inode
{
	char filename[50];
	int inodenumber;
	int filesize;
	int fileactualsize;
	int filetype;
	char * buffer;
	int linkcount;
	int referencecount;
	int permission;
	struct inode * next;

}INODE,*PINODE,**PPINODE;
typedef struct filetable
{
	int readoffset;
	int writeoffset;
	int count;
	int mode;
	PINODE ptrinode;

}FILETABLE,*PFILETABLE;

typedef struct ufdt
{
	PFILETABLE ptrfiletable;

}UFDT;
UFDT ufdtarr[MAXINODE];
SUPERBLOCK superblockobj;
PINODE head = NULL;
void man(char * name)
{
	if (name == NULL)
	{
		return;
	}
	if (strcmp(name, "create") == 0)
	{
		printf("Description:Used to create new regular file\n");
		printf("Usage: create FILE_NAME PERMISSIOn\n");
	}
	else if (strcmp(name, "read") == 0)
	{
		printf("Description:Used to read data from regular file\n");
		printf("Usage:read FILE_NAME NO_OF_BYTES\n");
	}
	else if (strcmp(name, "write") == 0)
	{
		("Description:Used to write data into regular file\n");
		printf("Usage:write FILE_NAME");
	}
	else if (strcmp(name, "ls") == 0)
	{
		("Description:Used to list all info of files\n");
		printf("Usage:ls\n");
	}
	else if (strcmp(name, "stat") == 0)
	{
		("Description:Used to display info of al file\n");
		printf("Usage:stat FILE_NAME\n");
	}
	else if (strcmp(name, "fstat") == 0)
	{
		("Description:Used to display info of a file\n");
		printf("Usage:fstat FILE_DESCRIPTOR\n");
	}
	else if (strcmp(name, "truncate") == 0)
	{
		("Description:Used to remove data from file\n");
		printf("Usage:truncate FILE_NAME\n");
	}
	else if (strcmp(name, "open") == 0)
	{
		("Description:Used to open existing file\n");
		printf("Usage:open FILE_NAME MODE\n");
	}
	else if (strcmp(name, "close") == 0)
	{
		("Description:Used to close opened file\n");
		printf("Usage:close FILE_NAME\n");
	}
	else if (strcmp(name, "closeall") == 0)
	{
		("Description:Used to close all open files\n");
		printf("Usage:closeall\n");
	}
	else if (strcmp(name, "lseek") == 0)
	{
		("Description:Used to change the offset of file\n");
		printf("Usage:lseek FILE_NAME CHANGE_IN_OFFSET START_POINT\n");
	}
	else if (strcmp(name, "rm") == 0)
	{
		("Description:Used to delete the file\n");
		printf("Usage:rm FILE_NAME\n");
	}
	else
	{
		printf("ERROR\n");
	}
}
void DisplayHelp()
{
	printf("ls:to list out all files\n");
	printf("clear:to clear console\n");
	printf("open:to open the file\n");
	printf("close:to close the file\n");
	printf("closall:to close all opened files\n");
	printf("read:read contetnd of a file\n");
	printf("write:write contents to a file\n");
	printf("stat:to display info of file using name\n");
	printf("fstat:to display info of file using fd");
	printf("truncate:to remove data from file\n");
	printf("rm:to delete a file\n");
}
int GetFDFromName(char * name)
{
	int i = 0;
	while (i < MAXINODE)
	{
		if (ufdtarr[i].ptrfiletable != NULL)
		{
			if (_stricmp((ufdtarr[i].ptrfiletable->ptrinode->filename), name) == 0)
			{
				break;
			}
		}
		i++;
	}
	if (i == 50)
	{
		return -1;

	}
	else
	{
		return i;
	}
}
PINODE Get_Inode(char * name)
{
	PINODE temp = head;
	int i = 0;
	if (name == NULL)
	{
		return NULL;

	}
	while (temp != NULL)
	{
		if (strcmp(name, temp->filename) == 0)
		{
			break;
		}
		temp = temp->next;
	}
	return temp;
}
void InitializeSuperBlock()
{
	int i = 0;
	while (i < MAXINODE)
	{
		ufdtarr[i].ptrfiletable = NULL;
		i++;
	}
	superblockobj.totalinodes = MAXINODE;
	superblockobj.freeinodes = MAXINODE;

}
void CreateDILB()
{
	int i = 1;
	PINODE newn = NULL;
	PINODE temp = NULL;
	while (i <= MAXINODE)
	{
		newn = (PINODE)malloc(sizeof(INODE));
		newn->linkcount = newn->referencecount = 0;
		newn->filetype = newn->filesize = 0;
		newn->buffer = NULL;
		newn->next = NULL;
		newn->inodenumber = i;
		if (temp == NULL)
		{
			head = newn;
			temp = head;

		}
		else
		{
			temp->next = newn;
			temp = temp->next;
		}
		i++;
		
	}
	//printf("DILB CREATED SUCCESSFULLY");
}
int CreateFile(char * name, int permission)
{
	int i = 0;
	PINODE temp = head;
	if (name == NULL || permission == 0 || permission > 3)
	{
		return -1;
	}
	if (superblockobj.freeinodes == 0)
	{
		return -2;
	}
	if (Get_Inode(name) != NULL)
	{
		return -3;
	}
	superblockobj.freeinodes--;
	while (temp != NULL)
	{
		if (temp->filetype == 0)
		{
			break;
		}
		temp = temp->next;
	}
	while (i < 50)
	{
		if (ufdtarr[i].ptrfiletable == NULL)
		{
			break;
		}
		i++;
	}
	ufdtarr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
	if (ufdtarr[i].ptrfiletable == NULL)
	{
		return -4;
	}
	ufdtarr[i].ptrfiletable->count = 1;
	ufdtarr[i].ptrfiletable->mode = permission;
	ufdtarr[i].ptrfiletable->readoffset = 0;
	ufdtarr[i].ptrfiletable->writeoffset = 0;
	ufdtarr[i].ptrfiletable->ptrinode = temp;
	strcpy(ufdtarr[i].ptrfiletable->ptrinode->filename, name);
	ufdtarr[i].ptrfiletable->ptrinode->filetype= REGULAR;
	ufdtarr[i].ptrfiletable->ptrinode->referencecount = 1;
	ufdtarr[i].ptrfiletable->ptrinode->linkcount = 1;
	ufdtarr[i].ptrfiletable->ptrinode->filesize = MAXFILESIZE;
	ufdtarr[i].ptrfiletable->ptrinode->fileactualsize = 0;
	ufdtarr[i].ptrfiletable->ptrinode->permission = permission;
	ufdtarr[i].ptrfiletable->ptrinode->buffer = (char *)malloc(MAXFILESIZE);
	memset(ufdtarr[i].ptrfiletable->ptrinode->buffer, 0, 1024);

	return i;
}
int rm_File(char * name)
{
	int fd = 0;
	fd = GetFDFromName(name);
	if (fd == -1)
	{
		return -1;
	}
	(ufdtarr[fd].ptrfiletable->ptrinode->linkcount)--;
	if (ufdtarr[fd].ptrfiletable->ptrinode->linkcount == 0)
	{
		ufdtarr[fd].ptrfiletable->ptrinode->filetype = 0;
		free(ufdtarr[fd].ptrfiletable);
	}
	ufdtarr[fd].ptrfiletable = NULL;
	(superblockobj.freeinodes)++;
}
int ReadFile(int fd, char * arr, int isize)
{
	int read_size = 0;
	if (ufdtarr[fd].ptrfiletable == NULL)
	{
		return -1;
	}
	if (ufdtarr[fd].ptrfiletable->mode != READ && ufdtarr[fd].ptrfiletable->mode != READ + WRITE)
	{
		return -2;
	}
	if (ufdtarr[fd].ptrfiletable->ptrinode->permission != READ && ufdtarr[fd].ptrfiletable->ptrinode->permission != READ + WRITE)
	{
		return -2;

	}
	if (ufdtarr[fd].ptrfiletable->readoffset == ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize)
	{
		return -3;
	}
	if (ufdtarr[fd].ptrfiletable->ptrinode->filetype != REGULAR)
	{
		return -4;
	}
	read_size = ((ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize)) - (ufdtarr[fd].ptrfiletable->readoffset);
	if (read_size < isize)
	{
		strncpy(arr, (ufdtarr[fd].ptrfiletable->ptrinode->buffer) + (ufdtarr[fd].ptrfiletable->readoffset), read_size);
		ufdtarr[fd].ptrfiletable->readoffset = ufdtarr[fd].ptrfiletable->readoffset + read_size;
	}
	else
	{
		strncpy(arr, (ufdtarr[fd].ptrfiletable->ptrinode->buffer) + (ufdtarr[fd].ptrfiletable->readoffset), isize);
		(ufdtarr[fd].ptrfiletable->readoffset) = ufdtarr[fd].ptrfiletable->readoffset + isize;
	}
	return isize;
}
int WriteFile(int fd, char * arr, int isize)
{
	if (((ufdtarr[fd].ptrfiletable->mode) != WRITE) && ((ufdtarr[fd].ptrfiletable->mode) != READ + WRITE))
	{
		return -1;
	}
	if (((ufdtarr[fd].ptrfiletable->ptrinode->permission) != WRITE) && ((ufdtarr[fd].ptrfiletable->ptrinode->permission) != READ + WRITE))
	{
		return -1;
	}
	if ((ufdtarr[fd].ptrfiletable->writeoffset) == MAXFILESIZE)
	{
		return -2;

	}
	if (ufdtarr[fd].ptrfiletable->ptrinode->filetype != REGULAR)
	{
		return -3;
	}
	strncpy((ufdtarr[fd].ptrfiletable->ptrinode->buffer) + (ufdtarr[fd].ptrfiletable->writeoffset), arr, isize);
	(ufdtarr[fd].ptrfiletable->writeoffset) = (ufdtarr[fd].ptrfiletable->writeoffset) + isize;
	(ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize) = (ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize) + isize;
	return isize;
}
int OpenFile(char * name, int mode)
{
	int i = 0;
	PINODE temp = NULL;
	if (name == NULL || mode <= 0)
	{
		return -1;
	}
	temp = Get_Inode(name);
	if (temp == NULL)
	{
		return -2;
	}
	if (temp->permission < mode)
	{
		return -3;
	}
	while (i < 50)
	{
		if (ufdtarr[i].ptrfiletable == NULL)
		{
			break;
		}
		i++;
	}
	ufdtarr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
	if (ufdtarr[i].ptrfiletable == NULL)
	{
		return -1;
	}
	ufdtarr[i].ptrfiletable->count = 1;
	ufdtarr[i].ptrfiletable->mode = mode;
	if (mode == READ + WRITE)
	{
		ufdtarr[i].ptrfiletable->readoffset = 0;
		ufdtarr[i].ptrfiletable->writeoffset = 0;
	}
	else if (mode == READ)
	{
		ufdtarr[i].ptrfiletable->readoffset = 0;
	}
	else if (mode == WRITE)
	{
		ufdtarr[i].ptrfiletable->writeoffset = 0;
	}
	ufdtarr[i].ptrfiletable->ptrinode = temp;
	(ufdtarr[i].ptrfiletable->ptrinode->referencecount)++;

	return i;
}
void CloseFileByName(int fd)
{
	ufdtarr[fd].ptrfiletable->readoffset = 0;
	ufdtarr[fd].ptrfiletable->writeoffset = 0;
	(ufdtarr[fd].ptrfiletable->ptrinode->referencecount)--;
}
int CloseFileByName(char * name)
{
	int i = 0;
	i = GetFDFromName(name);
	if (i == -1)
	{
		return -1;
	}
	ufdtarr[i].ptrfiletable->readoffset = 0;
	ufdtarr[i].ptrfiletable->writeoffset = 0;
	(ufdtarr[i].ptrfiletable->ptrinode->referencecount)--;
	return 0;
}
void CloseAllFile()
{
	int i = 0;
	while (i < 50)
	{
		if (ufdtarr[i].ptrfiletable != NULL)
		{
			ufdtarr[i].ptrfiletable->readoffset = 0;
			ufdtarr[i].ptrfiletable->writeoffset = 0;
			(ufdtarr[i].ptrfiletable->ptrinode->referencecount)--;
			break;
		}
		i++;
	}
}
int LseekFile(int fd, int size, int from)
{
	if ((fd < 0) || (from > 2))
	{
		return -1;
	}
	if (ufdtarr[fd].ptrfiletable == NULL)
	{
		return -1;
	}
	if ((ufdtarr[fd].ptrfiletable->mode == READ) || (ufdtarr[fd].ptrfiletable->mode == READ + WRITE))
	{
		if (from == CURRENT)
		{
			if (((ufdtarr[fd].ptrfiletable->readoffset) + size) > ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize)
			{
				return -1;
			}
			if (((ufdtarr[fd].ptrfiletable->readoffset) + size) < 0)
			{
				return -1;
			}
			(ufdtarr[fd].ptrfiletable->readoffset) = (ufdtarr[fd].ptrfiletable->readoffset) + size;
		}
		else if (from == START)
		{
			if (size > (ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize))
			{
				return -1;
			}
			if (size < 0)
			{
				return -1;
			}
			ufdtarr[fd].ptrfiletable->readoffset = size;
		}
		else if (from == END)
		{
			if ((ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize) + size > MAXFILESIZE)
			{
				return -1;
			}
			if ((ufdtarr[fd].ptrfiletable->readoffset + size) < 0)
			{
				return -1;
			}
			ufdtarr[fd].ptrfiletable->readoffset = (ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize) + size;
		}
	}
	else if (ufdtarr[fd].ptrfiletable->mode == WRITE)
	{
		if (from == CURRENT)
		{
			if (((ufdtarr[fd].ptrfiletable->writeoffset) + size) > MAXFILESIZE)
			{
				return -1;
			}
			if (((ufdtarr[fd].ptrfiletable->writeoffset) + size) < 0)
			{
				return -1;
			}
			if (((ufdtarr[fd].ptrfiletable->writeoffset) + size) > (ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize))
			{
				ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize = (ufdtarr[fd].ptrfiletable->writeoffset) + size;
			}
			ufdtarr[fd].ptrfiletable->writeoffset = (ufdtarr[fd].ptrfiletable->writeoffset) + size;
		}
		else if(from==START)
		{
			if (size > MAXFILESIZE)
			{
				return -1;
			}
			if (size < 0)
			{
				return -1;
			}
			if (size > (ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize))
			{
				ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize = size;
			}
			(ufdtarr[fd].ptrfiletable->writeoffset) = size;
		}
		else if (from == END)
		{
			if ((ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize) + size > MAXFILESIZE)
			{
				return -1;

			}
			if (((ufdtarr[fd].ptrfiletable->writeoffset) + size) < 0)
			{
				return -1;
			}
			(ufdtarr[fd].ptrfiletable->writeoffset) = (ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize) + size;
		
		}
	}
}
void ls_file()
{
	int i = 0;
	PINODE temp = head;
	if (superblockobj.freeinodes == MAXINODE)
	{
		printf("ERROR:there are no files\n");
		return;
	}
	printf("\nfile name\tinode no\tFile Size\tLinkCount\n");
	printf("-----------------------------------------------------------------------\n");
	while (temp != NULL)
	{
		if (temp->filetype != 0)
		{
			printf("%s\t\t%d\t\t%d\t\t%d\n", temp->filename, temp->inodenumber, temp->fileactualsize, temp->linkcount);
		}
		temp = temp->next;
	}
	printf("-----------------------------------------------------------------------\n");
}
int fstat_file(int fd)
{
	PINODE temp = head;
	int i = 0;

	if (fd < 0)
	{
		return -1;
	}
	if (ufdtarr[fd].ptrfiletable == NULL)
	{
		return -2;
	}
	temp = ufdtarr[fd].ptrfiletable->ptrinode;
	printf("------Statistical inf of file-----------\n");
	printf("File name:%s\n", temp->filename);
	printf("inode no:%d\n", temp->inodenumber);
	printf("File size:%d\n", temp->filesize);
	printf("Actual File Size:%d\n", temp->fileactualsize);
	printf("Link Count:%d\n", temp->linkcount);
	printf("reference count:%d\n", temp->referencecount);

	if (temp->permission == 1)
	{
		printf("File Permission:Read Only\n");

	}
	else if (temp->permission == 2)
	{
		printf("File Permission:Write\n");

	}
	else if (temp->permission == 3)
	{
		printf("File Permission:Read and Write\n");

	}
	return 0;
}
int stat_file(char * name)
{
	PINODE temp = head;
	int i = 0;
	if (name == NULL)
	{
		return -1;
	}
	while (temp != NULL)
	{
		if (strcmp(name, temp->filename) == 0)
		{
			break;
		}
		temp = temp->next;
	}
	if (temp == NULL)
	{
		return -2;
	}
	printf("------Statistical inf of file-----------\n");
	printf("File name:%s\n", temp->filename);
	printf("inode no:%d\n", temp->inodenumber);
	printf("File size:%d\n", temp->filesize);
	printf("Actual File Size:%d\n", temp->fileactualsize);
	printf("Link Count:%d\n", temp->linkcount);
	printf("reference count:%d\n", temp->referencecount);

	if (temp->permission == 1)
	{
		printf("File Permission:Read Only\n");

	}
	else if (temp->permission == 2)
	{
		printf("File Permission:Write\n");

	}
	else if (temp->permission == 3)
	{
		printf("File Permission:Read and Write\n");

	}
	return 0;
}
int truncate_file(char * name)
{
	int fd = GetFDFromName(name);
	if (fd == -1)
	{
		return -1;
	}
	memset(ufdtarr[fd].ptrfiletable->ptrinode->buffer, 0, 1024);
	ufdtarr[fd].ptrfiletable->readoffset = 0;
	ufdtarr[fd].ptrfiletable->writeoffset = 0;
	ufdtarr[fd].ptrfiletable->ptrinode->fileactualsize = 0;
}
int main()
{
	char * ptr = NULL;
	int ret = 0, fd = 0, count = 0;
	char command[4][80], str[80], arr[1024];
	InitializeSuperBlock();
	CreateDILB();

	while (1)
	{
		fflush(stdin);
		strcpy_s(str, "");
		printf("\nVFS:> ");
		
		fgets(str, 80, stdin);
		count = sscanf(str, "%s %s %s %s", command[0], command[1], command[2], command[3]);

		if (count == 1)
		{
			if (strcmp(command[0], "ls") == 0)
			{
				ls_file();
			}
			else if (strcmp(command[0], "closeall") == 0)
			{
				CloseAllFile();
				printf("all files are closed\n");
				continue;
			}
			else if (strcmp(command[0], "clear") == 0)
			{
				system("cls");
				continue;
			}
			else if (strcmp(command[0], "help") == 0)
			{
				DisplayHelp();
				continue;
			}
			else if (strcmp(command[0], "exit") == 0)
			{
				printf("Terminating vfs\n");
				break;
			}
			else
			{
				printf("ERROR:command not found\n");
				continue;
			}
		}
		else if (count == 2)
		{
			if (strcmp(command[0], "stat") == 0)
			{
				ret = stat_file(command[1]);
				if (ret == -1)
				{
					printf("ERROR:incorrect parameters\n");
				}
				if (ret == -2)
				{
					printf("ERROR:there is no such file\n");
				}
				continue;
			}
			else if (strcmp(command[0], "fstat") == 0)
			{
				ret = fstat_file(atoi(command[1]));
				if (ret == -1)
				{
					printf("ERROR:incorrect parameters\n");
				}
				if (ret == -2)
				{
					printf("ERROR:there is no such file\n");
				}
				continue;
			}

			else if (strcmp(command[0], "close") == 0)
			{
				ret = CloseFileByName(command[1]);
				if (ret == -1)
				{
					printf("ERROR:there is no such file\n");
				}
				continue;
			}

			else if (strcmp(command[0], "rm") == 0)
			{
				ret = rm_File(command[1]);
				if (ret == -1)
				{
					printf("ERROR:there is no such file\n");
				}
				continue;
			}
			else if (strcmp(command[0], "man") == 0)
			{
				man(command[1]);
			}
			else if (strcmp(command[0], "write") == 0)
			{
				fd = GetFDFromName(command[1]);
				if (fd == -1)
				{
					printf("ERROR:Incorrect Parameter\n");
					continue;
				}
				printf("enter data\n");
				scanf("%[^'\n']s", arr);
				
				ret = strlen(arr);
				
				if (ret == 0)
				{
					printf("ERROR:Incorrect Parameter\n");

				}
				ret = WriteFile(fd, arr, ret);
				if (ret == -1)
				{
					printf("ERROR:Permission Denied\n");

				}
				if (ret == -2)
				{
					printf("ERROR:There is no suffecient memory to write\n");

				}
				if (ret == -3)
				{
					printf("ERROR:It is not a regular File\n");

				}
				
			}
			else if (strcmp(command[0], "truncate") == 0)
			{
				ret = truncate_file(command[1]);
				if (ret == -1)
				{
					printf("ERROR:Incorrect Parameter\n");

				}
			}
			else 
			{
				printf("ERROR:Command not found\n");
				continue;
			}
		}
		else if (count == 3)
		{
			if (_stricmp(command[0], "create") == 0)
			{
				ret = CreateFile(command[1], atoi(command[2]));
				if(ret >= 0)
				{
					printf("File Suceesfully created with fd %d\n", ret);
				}
				if (ret == -1)
				{
					printf("ERROR:Incorrect Parameter\n");
				}
				if (ret == -2)
				{
					printf("ERROR:There are no inodes\n");
				}
				if (ret == -3)
				{
					printf("ERROR:file already exists\n");
				}
				if (ret == -4)
				{
					printf("ERROR:Memory allocation failure\n");
				}
			}
			else if (_stricmp(command[0], "open") == 0)
			{
				ret = OpenFile(command[1], atoi(command[2]));
				if (ret >= 0)
				{
					printf("File Suceesfully opened with fd %d\n", ret);
				}
				if (ret == -1)
				{
					printf("ERROR:Incorrect Parameter\n");
				}
				if (ret == -2)
				{
					printf("ERROR:file not present\n");
				}
				if (ret == -3)
				{
					printf("ERROR:permission denied\n");
				}
				
			}
			else if (_stricmp(command[0], "read") == 0)
			{
				fd = GetFDFromName(command[1]);
				if (fd == -1)
				{
					printf("ERROR:Incorrect Parameter\n");
				}
				ptr = (char*)malloc(sizeof(atoi(command[2])) + 1);
				if (ptr == NULL)
				{
					printf("ERROR:Memory allocation failure\n");
				}
				ret = ReadFile(fd,ptr,atoi(command[2]));
				
				if (ret == -1)
				{
					printf("ERROR:File not existing\n");
				}
				if (ret == -2)
				{
					printf("ERROR:permission denied\n");
				}
				if (ret == -3)
				{
					printf("ERROR:Reached at end of file\n");
				}
				if (ret == -4)
				{
					printf("ERROR:it is not regular file\n");
				}
				if (ret == 0)
				{
					printf("ERROR:File Empty\n");
				}
				if (ret > 0)
				{
					_write(2, ptr, ret);
				}
				continue;
			}
			else
			{
				printf("\nERROR:Command not found!!!\n");
				continue;
			}
		}
		else if (count == 4)
		{
			if (_stricmp(command[0], "lseek") == 0)
			{
				fd = GetFDFromName(command[1]);
				if (fd == -1)
				{
					printf("ERROR:Incorrect Parameter\n");
					continue;
				}
				ret = LseekFile(fd, atoi(command[2]), atoi(command[3]));
				if (ret == -1)
				{
					printf("ERROR:unable to perform lseek\n");
				}
			}
			else
			{
				printf("ERROR:Command not found!!!\n");
				continue;
			}
	    }
		else
		{
			printf("ERROR:Command not found!!!\n");
			continue;
		}
	}
	return 0;
}
