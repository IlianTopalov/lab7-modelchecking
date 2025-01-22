#ifndef MODELBUFFERNOPTI_H
#define MODELBUFFERNOPTI_H

#include <iostream>

#include "pcomodel.h"
#include "pcosynchro/pcosemaphore.h"
#include "pcoconcurrencyanalyzer.h"

#include "abstractbuffer.h"

class ThreadProdCon : public ObservableThread, public AbstractBuffer<int> {
public:
    int depth() {
        return this->scenarioSize;
    }

    explicit ThreadProdCon(std::string id = "") : ObservableThread(id) {
        std::string lowerId = id;
        std::transform(lowerId.begin(), lowerId.end(), lowerId.begin(), ::tolower);
        if (lowerId.compare(0, 4, "prod") == 0) {
            isProd = true;
        }
        scenarioGraph = std::make_unique<ScenarioGraph>();
    }

    static void init() {
    }

    static void clean() {

    }

    void put(int item) override {
        mutex.get()->acquire();
        if (nbElements == bufferSize) {
            nbWaitingProd += 1;
            mutex.get()->release();
            waitProd.get()->acquire();
        }
        elements[writePointer] = item;
        writePointer = (writePointer + 1)
                       % bufferSize;
        nbElements ++;
        if (nbWaitingConso > 0) {
            nbWaitingConso -= 1;
            waitConso.get()->release();
        }
        else {
            mutex.get()->release();
        }
    }

    int get(void) override {
        int item;
        mutex.get()->acquire();
        if (nbElements == 0) {
            nbWaitingConso += 1;
            mutex.get()->release();
            waitConso.get()->acquire();
        }
        item = elements[readPointer];
        readPointer = (readPointer + 1)
                      % bufferSize;
        nbElements --;
        if (nbWaitingProd > 0) {
            nbWaitingProd -= 1;
            waitProd.get()->release();
        }
        else {
            mutex.get()->release();
        }
        return item;
    }

private:
     bool isProd = false;
    int scenarioSize = 0;
    static std::unique_ptr<PcoSemaphore> mutex;
    static std::unique_ptr<PcoSemaphore> waitProd;
    static std::unique_ptr<PcoSemaphore> waitConso;
    std::vector<int> elements;
    int writePointer, readPointer, nbElements, bufferSize;
    unsigned nbWaitingProd, nbWaitingConso;

   void run() override {
        int item;
        if (isProd) {
            item = 1;
            put(item);
        } else {
            item = get();
        }

    }
};

std::unique_ptr<PcoSemaphore> ThreadProdCon::mutex = std::make_unique<PcoSemaphore>(0);
std::unique_ptr<PcoSemaphore> ThreadProdCon::waitProd = std::make_unique<PcoSemaphore>(0);
std::unique_ptr<PcoSemaphore> ThreadProdCon::waitConso = std::make_unique<PcoSemaphore>(0);

template <typename T>
class ModelProdConsOpti : public PcoModel {
    ModelProdConsOpti(int size, int scenarioAmount = 1) : elements(size), elementsCapacity(size), scenarioAmount(scenarioAmount),
                                                            writePointer(0), readPointer(0), nbElements(0), bufferSize(size),
                                                            mutex(1), waitProd(0), waitConso(0), nbWaitingProd(0), nbWaitingConso(0) {
    }

    bool checkInvariants() override {
        return true;
    }


    void build() override {
        int depth = 0;
        // Create threads for our scenario
        // threads.emplace_back(std::make_unique<ThreadProducer>("Prod1"));
        threads.emplace_back(std::make_unique<ThreadProdCon>("Prod1"));
        // threads.emplace_back(std::make_unique<ThreadConsumer>("Cons1"));
        // Get the depth of the scenario
        // Would have been cleaner to have some sort of intermediate class with virtual depth()
        // to handle this but threads (from PcoModel) is already <ObservableThread>
        for (auto &t : threads) {
            if(auto *producer = dynamic_cast<ThreadProducer *>(t.get()))
                depth += producer->depth();
            else if(auto *consumer = dynamic_cast<ThreadConsumer *>(t.get()))
                depth += consumer->depth();
        }
        // If scenarioAmount = 1 => generates all scenarios
        scenarioBuilder = std::make_unique<ScenarioBuilderBuffer>(scenarioAmount);
        scenarioBuilder->init(threads, depth);
    }

    void preRun(Scenario& /*scenario*/) override {
        elements.clear();
        elements.reserve(elementsCapacity); // In case something broke
    }

    void postRun(Scenario &scenario) override {

    }

    void finalReport() override {
        std::cout << "---------------------------------------" << std::endl;

        /* TODO LOGS */

        std::cout << std::endl;
        std::cout << std::flush;
    }

    private:
        int scenarioAmount;
        int elementsCapacity;
        std::vector<T> elements;

        int writePointer, readPointer, nbElements, bufferSize;
        PcoSemaphore mutex, waitProd, waitConso;
        unsigned nbWaitingProd, nbWaitingConso;
};

#endif // MODELBUFFERNOPTI_H
