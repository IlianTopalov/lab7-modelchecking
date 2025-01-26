#ifndef MODELBUFFERNIMPOSTOR_H
#define MODELBUFFERNIMPOSTOR_H

#include <iostream>

#include "pcomodel.h"
#include "pcosynchro/pcosemaphore.h"
#include "pcoconcurrencyanalyzer.h"

#include "abstractbuffer.h"

static std::unique_ptr<PcoSemaphore> mutexSem = std::make_unique<PcoSemaphore>(1);
static std::unique_ptr<PcoSemaphore> waitNotEmpty = std::make_unique<PcoSemaphore>(0);
static std::unique_ptr<PcoSemaphore> waitNotFull = std::make_unique<PcoSemaphore>(0);
static std::vector<int> elements(SIZE);
static int writePointer = 0;
static int readPointer = 0;
static int bufferSize = SIZE;

class ThreadProducer : public ObservableThread, public AbstractBuffer<int> {
protected:
    // std::vector<int>& elements;
    // int writePointer;
    // int readPointer;
    // int bufferSize;

    // Pointers to shared semaphores
    // static std::unique_ptr<PcoSemaphore> mutex;
    // static std::unique_ptr<PcoSemaphore> waitNotFull;
    // static std::unique_ptr<PcoSemaphore> waitNotEmpty;

public:
    explicit ThreadProducer(std::string id = "") : ObservableThread(std::move(id)) {
        scenarioGraph = std::make_unique<ScenarioGraph>();
        auto scenario = scenarioGraph->createNode(this, -1);
        auto p6 = scenarioGraph->createNode(this, 6);
        auto p7 = scenarioGraph->createNode(this, 7);
        auto p8 = scenarioGraph->createNode(this, 8);
        auto p9 = scenarioGraph->createNode(this, 9);
        auto p10 = scenarioGraph->createNode(this, 10);

        scenario->next.push_back(p6);
        p6->next.push_back(p7);
        p7->next.push_back(p8);
        p8->next.push_back(p9);
        p9->next.push_back(p10);
        scenarioGraph->setInitialNode(scenario);
    }

    static void init() {
    }

    static void clean() {

    }

    void put(int item) override {
        startSection(6);
        waitNotFull.get()->acquire();  // Should come after mutex acquire to be correct

        startSection(7);
        mutexSem.get()->acquire();

        startSection(8);
        elements[writePointer] = item;
        writePointer = (writePointer + 1) % bufferSize;

        startSection(9);
        waitNotEmpty.get()->release();

        startSection(10);
        mutexSem.get()->release();
    }

    int get() override {
        return 0;
    }

private:
    /**
     * Produce and put 3 items in the buffer
     */
    void run() override {
        int item1 = 3,
            item2 = 2,
            item3 = 1;

        put(item1);
        put(item2);
        put(item3);

        endScenario();  // TODO should be in put() ?
    }
};


class ThreadConsumer : public ObservableThread, public AbstractBuffer<int> {
protected:
    // std::vector<int>& elements;
    // int writePointer;
    // int readPointer;
    // int bufferSize;

    // Pointers to shared semaphores
    // static std::unique_ptr<PcoSemaphore> mutex;
    // static std::unique_ptr<PcoSemaphore> waitNotFull;
    // static std::unique_ptr<PcoSemaphore> waitNotEmpty;

public:
    explicit ThreadConsumer(std::string id = "") : ObservableThread(std::move(id)) {
        scenarioGraph = std::make_unique<ScenarioGraph>();
        auto scenario = scenarioGraph->createNode(this, -1);
        auto p1 = scenarioGraph->createNode(this, 1);
        auto p2 = scenarioGraph->createNode(this, 2);
        auto p3 = scenarioGraph->createNode(this, 3);
        auto p4 = scenarioGraph->createNode(this, 4);
        auto p5 = scenarioGraph->createNode(this, 5);

        scenario->next.push_back(p1);
        p1->next.push_back(p2);
        p2->next.push_back(p3);
        p3->next.push_back(p4);
        p4->next.push_back(p5);
        scenarioGraph->setInitialNode(scenario);
    }

    static void init() {

    }

    static void clean() {

    }

    int get() override {
        int item;
        startSection(1);
        waitNotEmpty.get()->acquire();  // Should come after mutex acquire to be correct

        startSection(2);
        mutexSem.get()->acquire();

        startSection(3);
        item = elements[readPointer];
        readPointer = (readPointer + 1) % bufferSize;

        startSection(4);
        waitNotFull.get()->release();

        startSection(5);
        mutexSem.get()->release();

        return item;
    }

    void put(int /*item*/) override {
    }

private:
    /**
     * Consume and remove 3 items from the buffer
     */
    void run() override {
        int item1,
            item2,
            item3;

        item1 = get();
        item2 = get();
        item3 = get();

        endScenario();  // Should be in get() ?
    }
};

// Shared semaphores
// std::unique_ptr<PcoSemaphore> ThreadProducer::mutex = std::make_unique<PcoSemaphore>();
// std::unique_ptr<PcoSemaphore> ThreadProducer::waitNotFull = std::make_unique<PcoSemaphore>();
// std::unique_ptr<PcoSemaphore> ThreadProducer::waitNotEmpty = std::make_unique<PcoSemaphore>();

class ModelProdConsImpostor : public PcoModel {

    bool checkInvariants() override {
        return true;
    }


    void build() override {

        std::vector<int> elements(SIZE);

        threads.emplace_back(std::make_unique<ThreadProducer>("producer1"));
        threads.emplace_back(std::make_unique<ThreadConsumer>("consumer1"));

        scenarioBuilder = std::make_unique<ScenarioBuilderBuffer>();
        scenarioBuilder->init(threads, 9);  // TODO Change recursion depth ?
    }

    void preRun(Scenario& /*scenario*/) override {
        mutexSem = std::make_unique<PcoSemaphore>(1);
        waitNotEmpty = std::make_unique<PcoSemaphore>(0);
        waitNotFull = std::make_unique<PcoSemaphore>(0);
        elements = std::vector{SIZE};
        writePointer = 0;
        readPointer = 0;
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
