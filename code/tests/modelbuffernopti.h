#ifndef MODELBUFFERNOPTI_H
#define MODELBUFFERNOPTI_H

#define PREDIFINED_SCENARIO true

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
            scenarioSize = 5;
        } else {
            isProd = false;
            scenarioSize = 5;
        }
        scenarioGraph = std::make_unique<ScenarioGraph>();
        auto scenario = scenarioGraph->createNode(this, -1);
        if (isProd) {
            auto p1 = scenarioGraph->createNode(this, 1);
            auto p2 = scenarioGraph->createNode(this, 2);
            auto p3 = scenarioGraph->createNode(this, 3);
            auto p4 = scenarioGraph->createNode(this, 4);
            auto p5 = scenarioGraph->createNode(this, 5);
            scenario->next.push_back(p1);
            p1->next.push_back(p2);
            p1->next.push_back(p3);
            p2->next.push_back(p3);
            p3->next.push_back(p4);
            p3->next.push_back(p5);
            scenarioGraph->setInitialNode(scenario);
        } else { // Consumer
            auto p1 = scenarioGraph->createNode(this, 1);
            auto p2 = scenarioGraph->createNode(this, 2);
            auto p3 = scenarioGraph->createNode(this, 3);
            auto p4 = scenarioGraph->createNode(this, 4);
            auto p5 = scenarioGraph->createNode(this, 5);
            scenario->next.push_back(p1);
            p1->next.push_back(p2);
            p1->next.push_back(p3);
            p2->next.push_back(p3);
            p3->next.push_back(p4);
            p3->next.push_back(p5);
            scenarioGraph->setInitialNode(scenario);
        }
    }

    static void init() {
    }

    static void clean() {

    }

    void put(int item) override {
        startSection(1);
        mutex.get()->acquire();
        if (nbElements == bufferSize) {
            nbWaitingProd += 1;
            mutex.get()->release();
            startSection(2);
            waitProd.get()->acquire();
        }
        startSection(3);
        elements[writePointer] = item;
        writePointer = (writePointer + 1)
                       % bufferSize;
        nbElements ++;
        if (nbWaitingConso > 0) {
            startSection(4);
            nbWaitingConso -= 1;
            waitConso.get()->release();
        }
        else {
            startSection(5);
            mutex.get()->release();
        }
        endScenario();
    }

    int get(void) override {
        int item;
        startSection(1);
        mutex.get()->acquire();
        if (nbElements == 0) {
            nbWaitingConso += 1;
            mutex.get()->release();
            startSection(2);
            waitConso.get()->acquire();
        }
        startSection(3);
        item = elements[readPointer];
        readPointer = (readPointer + 1)
                      % bufferSize;
        nbElements --;
        if (nbWaitingProd > 0) {
            startSection(4);
            nbWaitingProd -= 1;
            waitProd.get()->release();
        }
        else {
            startSection(5);
            mutex.get()->release();
        }
        endScenario();
        return item;
    }

    // Reinitialize the buffer's static values in case something broke
    static void reset() {
        writePointer = 0;
        readPointer = 0;
        nbElements = 0;
        nbWaitingProd = 0;
        nbWaitingConso = 0;
        elements.clear();
        elements.reserve(SIZE);
        mutex = std::make_unique<PcoSemaphore>(1);
        waitProd = std::make_unique<PcoSemaphore>(0);
        waitConso = std::make_unique<PcoSemaphore>(0);
    }

private:
    bool isProd = false;
    int scenarioSize = 0;
    static std::unique_ptr<PcoSemaphore> mutex;
    static std::unique_ptr<PcoSemaphore> waitProd;
    static std::unique_ptr<PcoSemaphore> waitConso;
    static std::vector<int> elements;
    static int writePointer, readPointer, nbElements, bufferSize;
    static unsigned nbWaitingProd, nbWaitingConso;

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

std::unique_ptr<PcoSemaphore> ThreadProdCon::mutex = std::make_unique<PcoSemaphore>(1);
std::unique_ptr<PcoSemaphore> ThreadProdCon::waitProd = std::make_unique<PcoSemaphore>(0);
std::unique_ptr<PcoSemaphore> ThreadProdCon::waitConso = std::make_unique<PcoSemaphore>(0);
std::vector<int> ThreadProdCon::elements(SIZE);
int ThreadProdCon::writePointer = 0;
int ThreadProdCon::readPointer = 0;
int ThreadProdCon::nbElements = 0;
int ThreadProdCon::bufferSize = SIZE; // or any other appropriate initial value
unsigned ThreadProdCon::nbWaitingProd = 0;
unsigned ThreadProdCon::nbWaitingConso = 0;

class ModelProdConsOpti : public PcoModel {

    bool checkInvariants() override {
        return true;
    }


    void build() override {
        int depth = 0;
        // Create threads for our scenario
        threads.emplace_back(std::make_unique<ThreadProdCon>("Prod1"));
        threads.emplace_back(std::make_unique<ThreadProdCon>("Con1"));
        // threads.emplace_back(std::make_unique<ThreadProdCon>("Prod2"));
        // threads.emplace_back(std::make_unique<ThreadProdCon>("Con2"));
        // Get the depth of the scenario
        // Would have been cleaner to have some sort of intermediate class with virtual depth()
        // to handle this but threads (from PcoModel) is already <ObservableThread>
        for (auto &t : threads) {
            if(auto *producer = dynamic_cast<ThreadProdCon *>(t.get()))
                depth += producer->depth();
        }
#ifdef SCENARIO_STEP
        scenarioBuilder = std::make_unique<ScenarioBuilderBuffer>(SCENARIO_STEP);
#else
#ifdef PREDIFINED_SCENARIO
        auto prod = threads.at(0).get();
        auto con = threads.at(1).get();
        auto builder = std::make_unique<PredefinedScenarioBuilderIter>();
        std::vector<Scenario> scenarios = {
            {{prod, 1},{prod, 3},{prod, 5},{con, 1}, {con, 3}, {con, 5}},
            {{con, 1},{prod, 1},{prod, 3},{prod, 4},{con, 2}, {con, 3}, {con, 5}}
        };
        builder->setScenarios(scenarios);
        scenarioBuilder = std::move(builder);
#else
        scenarioBuilder = std::make_unique<ScenarioBuilderBuffer>();
#endif
#endif
        scenarioBuilder->init(threads, depth);
    }

    void preRun(Scenario& /*scenario*/) override {
        ThreadProdCon::reset();
    }

    void postRun(Scenario &scenario) override {
        std::cout << "---------------------------------------" << std::endl;
        std::cout << "Scenario : ";
        ScenarioPrint::printScenario(scenario);
    }

    void finalReport() override {
        std::cout << "---------------------------------------" << std::endl;

        /* TODO LOGS */

        std::cout << std::endl;
        std::cout << std::flush;
    }

    // private:
        // int scenarioAmount = 1;
};

#endif // MODELBUFFERNOPTI_H
