#include "../header/scheduler/pcbqueue.h"

void pcbqueue_init(struct PCBQueue *queue)
{
    queue->count = 0;
}

void pcbqueue_enque(struct PCBQueue *queue, struct ProcessControlBlock *pcb)
{
    if (queue->count < 16)
    {
        queue->element[queue->count] = pcb;
        queue->count++;
    }
}

struct ProcessControlBlock *pcbqueue_deque(struct PCBQueue *queue)
{
    if (queue->count > 0)
    {
        struct ProcessControlBlock *pcb = queue->element[0];
        for (int i = 0; i < queue->count - 1; i++)
        {
            queue->element[i] = queue->element[i + 1];
        }
        queue->count--;
        return pcb;
    }
    return 0;
}