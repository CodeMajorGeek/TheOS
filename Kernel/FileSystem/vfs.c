#include <Kernel/FileSystem/initrd.h>

fs_node_t* fs_root = 0;

uint32_t read_fs(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buffer)
{
    if (node->read != 0)
        return node->read(node, offset, size, buffer);
    return 0;
}

uint32_t write_fs(fs_node_t* node, uint32_t offset, size_t size, uint8_t* buffer)
{
    if (node->write != 0)
        return node->write(node, offset, size, buffer);
    return 0;
}

void open_fs(fs_node_t* node, uint8_t read, uint8_t write)
{
    if (node->open != 0)
        return node->open(node);
}

void close_fs(fs_node_t* node)
{
    if (node->close != 0)
        return node->close(node);
}

dirent_t* readdir_fs(fs_node_t* node, uint32_t index)
{
    if ((node->flags & 0x7) == FS_DIRECOTRY && node->readdir != 0)
        return node->readdir(node, index);
    return 0;
}

fs_node_t* finddir_fs(fs_node_t* node, char* name)
{
    if ((node->flags & 0x7) == FS_DIRECOTRY && node->readdir != 0)
        return node->finddir(node, name);
    return 0;
}