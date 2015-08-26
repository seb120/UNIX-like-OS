#ifndef _FILESYSYSTEM_H
#define _FILESYSYSTEM_H

#include "lib.h"
#include "multiboot.h"



#define ELF_HEADER_SIZE 	52
#define BLOCK_SIZE			4096
#define MAX_BLOCKS_INODE 	1023
#define MAX_FNAME_LENGTH	32
#define FD_MIN_IDX		2
#define FD_MAX_IDX		7

#define fourBytes 4;
#define eightBytes 8;
#define FILEIMAGEOFFSET 0x00048000;
#define blank_param 5
#define	IMAGE_START	0x8048000

#define FILE_TYPE_RTC	0
#define FILE_TYPE_DIR	1
#define FILE_TYPE_REG	2

#define FILE_IN_USE			1
#define FILE_NOT_IN_USE 	0

//director entry struct -- size 64B
typedef struct dentry_t
{
	uint8_t fname[MAX_FNAME_LENGTH];
	uint32_t ftype;
	uint32_t inode;
	uint8_t reserved[24];
	
} dentry_t;

//index node struct -- size 4kB
typedef struct inodes_t
{
	uint32_t length;	//length in bytes
	uint32_t data_blocks[MAX_BLOCKS_INODE];
}inodes_t;

//data block struct -- size 4kB
typedef struct data_blocks
{
	uint8_t data_blocks[BLOCK_SIZE];
}data_blocks_t;

//boot block struct (first black of file system) -- size 4kB
typedef struct boot_block
{
	uint32_t num_dir_entries;
	uint32_t num_inodes;
	uint32_t num_data_blocks;
	uint8_t reserved[52];
	dentry_t dir_entries[63];
}boot_block_t;

typedef uint32_t (*openfuncptr)(const uint8_t* filename); //open
typedef uint32_t (*closefuncptr)(uint32_t fd);	//close
typedef uint32_t (*readfuncptr)(uint32_t fd, void* buf, int32_t nbytes); //read
typedef uint32_t (*writefuncptr)(uint32_t fd, const void* buf, int32_t nbytes); //write

typedef struct file_op_ptr
{
	openfuncptr openptr;
	closefuncptr closeptr;
	readfuncptr readptr;
	writefuncptr writeptr;

}file_op_ptr_t;


file_op_ptr_t file_ops;
file_op_ptr_t rtc_ops;
file_op_ptr_t terminal_ops;
file_op_ptr_t stdin_ops;
file_op_ptr_t stdout_ops;

typedef struct fs_descriptor_t
{
	file_op_ptr_t* file_ops;
	int32_t* inode_ptr;
	int32_t file_position;
	int32_t flags;//1-in use
} fs_descriptor_t;

// Struct of a ELF Format header for identifying executables
typedef struct file_header_t {
    union {
        uint8_t data[ELF_HEADER_SIZE];
        struct {
            uint8_t magic[4];
            uint8_t e_ident[12];
            uint16_t e_type;
            uint16_t e_machine;
            uint32_t e_version;

			uint32_t* entry;
            
            uint32_t e_phoff;
            uint32_t e_shoff;
            uint32_t e_flags;

            //12 Bytes
            uint16_t e_ehsize;
            uint16_t e_phentsize;
            uint16_t e_phnum;
            uint16_t e_shentsize;
            uint16_t e_shnum;
            uint16_t e_shstrndx;



        } __attribute__((packed));
    };
} file_header_t;


extern void fs_init();
extern int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
extern int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
extern uint32_t read_data_one(uint32_t fd, void* buf, uint32_t nbytes);

extern uint32_t open_dir(const uint8_t *fname);
extern uint32_t close_dir();
extern uint32_t read_dir(uint32_t fd, void *buf, int32_t nbytes);
extern uint32_t write_dir();

extern uint32_t read_file(uint32_t fd, void *buf, int32_t nbytes);
extern uint32_t write_file(uint32_t fd, const void* buf, int32_t nbytes);
extern uint32_t open_file(const uint8_t *fname);
extern uint32_t close_file(uint32_t fd);


extern uint32_t copy_file_image(uint8_t* fname);


boot_block_t* bootblock;
dentry_t dentry; 
unsigned int fs_start;
inodes_t* inodes_array;
data_blocks_t* data_array;
fs_descriptor_t* fs_descriptor;


#endif

