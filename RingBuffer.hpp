#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <semaphore.h>

typedef void DataType;
#define RING_BUFFER_SIZE 10

class RingBuffer {
public:
    RingBuffer();
    ~RingBuffer();
    
public:
    void enqueue(DataType *item);
    void dequeue(DataType **item);
    void flush();
    void notifyRingBufferExit();
    void setDestroy(void (*destroy)(DataType *));
private:
    void init();
    void destroy();
private:
    sem_t *mWriteSem;
    sem_t *mReadSem;
    
    int mReadIndex;
    int mWriteIndex;
    DataType *mRingBuffer[RING_BUFFER_SIZE];
    void (*mDestroyFunc)(DataType *);
};

#endif //_RING_BUFFER_H_