
#include "modeltemplate.h"
#include "modelnumbers.h"
#include "modelbuffernimpostor.h"
#include "modelbuffernopti.h"
#include "pcomodelchecker.h"

#include "modelbuffernimpostor.h"

#include <pcosynchro/pcomanager.h>
#include <cstring>

int main(int argc, char *argv[])
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

    // Opti ModelCheck
    if (argc == 1 || (argc == 2 && strcmp(argv[1], "opti") == 0) )
    {
        ModelProdConsOpti model;
        PcoModelChecker checker;
        checker.setModel(&model);
        checker.run();
    }

    // Impostor ModelCheck
    if (argc == 1 || (argc == 2 && strcmp(argv[1], "impostor") == 0) )
    {
        ModelProdConsImpostor model;
        PcoModelChecker checker;
        checker.setModel(&model);
        checker.run();
    }

    return 0;
}
