#ifndef MODELBUFFERNIMPOSTOR_H
#define MODELBUFFERNIMPOSTOR_H

#include <iostream>

#include "pcomodel.h"
#include "pcosynchro/pcosemaphore.h"
#include "pcoconcurrencyanalyzer.h"

class ThreadProducer : public ObservableThread {  // TODO Extend AbstractBuffer
public:
    explicit ThreadProducer(std::string id = "") : ObservableThread(std::move(id)) {
        scenarioGraph = std::make_unique<ScenarioGraph>();
    }

    static void init() {
    }

    static void clean() {

    }

private:
    static std::unique_ptr<PcoSemaphore> semaphore;
    void run() override {

    }
};

class ThreadConsumer : public ObservableThread {
public:
    explicit ThreadConsumer(std::string id = "") : ObservableThread(std::move(id)) {
        scenarioGraph = std::make_unique<ScenarioGraph>();
    }

    static void init() {

    }

    static void clean() {

    }

private:
    static std::unique_ptr<PcoSemaphore> semaphore;
    void run() override {

    }
};

class ModelProdConsImpostor : public PcoModel {
    bool checkInvariants() override {
        return true;
    }
    
    
    void build() override {
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
