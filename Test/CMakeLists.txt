
include_directories (../../UnitTesting)
include_directories (../)

add_executable (LoggingTest Logging.Test.cpp AdditionalTestFile.cpp)

add_custom_command(
	TARGET LoggingTest POST_BUILD
   	COMMAND LoggingTest
 )
