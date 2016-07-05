
#include "UnitTesting.h"
#include "Logging.h"
#include "Tests.h"

using namespace std;
using namespace Wild::Logging;
using namespace Wild::UnitTesting;

extern vector<string> allLines;

void AdditionalFileTests()
{
    allLines.push_back(Timestamp() + " Info: Testing, this log is to test that logging work across multiple files.");
    AssertPrints(
        Info("Testing", "this log is to test that logging work across multiple files"),
        allLines.back() + "\n");
}