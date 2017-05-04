#include <string.h>
#include "queue.h"

void Queue_Init(Queue *queue,unsigned char queue_size,unsigned char element_num)
{
  queue->tail = queue->head = 0;
  queue->element_num = element_num;
  queue->queue_size = queue_size;
}

unsigned short isEmpty(Queue *queue)
{
//#if OS_CRITICAL_METHOD == 3u                               /* Allocate storage for CPU status register */
//  OS_CPU_SR  cpu_sr = 0u;
//#endif
  
//  OS_ENTER_CRITICAL();
  if(queue->head == queue->tail)
  {
//    OS_EXIT_CRITICAL();
    return 1;
  }
//  OS_EXIT_CRITICAL();
  return 0;
}

unsigned short isFull(Queue *queue)
{
//#if OS_CRITICAL_METHOD == 3u                               /* Allocate storage for CPU status register */
//  OS_CPU_SR  cpu_sr = 0u;
//#endif
  
//  OS_ENTER_CRITICAL();
  //(rear+1)%maxSize==front;
  if((queue->head == ((queue->tail + 1)%queue->queue_size)))// || (queue->tail - queue->head == queue->queue_size - 1))
  {
//    OS_EXIT_CRITICAL();
    return 1;
  }
//  OS_EXIT_CRITICAL();
  return 0;
}

unsigned short enQueue(Queue *queue, unsigned short* pointer)
{
  
  if(isFull(queue))
  {
    return 1;
  }
  else
  {
    *pointer = queue->tail;

//    OS_ENTER_CRITICAL();
    queue->tail++;
    if(queue->tail == queue->queue_size)
    {
      queue->tail = 0;
    }
//    OS_EXIT_CRITICAL();
  }
  return 0;
}

unsigned char deQueue(Queue *queue, unsigned short* pointer)
{
  
  if(isEmpty(queue))
  {
    return 1;
  }
  else
  {
    *pointer = queue->head;
//    OS_ENTER_CRITICAL();
    queue->head++;
    if(queue->head == queue->queue_size)
    {
      queue->head = 0;
    }
//    OS_EXIT_CRITICAL();
  }
  return 0;
}
