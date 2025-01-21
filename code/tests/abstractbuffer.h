#ifndef ABSTRACTBUFFER_H
#define ABSTRACTBUFFER_H

template<typename T>
class AbstractBuffer {
protected:
    std::vector<T> elements;
    int writePointer, readPointer, nbElements, bufferSize;
    PcoSemaphore mutex, waitProd, waitConso;
    unsigned nbWaitingProd, nbWaitingConso;

public:
    // Constructor of empty buffer
    BufferN(unsigned int size) : elements(size), writePointer(0),
                                    readPointer(0), nbElements(0),
                                    bufferSize(size),
                                    mutex(1), waitProd(0),waitConso(0),
                                    nbWaitingProd(0), nbWaitingConso(0) {}

    // Destructor of buffer
    virtual ~BufferN() {}

    // Production / Consumption of <T> in buffer
    virtual void put(T item) = 0;
    virtual T get() = 0;
};

#endif // ABSTRACTBUFFER_H
