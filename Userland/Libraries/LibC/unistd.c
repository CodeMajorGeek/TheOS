#include <unistd.h>

int execve(const char* file, const char* argv[], const char* envp[])
{
    return NULL;
}

ssize_t read(int fd, void* buf, size_t count)
{
    size_t pos = 0;

    if (fd == STDIN)
    {
        while (pos < (count - 1))
        {
            int last_sc = NULL;
            while (!last_sc)
                last_sc = getc();
            
            char c = (char) getc();
            if (c)
                ((char*) buf)[pos++] = c;
        }
        ((char*) buf)[count - 2] = '\0';
        return 0;
    }
    else
        return EBADF;
}