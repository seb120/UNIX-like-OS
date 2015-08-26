#include "filesystem.h"
#include "terminal.h"
#include "paging.h"
#include "pcb.h"


/*
*	fs_init
*		Function:	Initialize the filesystem
*		Inputs:		none
*		Outputs:	none
*		Effect:		file operator function pointers are set
*/
void fs_init()
{
	
	bootblock = (boot_block_t*)fs_start;
	inodes_array=(inodes_t*)bootblock;
	inodes_array++;
	data_array=(data_blocks_t*)inodes_array;
	data_array+=bootblock->num_inodes;
	
	file_ops.openptr=&open_file;
    file_ops.closeptr=&close_file;
    file_ops.readptr=&read_file;
    file_ops.writeptr=&write_file;

} 

/*
*	read_dentry_by_index
*		Function:	Returns for the user the dentry given by an index
*		Inputs:		index, of the directory entries
*		Outputs:	dentry*, the directory entry to be filled
*		Effect:		none
*/
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry)
{	
	*dentry=bootblock->dir_entries[index];
	
	return 0; 
}


/*
*	read_dentry_by_name
*		Function:	Returns for the user the dentry given by a filename
*		Inputs:		fname, charstring of the dentry to search for
*		Outputs:	dentry*, the directory entry to be filled, if found
*		Effect:		Returns -1 for error, if dentry with fname is not found 
*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
{
	int i;
	int found=0;

	for(i=0; i<bootblock->num_dir_entries; i++)
	{
		if(strncmp((int8_t*) fname, (int8_t*) bootblock->dir_entries[i].fname,MAX_FNAME_LENGTH) == 0 )
		{
			*dentry=bootblock->dir_entries[i];
			found=1;
		}
	}

	if(found==0)
	{
		return -1;
	}

	return 0;
}


/*
*	read_data
*		Function:	Read data from a given inode into a given buffer
*		Inputs:		inode, index of the inodes array
*					offset, data block to start filling the buffer from
*					length, in bytes to copy from the filesystem
*		Outputs:	buf*, the data buffer to be filled
*		Effect:		Returns the number of bytes copied into the buffer for success
*					Returns -1 if the user's arguments exceeded the 
*					length of the file described by the inode
*/
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	int i,a; 
	int num_data_blocks=0;
	int bytes_read=0;
	uint8_t* buf_curr=buf;
	uint32_t datablock_number;
	int bytes_left=BLOCK_SIZE-(offset%BLOCK_SIZE);
	int datablock_start=offset/BLOCK_SIZE;
	
	if(offset>0 && offset%BLOCK_SIZE==0)
		datablock_start--;
	
	if(offset+length>inodes_array[inode].length)
		return -1;

	for(a=offset; a<offset+length; a++)
	{
		if((a-1)%BLOCK_SIZE==0)
			num_data_blocks++;
	}

	if(bytes_left!=BLOCK_SIZE)
	{
		datablock_number=inodes_array[inode].data_blocks[datablock_start];
		memcpy(buf_curr, (uint8_t*) (&data_array[datablock_number].data_blocks[offset]), bytes_left);
		datablock_start++;
		num_data_blocks--;
		bytes_read+=bytes_left;
		buf_curr+=bytes_left;
	}
	
	for(i=datablock_start; i<num_data_blocks-1;i++)
	{
		datablock_number=inodes_array[inode].data_blocks[i];

		memcpy(buf_curr, (uint8_t*) &data_array[datablock_number], BLOCK_SIZE);
		buf_curr+=BLOCK_SIZE;
		bytes_read+=BLOCK_SIZE;
	}

	bytes_left=(offset+length)%BLOCK_SIZE;
	datablock_number=inodes_array[inode].data_blocks[datablock_start+num_data_blocks-1];
	
	memcpy(buf_curr, (uint8_t*) &data_array[datablock_number],bytes_left);
	bytes_read+=BLOCK_SIZE;
	
	return bytes_read;
}

/*
*	read_data_one
*		Function:	Read data from a given file descriptor into a given buffer
*		Inputs:		fd, the file descriptor index of the desired file_array[8]
*					n_bytes, in bytes to copy from the filesystem
*		Outputs:	buf*, the data buffer to be filled
*		Effect:		Returns the number of bytes copied into the buffer for success
*/
uint32_t read_data_one(uint32_t fd, void* buf, uint32_t n_bytes)
{
	fs_descriptor_t* fd_cur;
	fd_cur=&current_pcb->file_array[fd];
	uint32_t offset=current_pcb->file_array[fd].file_position;
	uint32_t inode = ((uint32_t)fd_cur->inode_ptr - (uint32_t) bootblock)/BLOCK_SIZE - 1;
	uint32_t cur_datablock=offset/BLOCK_SIZE; //datablock index with respect to particular file
	uint32_t datablock_number=inodes_array[inode].data_blocks[cur_datablock];//datablock index with respect to filesystem
	uint32_t bytes_left=n_bytes;//bytes left to copy
	uint32_t bytes_read=0;//number of bytes read
	uint32_t f_size=inodes_array[inode].length;
	uint32_t cur_offset=offset%BLOCK_SIZE; //offset in current datablock
	int i; //loop index


	for(i=0; i<n_bytes && offset<f_size; i++)
	{
		memcpy(buf,(uint8_t*) (&data_array[datablock_number].data_blocks[cur_offset]), 1);
		offset++;
		cur_offset++;
		bytes_left--;
		bytes_read++;
		buf++;
		current_pcb->file_array[fd].file_position++;
		if(cur_offset%BLOCK_SIZE==0)
		{
			cur_offset=0;
			datablock_number++;
		}

	}
	
	return bytes_read;
}


/*
*	copy_file_image
*		Function:	Copies a file image to physical memory
*		Inputs:		fname, charstring of filename to copy
*		Outputs:	none
*		Effect:		Copies the file to the physical memory specified for image start address
*/
uint32_t copy_file_image(uint8_t* fname)
{
	dentry_t dentry_curr;
	read_dentry_by_name(fname, &dentry_curr);
	uint32_t fsize=inodes_array[dentry_curr.inode].length; 
	uint8_t buf[ inodes_array[dentry_curr.inode].length];
	read_data(dentry_curr.inode, 0, buf, fsize);
	uint8_t* image_start_addr=(uint8_t*) IMAGE_START;
	memcpy(image_start_addr,buf,fsize);
	return 0;
}


/*
*	read_dir()
*		Function:	Prints file types and sizes in directory
*		Inputs:		fd, the file descriptor index of the desired file_array[8]
*		Outputs:	buf*, the data buffer to be filled
*		Effect:		Fills buffer with all filenames in the directory and their size
*/
uint32_t read_dir(uint32_t fd, void *buf, int32_t nbytes)
{
	uint32_t name_length;

	if(current_pcb->file_array[fd].file_position < bootblock->num_dir_entries)
	{
		name_length=strlen( (int8_t *) bootblock->dir_entries[current_pcb->file_array[fd].file_position].fname);
		memcpy(buf, bootblock->dir_entries[current_pcb->file_array[fd].file_position].fname, name_length);
		current_pcb->file_array[fd].file_position++;

		return name_length;
	}
	
	return 0;
}


/*
*	write_dir()
*		Function:	Write system call for filesystem (dir)
*		Effect:		Does nothing, since filesystem is read only.
*/
uint32_t write_dir()
{
	return -1;
}


/*
*	read_file()
*		Function:	Reads regular files
*		Inputs:		fd, the file descriptor index of the desired file_array[8]
*					n_bytes, in bytes to copy from the filesystem
*		Outputs:	buf*, the data buffer to be filled
*		Effect:		Fills buffer with the contents of the file descriptor
*/
uint32_t read_file(uint32_t fd, void *buf, int32_t nbytes)
{
	// Get the file descriptor of the given index
	fs_descriptor_t* fd_cur;
	fd_cur=&current_pcb->file_array[fd];

	// Read as dir if file descriptor has no inode_ptr
	if((fd_cur->inode_ptr)==NULL)
	{
		return read_dir(fd,buf,nbytes);
	}
	else
	{
		return read_data_one(fd, buf, nbytes);
	}
	
	return 0;
}


/*
*	write_file()
*		Function:	Write system call for filesystem (file)
*		Effect:		Does nothing, since filesystem is read only.
*/
uint32_t write_file(uint32_t fd, const void* buf, int32_t nbytes)
{
	return -1;
}


/*
*	open_file()
*		Function:	Checks if a file is valid and opens it
*		Inputs:		fname, charstring of the dentry to search for
*		Effect:		Initializes the file_array[8] depending on the file to be opened
*		Returns:	fd, the index of the newly created file descriptor (success)
*					-1, if we could not find a empty file decriptor in file_array[8]
*/
uint32_t open_file(const uint8_t *fname)
{
	int check_file=read_dentry_by_name(fname, &dentry);

	//checks for valid file
	if (dentry.fname == NULL || check_file == -1)
		{
			return -1;
		}
	
	uint32_t fd = find_free_fd( current_pcb ); 
	if(fd==-1)
		return -1;
	if(dentry.ftype==FILE_TYPE_RTC) //user level access to rtc
	{
		((current_pcb->file_array)[fd]).file_ops = &rtc_ops;
		current_pcb->file_array[fd].inode_ptr=0;
		current_pcb->file_array[fd].file_position=0;
		current_pcb->file_array[fd].flags=FILE_IN_USE;
	}
	else if(dentry.ftype==FILE_TYPE_DIR) //file type is directory
	{
		((current_pcb->file_array)[fd]).file_ops = &file_ops;
		current_pcb->file_array[fd].inode_ptr=0;
		current_pcb->file_array[fd].file_position=0;
		current_pcb->file_array[fd].flags=FILE_IN_USE;
	}
	else if(dentry.ftype==FILE_TYPE_REG) //file type is regular
	{
		((current_pcb->file_array)[fd]).file_ops = &file_ops;
		current_pcb->file_array[fd].inode_ptr=(int32_t*)&inodes_array[dentry.inode];
		current_pcb->file_array[fd].file_position=0;
		current_pcb->file_array[fd].flags=FILE_IN_USE;
	}

	return fd;
}


/*
*	close_file()
*		Function:	Closes the given file
*		Inputs:		fd, the index of the file descriptor to close
*		Effect:		Clears the file_array[8] depending on the given fd index
*		Returns:	0 for success.
*/
uint32_t close_file(uint32_t fd)
{
	current_pcb->file_array[fd].file_ops=0;
	current_pcb->file_array[fd].inode_ptr=0;
	current_pcb->file_array[fd].file_position=0;
	current_pcb->file_array[fd].flags=FILE_NOT_IN_USE;

	return 0;
}


/*
*	close_dir()
*		Function:	Closes a dir
*		Inputs:		none
*		Effect:		none
*		Returns:	0 for success.
*/
uint32_t close_dir()
{
	return 0;
}
