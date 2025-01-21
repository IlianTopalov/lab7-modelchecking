#ifndef ABSTRACTBUFFER_H
#define ABSTRACTBUFFER_H

# define SIZE 10

template<typename T>
class AbstractBuffer {
public:
    // Production / Consumption of <T> in buffer
    virtual void put(T item) = 0;
    virtual T get() = 0;
};

#endif // ABSTRACTBUFFER_H
