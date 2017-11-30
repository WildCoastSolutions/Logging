

#include "Logging.h"
#include "UnitTesting.h"
#include "Tests.h"
#include <fstream>
#include <thread>

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

    allLines.push_back(Timestamp() + " Info: Started application. Data {1: 2, 3: 4}");
    AssertPrints(
        Info("Started application", b),
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

    b.push_back(I("5", "6"));

    allLines.push_back(Timestamp() + " Info: Starting application, startup successful. Data {1: 2, 3: 4, 5: 6, foo: bar, Kung Fu: Hustle}");
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
        if (i < allLines.size())
        {
            AssertEquals(line, allLines[i]);
        }
        else
        {
            AssertEquals(81, line.size());  // Check all our multithreaded file writes from TestThreadedBehaviour aren't interleaved etc
        }
        i++;
    }

    AssertTrue(i > 0);  // We saw some lines
}

void Thread1()
{
    InfoBlob blob({ I("thread#", "1") });
    for (int i = 0; i < 100; i++)
        Info("Logging from thread", "thread running", blob);
}

void Thread2()
{
    InfoBlob blob({ I("thread#", "2") });
    for (int i = 0; i < 100; i++)
        Info("Logging from thread", "thread running", blob);
}

void TestThreadedBehaviour()
{
    stringstream output;
    streambuf* original = std::cout.rdbuf(output.rdbuf());

    InfoBlob blob({ I("thread#", "0") });
    thread t1(Thread1);
    thread t2(Thread2);
    for (int i = 0; i < 100; i++)
        Info("Logging from thread", "thread running", blob);
    t1.join();
    t2.join();

    string out = output.str();
    std::cout.rdbuf(original);

    string line;
    while (std::getline(output, line, '\n')) {
        AssertEquals(81, line.size());
    }

}

void ReadmeExampleCode()
{
    string info = "interesting info";
    string moreInfo = "more interesting info";
    string reason = "things went wrong";

    SetupLogging();
    AddFileDestination("application.log");
    SetDebugLevel(1);

    // Simple message
    Info("Starting application", "startup successful");

    // Message with additional data
    Info(   "Starting application", "startup successful",
            { I("info", info), I("more_info", moreInfo) });

    Warning("Starting application", "wait I don't like the looks of this");

    // Persistent logging data used for multiple messages
    InfoBlob blob = { I("info", info), I("more_info", moreInfo) };

    Error("Starting application", "startup failed", blob, { I("reason", reason) });

    // Not shown due to debug level
    Debug(2, "Starting application", "here is some really detailed info", blob);

    Debug(1, "Starting application", "here is some detailed info", { I("info", info) });

    ShutdownLogging();
}


int main(int argc, char* argv[])
{
    remove(logFileName.c_str());

    SetupLogging();

    AssertThrows(AddFileDestination(""), std::runtime_error);
    AddFileDestination(logFileName); // All messages will be written to this file, we'll check them in TestFileOutput

    TestLogging();
    TestDebugging();
    AdditionalFileTests();
    TestIndividualLoggers();

    TestThreadedBehaviour();

    TestFileOutput();

    ShutdownLogging();

    AssertTrue(remove(logFileName.c_str()) == 0);
    ifstream file(logFileName);
    AssertTrue(!file.is_open());

    //ReadmeExampleCode();


    EndTest
}