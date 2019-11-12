// Project Identifier: C3BF016AD7E8B6F837DF18926EC3E83350


#include <string.h>
#include <deque>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
using std::cin;
using std::cout;

#ifdef _MSC_VER
inline int strcasecmp(const char *s1, const char *s2)
{
	return _stricmp(s1, s2);
}
#endif // _MSC_VER

/**
 * Struct to store entries
 */
struct Entry
{
	int id;
	uint64_t time;
	std::string cat;
	std::string msg;

	// Ctor
	Entry(const int i, const uint64_t ts, const std::string c, const std::string m)
	: id{ i }, time{ ts }, cat{ c }, msg{ m } {}
};

// Helper function to convert from mm:dd:hh:mm:ss to a value
uint64_t convert_time(std::string &ts)
{
	uint64_t time = 0;
	time += uint64_t(std::stoi(ts.substr(0, 2))) * 100000000;
	time += uint64_t(std::stoi(ts.substr(3, 2))) * 1000000;
	time += uint64_t(std::stoi(ts.substr(6, 2))) * 10000;
	time += uint64_t(std::stoi(ts.substr(9, 2))) * 100;
	time += uint64_t(std::stoi(ts.substr(12, 2)));
	return time;
}

/**
 * Predicate for case insensitivity
 */
class stringPred
{
	bool operator()(const std::string &lhs, const std::string &rhs) {
		return strcasecmp(lhs.c_str(), rhs.c_str()) == 0;
	}
};

class Logger
{
	std::string logfile; // stores filename
public:
	
	/*
	 * THIS FUNCTION READS COMMAND LINE AND UPDATES THE PARAMETERS OF THE HUNT
	 */
	void get_options(int argc, char* argv[])
	{
		// If no -h or filename specified
		if (argc != 2)
		{
			exit(1);
		}
		std::string cmd(argv[1]);
		if (cmd == "-h" || cmd == "--help")
		{
			cout << "This program reads from a log file and allows you to access entries, move them around, and print them out." << std::endl;
				exit(0);
		}
		// Else
		logfile = cmd;
	} //get_options

	
	/**
	 * This function opens the file, processes it, and stores the entries in a vector
	 */
	void read_logfile()
	{
		
	}
};

int main(int argc, char* argv[])
{
	std::string str = "99:88:77:66:55";
	uint64_t time = convert_time(str);
	cout << time;
}