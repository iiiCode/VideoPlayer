#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "RingBuffer.hpp"

RingBuffer::RingBuffer()
{
    init();
}

RingBuffer::~RingBuffer()
{
    destroy();
}

void RingBuffer::init()
{
    mReadIndex = 0;
    mWriteIndex = 0;
    
    mWriteSem = sem_open("write.sem", O_CREAT, 0644, RING_BUFFER_SIZE);
    mReadSem = sem_open("read.sem", O_CREAT, 0644, 0);
    
    if (mWriteSem == SEM_FAILED || mReadSem == SEM_FAILED) {
        printf("sem_init failed: %s\n", strerror(errno));
        exit(-1);
    }
    
    sem_unlink("write.sem");
    sem_unlink("read.sem");
}

void RingBuffer::destroy()
{
    if (mReadIndex <= mWriteIndex) {
        for (int i = mReadIndex; i < mWriteIndex; i ++) {
            mDestroyFunc(mRingBuffer[i]);
        }
    } else {
        for (int i = 0; i < mWriteIndex; i ++) {
            mDestroyFunc(mRingBuffer[i]);
        }
        
        for (int i = mReadIndex; i < RING_BUFFER_SIZE; i ++) {
            mDestroyFunc(mRingBuffer[i]);
        }
    }
    
    mReadIndex = 0;
    mWriteIndex = 0;
    
    sem_close(mReadSem);
    sem_close(mWriteSem);
}

void RingBuffer::setDestroy(void (*destroy)(DataType *))
{
    mDestroyFunc = destroy;
}

void RingBuffer::enqueue(DataType *item)
{
    sem_wait(mWriteSem);
    
    mRingBuffer[mWriteIndex] = item;
    mWriteIndex = (mWriteIndex + 1) % RING_BUFFER_SIZE;
    
    sem_post(mReadSem);
}

void RingBuffer::dequeue(DataType **item)
{
    sem_wait(mReadSem);
    
    *item = mRingBuffer[mReadIndex];
    mReadIndex = (mReadIndex + 1) % RING_BUFFER_SIZE;
    
    sem_post(mWriteSem);
}

void RingBuffer::flush()
{
    destroy();
    init();
}

void RingBuffer::notifyRingBufferExit()
{
    sem_post(mReadSem);
    sem_post(mWriteSem);
}