// Project Identifier: C3BF016AD7E8B6F837DF18926EC3E83350


#include <string.h>
#include <deque>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
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

class EntryLess
{
	// Compares by timestamp, category, then ID
	// Entry is less if timestamp is less, cat is less, entry id is less
	bool operator()(const Entry* lhs, const Entry* rhs)
	{
		if(lhs->time < rhs-> time)
		{
			return true;
		}
		else if(lhs->time > rhs->time)
		{
			return false;
		}
		// If timestamps are equal
		else
		{
			int val = strcasecmp(lhs->cat.c_str(), rhs->cat.c_str());
			if(val < 0)
			{
				return true;
			}
			else if(val > 0)
			{
				return false;
			}
			// If cats are equal
			else
			{
				return lhs->id < rhs->id;
			}
		}
	}
};

class Logger
{
	
	std::string logfile; // stores filename
	std::vector<Entry*> master;
	std::vector<int> id_to_index; // stores where an entryId can be found in master
public:
	
	/*
	 * THIS FUNCTION READS COMMAND LINE ARGS
	 */
	void read_cmd(int argc, char* argv[])
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
	} //read_cmd

	
	/**
	 * This function opens the file, processes it, and stores the entries in a vector
	 * 
	 */
	void read_logfile()
	{
		std::ifstream input;
		input.open(logfile.c_str());

		std::string ts, cat, msg;
		int num = 0;
		while(getline(input, ts, '|')){
			std::getline(input, cat, '|');
			std::getline(input, msg);
			Entry* en = new Entry(num, convert_time(ts), cat, msg);
			master.push_back(en);
			++num;
		}

		// Print number of entries read
		cout << master.size() << " entries read\n";

		// Update
	}

	/**
	 * Prints contents of master 
	 */
	void print_master()
	{
		for(int i = 0; i < int(master.size()); ++i)
		{
			cout << master[i]->id << " " << master[i]->time << " " << master[i]->cat << " " << master[i]->msg << "\n";
		}
	}
};

int main(int argc, char* argv[])
{
	std::ios_base::sync_with_stdio(false);

	Logger log;
	log.read_cmd(argc, argv);
	log.read_logfile();
	log.print_master();
}