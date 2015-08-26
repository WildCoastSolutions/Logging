# C++ Logging

This is a simple, header only, no dependency logging library.


# Sample Code

```C++
#include "Wild/Logging.h"

using namespace Wild::Logging;
using namespace std;

int main(int argc, char* argv[])
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
    InfoBlob blob = { I("info", info), I("moreInfo", moreInfo) };

    Error("Starting application", "startup failed", blob, { I("reason", reason) });

    // Not shown due to debug level
    Debug(2, "Starting application", "here is some really detailed info", blob);

    Debug(1, "Starting application", "here is some detailed info", { I("info", info) });

    ShutdownLogging();
}

```

The above code results in the following being printed to console and the file ```application.log```

```
2015-08-26T06:39:29Z Info: Starting application, startup successful.
2015-08-26T06:39:29Z Info: Starting application, startup successful. Data {info: interesting info, more_info: more interesting info}
2015-08-26T06:39:29Z Warning: Starting application, wait I don't like the looks of this.
2015-08-26T06:39:29Z Error: Starting application, startup failed. Data {info: interesting info, moreInfo: more interesting info, reason: things went wrong}
2015-08-26T06:39:29Z Debug: Starting application, here is some detailed info. Data {info: interesting info}
```

# Installing

## Download

All you need to use this library is to put ```Logging.h``` in your project and include it in your code.

## Nuget

There is also a Nuget package for Visual Studio users, more info at:

https://www.nuget.org/packages/WildLogging

The correct include path to use after installing the Nuget package is

```C++
#include "Wild/Logging.h"
```

# Using

##Setting Up

Call ```SetupLogging()``` before starting to log, it optionally accepts a debug level parameter.

The library will write errors to stderr and all other messages to stdout by default. A log file can be specified by using ```AddFileDestination(path)``` which will open a log file for writing.

##Logging

A typical log function looks like this:

```Info(doing, result, [info_blob], [info])```

Supported types are ```Info, Warning, Error & Debug```

Param  | Description
------------- | -------------
doing  | what the code is trying to accomplish right now, usually contains a verb e.g. connecting to database
result | what happened e.g. connection failed
info_blob | optional prebuilt list of name value pairs
info | list of name value pairs for this message only

One difference from other logging libraries is the requirement to add two messages. This is a way to improve the readability and usefulness of the logs. We used this general idea on a project a few years ago and found that almost everything you want to log can be expressed this way. Credit for this idea goes to our user experience expert Ailene (http://oldmountainart.com/).

| doing  | what the code is trying to accomplish right now, usually contains a verb e.g. connecting to database |
| result | what happened e.g. connection failed | 
| info_blob | optional prebuilt list of name value pairs |
| info | list of name value pairs for this message only |


## Platforms

It was built on VS2013 but should work on earlier versions though it does require C++11 features, and tested on gcc 4.8.2. Info in this document is VS specific but all the concepts translate to Linux. The Linux build uses cmake, to test the lib it you can clone it, enter the directory and use

```
cmake .
make
Test\LoggingTest
```

