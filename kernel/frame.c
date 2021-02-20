#include <kernel/frame.h>

int* frames;
int nframes;

static void set_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr / 0x1000;
    uint32_t index = INDEX_FROM_BIT(frame);
    uint32_t offset = OFFSET_FROM_BIT(frame);
    frames[index] |= (0x1 << offset);
}

static void clear_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr / 0x1000;
    uint32_t index = INDEX_FROM_BIT(frame);
    uint32_t offset = OFFSET_FROM_BIT(frame);
    frames[index] &= ~(0x1 << offset);
}

static uint32_t test_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr / 0x1000;
    uint32_t index = INDEX_FROM_BIT(frame);
    uint32_t offset = OFFSET_FROM_BIT(frame);
    return (frames[index] & (0x1 << offset));
}

static uint32_t first_frame(void)
{
    for (uint32_t i = 0; i < INDEX_FROM_BIT(nframes); i++)
    {
        if (frames[i] != 0xFFFFFFFF)
        {
            for (uint32_t j = 0; j < 32; j++)
            {
                uint32_t test = 0x1 << j;
                if (!(frames[i] & test))
                    return i * 4 * 8 + j;
            }
        }
    }
    return (uint32_t) -1;
}

void frame_alloc(page_t* p, bool is_kernel, bool is_writable)
{
    if (p->frame != 0)
        return;
    else
    {
        uint32_t index = first_frame();
        if (index == (uint32_t) -1)
        {
            klog(FATAL, "No free frames !");
            abort();
        }
        set_frame(index * 0x1000);
        p->present = 1;
        p->rw = is_writable >= 1;
        p->user = !is_kernel;
        p->frame = index;
    }
}

void frame_free(page_t* p)
{
    uint32_t frame = p->frame;
    if (frame)
    {
        clear_frame(frame);
        p->frame = 0x0;
    } 
}