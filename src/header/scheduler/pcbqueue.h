#ifndef PCBQUEUE_H
#define PCBQUEUE_H
#include <stdint.h>
#include <stdbool.h>
struct PCBQueue
{
    struct ProcessControlBlock *element[16];
    uint8_t count;
};
/**
 * @brief
 *
 * @param queue
 */
void pcbqueue_init(struct PCBQueue *queue);
/**
 * @brief
 *
 * @param queue
 * @param pcb
 */
void pcbqueue_enque(struct PCBQueue *queue, struct ProcessControlBlock *pcb);
/**
 * @brief
 *
 * @param queue
 * @return struct ProcessControlBlock*
 */
struct ProcessControlBlock *pcbqueue_deque(struct PCBQueue *queue);
#endif