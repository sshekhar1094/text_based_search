To compile the program, the system must have Boost library installed in the system.
Installation and build instructions can be found at - 
	(for UNIX variants) - http://www.boost.org/doc/libs/1_59_0/more/getting_started/unix-variants.html
	(for Windows) - http://www.boost.org/doc/libs/1_59_0/more/getting_started/windows.html

After building the libary from source the program can be compiled using the following command(without quotes) - 	
	"g++ searchText.cpp -I /path/to/boost_1_59_0 -lboost_system -lboost_filesystem"

	Example - 
	g++ searchText.cpp -I /usr/local/boost_1_59_0 -lboost_system -lboost_filesystem

Command to run the file(without quotes) - 
	"./a.out /path/to/dataset"

	Example(To run the file with the given data set) -
	./a.out ./Dataset/

**If the library binaries are installed at a location where the linker cannot by default find them, mention the location followed by -L while compiling.

**The path to the dataset has to be supplied as command line argument The program assumes that the dataset consists only of .txt documents.