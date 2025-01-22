
#include "modeltemplate.h"
#include "modelnumbers.h"
#include "modelbuffernimpostor.h"
#include "modelbuffernopti.h"
#include "pcomodelchecker.h"

#include <pcosynchro/pcomanager.h>

int main(int argc, char *argv[])
{
    // Uncommenting the following line allows to easily observe the PcoManager in the debugger
    //    auto pcoManager = PcoManager::getInstance();

    // Model template
    {
        // TemplateModelBlock model;
        // PcoModelChecker checker;
        // checker.setModel(&model);
        // checker.run();
    }
    {
        // ModelNumbers model;
        // PcoModelChecker checker;
        // checker.setModel(&model);
        // checker.run();
    }
    if (argc == 1 || argc == 2 && argv[1] == "opti" )
    {
        ModelProdConsOpti model;
        PcoModelChecker checker;
        checker.setModel(&model);
        checker.run();
    }
    if (argc == 1 || argc == 2 && argv[1] == "impostor" )
    {
        ModelProdConsImpostor model;
        PcoModelChecker checker;
        checker.setModel(&model);
        checker.run();
    }

    return 0;
}
