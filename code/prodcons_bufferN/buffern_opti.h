#ifndef BUFFERN_OPTI_H
#define BUFFERN_OPTI_H

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
        mutex.acquire();
        if (nbElements == bufferSize) {
            nbWaitingProd += 1;
            mutex.release();
            waitProd.acquire();
        }
        elements[writePointer] = item;
        writePointer = (writePointer + 1)
                       % bufferSize;
        nbElements ++;
        if (nbWaitingConso > 0) {
            nbWaitingConso -= 1;
            waitConso.release();
        }
        else {
            mutex.release();
        }
    }

    virtual T get(void) override {
        T item;
        mutex.acquire();
        if (nbElements == 0) {
            nbWaitingConso += 1;
            mutex.release();
            waitConso.acquire();
        }
        item = elements[readPointer];
        readPointer = (readPointer + 1)
                      % bufferSize;
        nbElements --;
        if (nbWaitingProd > 0) {
            nbWaitingProd -= 1;
            waitProd.release();
        }
        else {
            mutex.release();
        }
        return item;
    }
};

#endif // BUFFERN_OPTI_H
