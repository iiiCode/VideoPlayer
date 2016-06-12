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

#ifdef ANDROID_PLATFORM
    sem_init(&mWriteSem, 0, RING_BUFFER_SIZE);
    sem_init(&mReadSem, 0, 0);
#else
    mWriteSem = sem_open("write.sem", O_CREAT, 0644, RING_BUFFER_SIZE);
    mReadSem = sem_open("read.sem", O_CREAT, 0644, 0);
    
    if (mWriteSem == SEM_FAILED || mReadSem == SEM_FAILED) {
        vLOGE("sem_init failed: %s\n", strerror(errno));
        exit(-1);
    }
    
    sem_unlink("write.sem");
    sem_unlink("read.sem");
#endif
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
    
#ifdef ANDROID_PLATFORM
    sem_destroy(&mWriteSem);
    sem_destroy(&mReadSem);
#else
    sem_close(mReadSem);
    sem_close(mWriteSem);
#endif
}

void RingBuffer::setDestroy(void (*destroy)(DataType *))
{
    mDestroyFunc = destroy;
}

void RingBuffer::enqueue(DataType *item)
{
#ifdef ANDROID_PLATFORM
    sem_wait(&mWriteSem);
#else
    sem_wait(mWriteSem);
#endif
    
    mRingBuffer[mWriteIndex] = item;
    mWriteIndex = (mWriteIndex + 1) % RING_BUFFER_SIZE;
    
#ifdef ANDROID_PLATFORM
    sem_post(&mReadSem);
#else
    sem_post(mReadSem);
#endif
}

void RingBuffer::dequeue(DataType **item)
{
#ifdef ANDROID_PLATFORM
    sem_wait(&mReadSem);
#else
    sem_wait(mReadSem);
#endif
    
    *item = mRingBuffer[mReadIndex];
    mReadIndex = (mReadIndex + 1) % RING_BUFFER_SIZE;
    
#ifdef ANDROID_PLATFORM
    sem_post(&mWriteSem);
#else
    sem_post(mWriteSem);
#endif
}

void RingBuffer::flush()
{
    destroy();
    init();
}

void RingBuffer::notifyRingBufferExit()
{
#ifdef ANDROID_PLATFORM
    sem_post(&mReadSem);
    sem_post(&mWriteSem);
#else
    sem_post(mReadSem);
    sem_post(mWriteSem);
#endif
}
