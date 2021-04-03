#ifndef _KERNEL_VFS_H
#define _KERNEL_VFS_H

#include <stdint.h>
#include <stddef.h>

#define FS_FILE         0x01
#define FS_DIRECOTRY    0x02
#define FS_CHARDEVICE   0x03
#define FS_BLOCKDEVICE  0x04
#define FS_PIPE         0x05
#define FS_SYMLINK      0x06
#define FS_MOUNTPOINT   0x08

struct fs_node;

typedef struct
{
    char name[128];
    uint32_t inode;
} dirent_t;

typedef uint32_t (*read_type_t)(struct fs_node*, uint32_t, size_t, uint8_t*);
typedef uint32_t (*write_type_t)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef dirent_t* (*readdir_type_t)(struct fs_node*, uint32_t);
typedef struct fs_node* (*finddir_type_t)(struct fs_node*, char*);

typedef struct fs_node
{
    char name[128];
    uint32_t inode;
    uint32_t flags;
    uint32_t mask;
    uint32_t uid;
    uint32_t gid;
    uint32_t length;
    uint32_t impl;
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    struct fs_node* ptr;
} fs_node_t;

extern fs_node_t* fs_root;

uint32_t read_fs(fs_node_t*, uint32_t, size_t, uint8_t*);
uint32_t write_fs(fs_node_t*, uint32_t, size_t, uint8_t*);
void open_fs(fs_node_t*, uint8_t, uint8_t);
void close_fs(fs_node_t*);
dirent_t* readdir_fs(fs_node_t*, uint32_t);
fs_node_t* finddir_fs(fs_node_t*, char*);

#endif