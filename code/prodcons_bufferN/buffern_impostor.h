#ifndef BUFFERN_IMPOSTOR_H
#define BUFFERN_IMPOSTOR_H

#include "abstractbuffer.h"
#include <pcosynchro/pcosemaphore.h>

template<typename T> class BufferN : public AbstractBuffer<T> {
protected:
    std::vector<T> elements;
    int writePointer, readPointer, nbElements, bufferSize;
    PcoSemaphore mutex, waitProd, waitConso;
    unsigned nbWaitingProd, nbWaitingConso;

public:

    BufferN(unsigned int size) : elements(size), writePointer(0),
                                    readPointer(0), nbElements(0),
                                    bufferSize(size),
                                    mutex(1), waitProd(0),waitConso(0),
                                    nbWaitingProd(0), nbWaitingConso(0) {
       }

    virtual ~BufferN() {}

    virtual void put(T item) override {
        waitNotFull.acquire();  // Should come after mutex acquire
        mutex.acquire();
        elements[writePointer] = item;
        writePointer = (writePointer + 1) % bufferSize;
        waitNotEmpty.release();
        mutex.release();
        }
    

    virtual T get(void) override {
        T item;
        waitNotEmpty.acquire();  // Should come after mutex acquire
        mutex.acquire();
        item = elements[readPointer];
        readPointer = (readPointer + 1) % bufferSize;
        waitNotFull.release();
        mutex.release();
        return item;
    }
};

#endif // BUFFERN_IMPOSTOR_H
