#include "emufs.h"
#include <string.h>
#include <time.h>

/**************************** File system helper functions ****************************/

//	Function to encrypt a block of data 
int encrypt(char* input, char* encrypted)
{
	for(int i=0; i<BLOCKSIZE; i++)
	{
		encrypted[i] = ~input[i];
	}
}

//	Function to decrypt a block of data 
int decrypt(char* input, char* decrypted)
{
	for(int i=0; i<BLOCKSIZE; i++)
	{
		decrypted[i] = ~input[i];
	}
}

//	The following helper functions are used to read and write 
//	superblock and metadata block. 
//	Change the function definitions with required arguments
struct superblock_t* readSuperblock(struct mount_t* mount_point)
{
	/*
		* Read 0th block from the device into a blocksize buffer
		* Create superblock_t variable and fill it using reader buffer
		* Return the superblock_t variable
	*/
	if(mount_point->device_fd <= 0)
		printf("No device associated with given mount point\n");

	int num_read;
	char *buf = (char*) malloc(BLOCKSIZE);
	num_read = readblock(mount_point->device_fd, 0,buf);
	if(num_read < 0 )
		printf("Error in reading block\n");
	return (struct superblock_t *) buf;

}

int writeSuperblock(struct mount_t* mount_point, struct superblock_t* st)
{
	/*
		* Read the 0th block from device into a buffer
		* Write the superblock into the buffer
		* Write back the buffer into block 0
	*/
	if(mount_point->device_fd <= 0)
		printf("No device associated with given mount point\n");
	char* buf = (char*) malloc(BLOCKSIZE);
	buf = (char*) st;
	int ret = writeblock(mount_point->device_fd, 0, buf);
	return ret;

}

struct metadata_t* readMetadata(struct mount_t* mount_point)
{
	// Same as readSuperBlock(), but it is stored on block 1
	// Need to decrypt if emufs-encrypted is used
	if(mount_point->device_fd <= 0)
		printf("No device associated with given mount point\n");

	int num_read;
	char *buf = (char*) malloc(BLOCKSIZE);
	num_read = readblock(mount_point->device_fd, 1, buf);
	if(num_read < 0 )
		printf("Error in reading block\n");

	char *decrypted = (char*) malloc(BLOCKSIZE);
	if(mount_point->fs_number == 1){
		decrypt(buf,decrypted);
		return (struct metadata_t*) decrypted;
	}
	else
		return (struct metadata_t*) buf;
}

int writeMetadata(struct mount_t* mount_point, struct metadata_t* mt)
{
	// Same as writeSuperblock(), but it is stored on block 1
	// Need to decrypt/encrypt if emufs-encrypted is used 
	if(mount_point->device_fd <= 0)
		printf("No device associated with given mount point\n");
	char* buf = (char*) malloc(BLOCKSIZE);
	buf = (char*) mt;
	char * encrypted = (char*) malloc(BLOCKSIZE);
	if(mount_point->fs_number == 1){
		encrypt(buf, encrypted);
		return writeblock(mount_point->device_fd, 1, encrypted);
		//return writeblock(mount_point->device_fd, 1, buf);
	}
	else
		return writeblock(mount_point->device_fd, 1, buf);
}

/**************************** File system API ****************************/

int create_file_system(struct mount_t *mount_point, int fs_number)
{
	/*
	   	* Read the superblock.
	    * Set file system number on superblock
		* Clear the bitmaps.  values on the bitmap will be either '0', or '1', or'x'. 
		* Create metadata block in disk
		* Write superblock and metadata block back to disk.

		* Return value: -1,		error
						 1, 	success
	*/

	struct superblock_t* st = readSuperblock(mount_point);
	st->fs_number = fs_number;
	mount_point->fs_number = fs_number;
	st->bitmap[0] = '1';
	st->bitmap[1] = '1';
	for(int i=2;i<MAX_BLOCKS;i++){
		if(i >= st->disk_size)
			st->bitmap[i] = 'x';
		else
			st->bitmap[i] = '0';
	}

	struct metadata_t* mt = readMetadata(mount_point);
	for(int i=0;i<MAX_FILES;i++){
		(mt->inodes[i]).status = 0;
	}

	int ret1, ret2;
	ret1 = writeSuperblock(mount_point,st);
	if(ret1 < 0)
		return ret1;
	ret2 = writeMetadata(mount_point, mt);
	if(ret2 < 0)
		return ret2;

	return 1;
}


struct file_t* eopen(struct mount_t* mount_point, char* filename)
{
	/* 
		* If file exist, get the inode number. inode number is the index of inode in the metadata.
		* If file does not exist, 
			* find free inode.
			* allocate the free inode as USED
			* if free id not found, print the error and return -1
		* Create the file hander (struct file_t)
		* Initialize offset in the file hander
		* Return file handler.

		* Return NULL on error.
	*/

	//Check if file name exists
	struct metadata_t *mt;
	mt = readMetadata(mount_point);
	int found = -1;
	int i;

	for(int i=0;i<MAX_FILES;i++){
		if((mt->inodes[i]).status == 0)
			continue;
		//Modify line below
		if(strcmp((mt->inodes[i]).name, filename) == 0){
			found = i;
			break;
		}
	}


	if(found == -1){
		for(int i=0;i<MAX_FILES;i++){
			if(mt->inodes[i].status == 0){
				mt->inodes[i].status = 1;
				strcpy(mt->inodes[i].name, filename);
				mt->inodes[i].file_size = 0;
				for(int j=0;j<4;j++)
					mt->inodes[i].blocks[j] = -1;
				mt->inodes[i].modtime = time(NULL);
				found = i;
				break;
			}
		}
	}

	if(found == -1){
		printf("No free inode found\n");
		return NULL;
	}

	
	struct file_t *ft = (struct file_t*) malloc(sizeof(struct file_t));
	ft->offset = 0;
	ft->inode_number = found;
	ft->mount_point = mount_point;

	int ret = writeMetadata(mount_point, mt);
	if(ret < 0)
		return NULL;

	printf("File '%s' Opened \n", filename);

	return ft;
	
}

int ewrite(struct file_t* file, char* data, int size)
{
	// You do not need to implement partial writes in case file exceeds 4 blocks
	// or no free block is available in the disk. 

	// Return value: 	-1,  error
	//					Number of bytes written

	struct mount_t* mount_point = file->mount_point;
	int inode_number = file->inode_number;
	int offset = file->offset;
	int i=0, j=0;

	struct metadata_t* mt = readMetadata(mount_point);
	struct superblock_t* st = readSuperblock(mount_point);

	int start_point = (file->offset)/BLOCKSIZE;
	int nblocks = size/BLOCKSIZE;

	int num_written = 0;

	if(nblocks + start_point > 4){
		printf("ERROR: Maximum File Size Exceeded\n");
		return -1;
	}

	//Error Checking
	int req = 0;
	int avail = 0;

	for(i=0;i<nblocks;i++){
		if((mt->inodes[inode_number]).blocks[i + start_point ] == -1)
			req++;
	}

	for(int j=0;j<MAX_BLOCKS;j++){
		if(st->bitmap[j] == '0')
			avail++;
	}

	if(avail < req){
		printf("ERROR: Out of diskspace\n");
		return -1;
	}

	
	for(i=0;i<nblocks;i++){
		if((mt->inodes[inode_number]).blocks[i + start_point ] == -1){
			for(j=0;j<MAX_BLOCKS;j++){
				if(st->bitmap[j] == '0'){
					st->bitmap[j] = '1';
					mt->inodes[inode_number].blocks[i+start_point] = j;
					break;
				}
			}
		}
		if(st->fs_number == 1){
			char * encrypted = (char*) malloc(BLOCKSIZE);
			if(st->fs_number == 1)
				encrypt(data + (BLOCKSIZE * i), encrypted);
			writeblock(mount_point->device_fd, mt->inodes[inode_number].blocks[i+start_point], encrypted);	
		}
		else
			writeblock(mount_point->device_fd, mt->inodes[inode_number].blocks[i+start_point], data + (BLOCKSIZE * i));
		file->offset = file->offset + BLOCKSIZE;
		mt->inodes[inode_number].file_size = mt->inodes[inode_number].file_size + BLOCKSIZE;
		num_written += BLOCKSIZE;
	}

	//Set time
	mt->inodes[inode_number].modtime = time(NULL);

	int ret1 = writeMetadata(mount_point, mt);
	if(ret1 < 0)
		return -1;
	int ret2 = writeSuperblock(mount_point, st);
	if(ret2 < 0)
		return -1;

	return num_written;
}

int eread(struct file_t* file, char* data, int size)
{
	// NO partial READS.

	// Return value: 	-1,  error
	//					Number of bytes read
	struct mount_t* mount_point = file->mount_point;
	int inode_number = file->inode_number;
	int offset = file->offset;
	int i=0, j=0;

	struct metadata_t* mt = readMetadata(mount_point);
	struct superblock_t* st = readSuperblock(mount_point);

	int start_point = (file->offset)/BLOCKSIZE;
	int nblocks = size/BLOCKSIZE;

	if(nblocks + start_point > 4){
		printf("Exceeded capacity of 4 blocks\n");
		return -1;
	}

	int num_read = 0;
	int b_num;

	data[0] = '\0';
	
	for(i=0;i<nblocks;i++){
		char* data_temp = (char *) malloc(BLOCKSIZE);
		b_num = (mt->inodes[inode_number]).blocks[i + start_point];
		if(b_num == -1){
			printf("ERROR: Invalid offset\n");
			return -1;
		}
		
		readblock(mount_point->device_fd, b_num, data_temp);
		if(st->fs_number == 1){
			char* decrypted = (char *) malloc(BLOCKSIZE);
			decrypt(data_temp, decrypted);
			strcat(data, decrypted);
		}
		else
			strcat(data, data_temp);
		file->offset = file->offset + BLOCKSIZE;
		num_read += BLOCKSIZE;

	}

	return num_read;
}

void eclose(struct file_t* file)
{
	// free the memory allocated for the file handler 'file'
	printf("File Closed\n");
	free(file);
	return;
}

int eseek(struct file_t *file, int offset)
{
	// Change the offset in file hanlder 'file'

	file->offset = offset;

	return 1;
}

void fsdump(struct mount_t* mount_point)
{

	printf("\n[%s] fsdump \n", mount_point->device_name);
	printf("%-10s %6s \t[%s] \t%s\n", "  NAME", "SIZE", "BLOCKS", "LAST MODIFIED");
	
	struct superblock_t* st = readSuperblock(mount_point);
	struct metadata_t* mt = readMetadata(mount_point);

	for(int i=0;i<MAX_FILES;i++){
		if (mt->inodes[i].status == 1){

			printf("%-10s %6d \t[%d %d %d %d] \t%s", 
				mt->inodes[i].name, 
				mt->inodes[i].file_size,
				mt->inodes[i].blocks[0],
				mt->inodes[i].blocks[1],
				mt->inodes[i].blocks[2],
				mt->inodes[i].blocks[3],
				asctime(localtime(&(mt->inodes[i].modtime))));
		}
	}

	// printf("%-10s %6d \t[%d %d %d %d] \t%s", 
	// 	file anme, 
	// 	file size,
	// 	first block,
	// 	second block,
	// 	third block,
	// 	fourth block,
	// 	asctime(localtime(&(inode->modtime))));

}
