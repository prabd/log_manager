// Project Identifier: C3BF016AD7E8B6F837DF18926EC3E83350


#include <string.h>
#include <algorithm>
#include <sstream>
#include <deque>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <algorithm>
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
long long int convert_time(std::string &ts)
{
	std::istringstream input_str(ts);
	std::string str;
	for(int i = 0; i < 4; ++i)
	{
		std::string s;
		std::getline(input_str, s, ':');
		str += s;
	}
	std::string s;
	std::getline(input_str, s);
	str += s;
	return std::stoll(str);
}

// Helper function to convert string to lower case
void toLower(std::string &s)
{
	// Transform
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

/**
 * Function converts a given string into an alphanumeric string
 * Returns a vector of split words, transformed to lowercase
 */
std::vector<std::string> convert_alnum(const std::string &s)
{
	std::vector<std::string> vec;
	std::string word = "";
	for(int i = 0; i < int(s.length()); ++i)
	{
		// If not alphanumeric
		if(!isalnum(s[i]))
		{
			// If word is not empty, append to vec
			if(word.length() > 0)
			{
				toLower(word); // convert to lower case
				vec.push_back(word);
			}
			// Flush word
			word = "";
		}
		// If alphanumeric, append character to word
		else
		{
			word += s[i];
		}
	}
	// append last word if it exists
	if(word != "")
	{
		vec.push_back(word);
	}
	return vec;
}
/**
 * Predicate for case insensitivity
 */
class stringPred
{
public:
	bool operator()(const std::string &lhs, const std::string &rhs) {
		return strcasecmp(lhs.c_str(), rhs.c_str()) == 0;
	}
};

/**
 * Custom Less to sort entries
 */
class EntryLess
{
public:
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

/**
 * Main structure, used to organize functions
 */
class Logger
{
	
	std::string logfile; // stores filename
	std::vector<Entry*> master;
	std::vector<int> id_to_index; // stores where an entryId can be found in master
	std::unordered_map < std::string, std::vector<Entry*>> cats; // unordered map of categories to entries
	std::unordered_map<std::string, std::vector<Entry*>> keywords;
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
	 * This function opens the file, sorts entries in master, and creates conversion vector
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

		sort_master();
	}

	/**
	 * This function sorts master and then creates a conversion vector
	 */
	void sort_master()
	{
		EntryLess comp;
		// Sort master
		std::sort(master.begin(), master.end(), comp);

		// Create vector of index conversions from entryID to index in master
		id_to_index.resize(master.size(), 0);
		for (int i = 0; i < int(id_to_index.size()); ++i)
		{
			id_to_index[master[i]->id] = i; // index at entryid is i
		}
	}

	/**
	 * Iterates through sorted master, parses categories and messages, and adds entry to vector
	 */
	// TODO
	void create_maps()
	{
		for(int i = 0; i < int(master.size()); ++i)
		{
			Entry* entry = master[i];
			// Transform category into lower case, then hash
			std::string cat = entry->cat;
			// need lowercase for standardized hashing
			toLower(cat);
			cats[cat].push_back(entry);

			// TODO
			// Update map for keywords
			// Split cat into vector of alphanumeric words, convert to lower case
			std::vector<std::string> cat_split = convert_alnum(cat);
			// Split msg into vector of alphanumeric words, convert to lower case
			std::string msg = entry->msg;
			std::vector<std::string> msg_split = convert_alnum(msg);
			// Merge vectors by moving cat split to msg split
			msg_split.reserve(msg_split.size() + cat_split.size()); // reserve
			std::move(cat_split.begin(), cat_split.end(), std::back_inserter(msg_split));
			// Hash into table, if vec does not exist, or last element in vec is not current entry, push back
			cout << msg << std::endl;
			for(int i = 0; i < int(msg_split.size()); ++i)
			{
				cout << msg_split[i] << " ";
			}
			cout << std::endl;
		}
		cout << cats["thread"].size() << std::endl;
	}
	
	/**
	 * Prints contents of master 
	 */
	void print_master()
	{
		for(int i = 0; i < int(master.size()); ++i)
		{
			cout << master[i]->id << " " << master[i]->time << 
				" " << master[i]->cat << " " << master[i]->msg << "\n";
		}
	}

	void test_conversion()
	{
		for (int id = 0; id < int(master.size()); ++id)
		{
			cout << master[id_to_index[id]]->id << " " << master[id_to_index[id]]->time << 
				" " << master[id_to_index[id]]->cat << " " << master[id_to_index[id]]->msg << "\n";
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
	//log.test_conversion();
	log.create_maps();

	std::string str = "load Print-''sTaTe,valid'brEakdOwn ";
	std::vector<std::string> words = convert_alnum(str);
	
	for(int i = 0; i < int(words.size()); ++i)
	{
		cout << "Word " << i << ": " << words[i] << "--\n";
	}
	
	return 0;
}