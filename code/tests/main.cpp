
#include "modeltemplate.h"
#include "modelnumbers.h"
#include "pcomodelchecker.h"

#include "modelbuffernimpostor.h"

#include <pcosynchro/pcomanager.h>

int main(int /*argc*/, char */*argv*/[])
{
    // Uncommenting the following line allows to easily observe the PcoManager in the debugger
    //    auto pcoManager = PcoManager::getInstance();

    // Model template
    {
        /*
        TemplateModelBlock model;
        PcoModelChecker checker;
        checker.setModel(&model);
        checker.run();
        */
    }

    // Numbers ModelCheck (example)
    {
        /*
        ModelNumbers model;
        PcoModelChecker checker;
        checker.setModel(&model);
        checker.run();
        */
    }

    // Impostor ModelCheck
    {
        ModelProdConsImpostor model;
        PcoModelChecker checker;
        checker.setModel(&model);
        checker.run();
    }

    return 0;
}
