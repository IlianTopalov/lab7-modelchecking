#ifndef MODELBUFFERNIMPOSTOR_H
#define MODELBUFFERNIMPOSTOR_H

#include <iostream>

#include "pcomodel.h"
#include "pcosynchro/pcosemaphore.h"
#include "pcoconcurrencyanalyzer.h"

#include "abstractbuffer.h"

class ThreadProducer : public ObservableThread, public AbstractBuffer<int> {
protected:
    std::vector<int>& elements;
    int writePointer;
    int readPointer;
    int bufferSize;

public:
    explicit ThreadProducer(std::string id = "", std::vector<int>& elements) : ObservableThread(std::move(id)),
    bufferSize(this->elements.capacity()),
    writePointer(0), readPointer(0),
    elements(elements) {
        scenarioGraph = std::make_unique<ScenarioGraph>();
    }

    static void init() {
    }

    static void clean() {

    }

    void put(int item) override {
        // Section 1
        waitNotFull.acquire();  // Should come after mutex acquire
        // Section 2
        mutex.acquire();
        // Section 3
        elements[writePointer] = item;
        writePointer = (writePointer + 1) % bufferSize;
        // Section 4
        waitNotEmpty.release();
        // Section 5
        mutex.release();
    }

private:
    static std::unique_ptr<PcoSemaphore> mutex;
    static std::unique_ptr<PcoSemaphore> waitNotFull;
    static std::unique_ptr<PcoSemaphore> waitNotEmpty;


    void run() override {
        int item1 = 3,
            item2 = 2,
            item3 = 1;

        put(item1);
        put(item2);
        put(item3);
    }
};


class ThreadConsumer : public ObservableThread, public AbstractBuffer<int> {
protected:
    std::vector<int>& elements;
    int writePointer;
    int readPointer;
    int bufferSize;

public:
    explicit ThreadConsumer(std::string id = "") : ObservableThread(std::move(id)),
    bufferSize(this->elements.capacity()),
    writePointer(0), readPointer(0),
    elements(elements) {
        scenarioGraph = std::make_unique<ScenarioGraph>();
    }

    static void init() {

    }

    static void clean() {

    }

    int get() override {
        int item;
        // Section 1
        waitNotEmpty.acquire();  // Should come after mutex acquire
        // Section 2
        mutex.acquire();
        // Section 3
        item = elements[readPointer];
        readPointer = (readPointer + 1) % bufferSize;
        // Section 4
        waitNotFull.release();
        // Section 5
        mutex.release();

        return item;
    }

private:
    static std::unique_ptr<PcoSemaphore> mutex;
    static std::unique_ptr<PcoSemaphore> waitNotFull;
    static std::unique_ptr<PcoSemaphore> waitNotEmpty;

    void run() override {
        int item1,
            item2,
            item3;

        item1 = get();
        item2 = get();
        item3 = get();
        

    }
};

std::unique_ptr<PcoSemaphore> ThreadProducer::mutex = nullptr;
std::unique_ptr<PcoSemaphore> ThreadProducer::waitNotFull = nullptr;
std::unique_ptr<PcoSemaphore> ThreadProducer::waitNotEmpty = nullptr;

class ModelProdConsImpostor : public PcoModel {

    bool checkInvariants() override {
        return true;
    }
    
    
    void build() override {
        
        std::vector<int> elements(SIZE);

        scenarioBuilder = std::make_unique<ScenarioBuilderBuffer>();
        scenarioBuilder->init(threads, 9);  // TODO Change recursion depth ?
    }

    void preRun(Scenario& /*scenario*/) override {

    }

    void postRun(Scenario &scenario) override {

    }

    void finalReport() override {
        std::cout << "---------------------------------------" << std::endl;

        /* TODO LOGS */

        std::cout << std::endl;
        std::cout << std::flush;
    }
};

#endif // MODELBUFFERNIMPOSTOR_H
