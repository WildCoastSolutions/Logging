#include "Logging.h"
#include "UnitTesting.h"
#include "Tests.h"
#include <fstream>
#include <thread>

using namespace Wild::Logging;
using namespace std;

void TestIndividualLoggers()
{
    Logger logger1;
    Logger logger2;

    logger1.AddStdoutDestination();
    logger2.AddStderrDestination();

    string line = Timestamp() + " Info: Testing logging, successful.\n";
    AssertPrints(
        logger1.Log(
            Level::Info,
            "Testing logging",
            "successful",
            {}),
        line);

    line = Timestamp() + " Error: Testing logging, successful.\n";
    AssertPrintsToStderr(
        logger2.Log(
            Level::Error,
            "Testing logging",
            "successful",
            {}),
        line);
}