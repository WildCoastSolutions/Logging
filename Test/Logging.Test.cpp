

#include "Logging.h"
#include "UnitTesting.h"
#include <fstream>

using namespace Wild::Logging;
using namespace std;

// Keep a list of all the lines we've output to check against the log file
vector<string> allLines;
string logFileName = "test.log";

void TestLogging()
{
    InfoBlob b = { I("1", "2"), I("3", "4") };

    allLines.push_back(Timestamp() + " Info: Starting application, startup successful. Data {1: 2, 3: 4}");
    AssertPrints(
        Info("Starting application", "startup successful", b),
        allLines.back() + "\n");
    
    allLines.push_back(Timestamp() + " Info: Starting application, startup successful. Data {foo: bar, Kung Fu: Hustle}");
    AssertPrints(
        Info("Starting application",
            "startup successful",
            { I("foo", "bar"), I("Kung Fu", "Hustle") }),
        allLines.back() + "\n");

    allLines.push_back(Timestamp() + " Info: Starting application, startup successful. Data {1: 2, 3: 4}");
    AssertPrints(
        Info(
            "Starting application",
            "startup successful",
            b),
        allLines.back() + "\n");

    allLines.push_back(Timestamp() + " Info: Starting application, startup successful. Data {1: 2, 3: 4, foo: bar}");
    AssertPrints(
        Info(
            "Starting application",
            "startup successful",
            b,
            { I("foo", "bar") }),
        allLines.back() + "\n");

    allLines.push_back(Timestamp() + " Info: Starting application, startup successful. Data {1: 2, 3: 4, foo: bar, Kung Fu: Hustle}");
    AssertPrints(
        Info(
            "Starting application",
            "startup successful",
            b,
            { I("foo", "bar"), I("Kung Fu", "Hustle") }),
        allLines.back() + "\n");

    allLines.push_back(Timestamp() + " Warning: Starting application but something weird happened, it might break.");
    AssertPrints(
        Warning("Starting application but something weird happened", "it might break"),
        allLines.back() + "\n");

    allLines.push_back(Timestamp() + " Error: Starting application, it broke.");
    AssertPrintsToStderr(
        Error("Starting application", "it broke"),
        allLines.back() + "\n");
}

void TestDebugging()
{
    std::stringstream output;
    std::streambuf* original = std::cout.rdbuf(output.rdbuf());

    Debug(1,
        "Useful debug message",
        "shouldn't be displayed as debug level is 0");
        
    AssertEquals(output.str(), "");

    std::cout.rdbuf(original);

    SetDebugLevel(1);

    allLines.push_back(Timestamp() + " Debug: Useful debug message, should be displayed.");
    AssertPrints(
        Debug(1,
            "Useful debug message",
            "should be displayed"),
        allLines.back() + "\n");
}

void TestFileOutput()
{
    // Check that everything has been written correctly to the file
    // allLines contains all of the output lines that we expect to be in the file
    
    ifstream file(logFileName);

    AssertTrue(file.is_open());

    string line;
    size_t i = 0;
    while (std::getline(file, line))
    {
        AssertEquals(line, allLines[i]);
        i++;
    }

    AssertTrue(i > 0);  // We saw some lines
}


int main(int argc, char* argv[])
{
    remove(logFileName.c_str());

    SetupLogging();

    AssertThrows(AddFileDestination(""), std::runtime_error);
    AddFileDestination(logFileName); // All messages will be written to this file, we'll check them in TestFileOutput

    TestLogging();
    TestDebugging();

    TestFileOutput();

    ShutdownLogging();

    AssertTrue(remove(logFileName.c_str()) == 0);
    ifstream file(logFileName);
    AssertTrue(!file.is_open());

    EndTest
}