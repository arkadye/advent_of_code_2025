#include "advent/advent_of_code.h"
#include "advent/default_filters.h"

#include <iostream>

int main(int argc, char** argv)
{
	std::cout.sync_with_stdio(false);

	// Use the filter to only run certain tests.
	// This uses some magic to test against the name of the function,
	// so putting "eighteen" as the argument will only run advent_eighteen_p1()
	// and advent_eighteen_p2() (as well as any other test functions with "eighteen"
	// in the function name.
	// Leave blank to run everything.
	std::vector<std::string_view> filters;
	filters.reserve(argc - 1);
	for(int i=1;i<argc;++i)
	{
		filters.push_back(argv[i]);
	}

	if (filters.empty())
	{
		filters = std::move(advent::default_test_filters);
	}

	verify_all(filters);

#ifndef WIN32
	std::cout << "Program finished. Press any key to continue.";
	std::cin.get();
#endif
	return 0;
}