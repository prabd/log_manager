// Project Identifier: C3BF016AD7E8B6F837DF18926EC3E83350


#include <string.h>
#include <algorithm>
#include <numeric>
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
	std::string ts;
	long long int time;
	std::string cat;
	std::string msg;

	// Ctor
	Entry(const int i,const std::string &ti, const long long int t, const std::string c, const std::string m)
		: id{ i }, ts{ ti }, time{ t }, cat{ c }, msg{ m } {}

	void print()
	{
		cout << id << "|" << ts << "|" << cat << "|" << msg << "\n";
	}
};


// Helper function to convert from mm:dd:hh:mm:ss to a value
long long int timeToNum(std::string &ts)
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
std::string toLower(std::string &s)
{
	// Transform
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

/**
 * Function converts a given string into an alphanumeric string
 * Returns a vector of split words, transformed to lowercase
 */
std::vector<std::string> convert_alnum(const std::string &s)
{
	std::vector<std::string> vec;
	std::string word;
	for(char c : s)
	{
		// If not alphanumeric
		if(!isalnum(c))
		{
			// If word is not empty, append to vec
			if(word.length() > 0)
			{
				toLower(word); // convert to lower case
				vec.push_back(word);
			}
			// Flush word
			word.clear();
		}
		// If alphanumeric, append character to word
		else
		{
			word += c;
		}
	}
	// append last word if it exists
	if(!word.empty())
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
	bool operator()(const std::string &lhs, const std::string &rhs) const {
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
	bool operator()(const Entry* lhs, const Entry* rhs) const
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

/*
 * Custom less for timestamp searches
 */
class EntryTimeLessUpper
{
public:
	bool operator()(long long int val, Entry* rhs) const
	{
		return val < rhs->time;
	}
};
class EntryTimeLessLower
{
public:
	bool operator()(Entry* lhs, long long int val) const
	{
		return lhs->time < val;
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
	std::unordered_map < std::string, std::vector<int>> cats; // unordered map of categories to entries
	std::unordered_map<std::string, std::vector<int>> keywords;
	std::deque<int> excerpts;
	std::vector<int> recents;
	
public:
	/**
	 * Custom Dtor
	 */
	~Logger()
	{
		for(auto entry : master)
		{
			delete entry;
		}
	}
	
	/* SETUP
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

	
	/** SETUP
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
			Entry* en = new Entry(num, ts, timeToNum(ts), cat, msg);
			master.push_back(en);
			++num;
		}

		// Print number of entries read
		cout << master.size() << " entries read\n";

		sort_master();
	}

	/** SETUP
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

	/** SETUP
	 * Iterates through sorted master, parses categories and messages
	 * Hashes maps
	 */
	void create_maps()
	{
		// Iterate over entries in master
		for(int i = 0; i < int(master.size()); ++i)
		{
			Entry* entry = master[i]; // reference to current entry
			// Transform category into lower case, then hash
			std::string cat = entry->cat;
			// need lowercase for standardized hashing
			toLower(cat);
			cats[cat].push_back(i); //push as is

			// Update map for keywords
			// Split cat into vector of alphanumeric words, convert to lower case
			std::vector<std::string> cat_split = convert_alnum(cat);
			// Split msg into vector of alphanumeric words, convert to lower case
			std::string msg = entry->msg;
			std::vector<std::string> msg_split = convert_alnum(msg);
			// Merge vectors by moving cat split to msg split (cat should be less than msg)
			msg_split.reserve(msg_split.size() + cat_split.size()); // reserve
			std::move(cat_split.begin(), cat_split.end(), std::back_inserter(msg_split)); // merge into msg
			
			// Hash into table, if vec does not exist, or last element in vec is not current entry, push back
			for(const auto& word : msg_split)
			{
				// If key does exist
				if(keywords.find(word) != keywords.end())
				{
					auto vec = (keywords[word]);
					// If previous element in vector is not equal to current entry
					if (keywords[word][keywords[word].size() - 1] != i)
					{
						keywords[word].push_back(i);
					}
					// Last entry in vector is same entry, so don't do anything
				}
				// Else if key does not exist, simply push back
				else
				{
					keywords[word].push_back(i);
				}
			}
		}
	}

	/** MAIN
	 * Process user commands and acts accordingly
	 */
	void read_commands()
	{
		char cmd;
		do
		{
			cout << "% ";
			cin >> cmd;
			
			// Time range search
			if(cmd == 't')
			{
				std::string t1, t2;
				getline(cin, t1, '|');
				// Strip leading space from t1
				t1.erase(0, 1);
				getline(cin, t2);
				// Error checking
				if(t1.length() != 14 || t2.length() != 14)
				{
					std::cerr << "Input formatted wrong\n";
				}
				else {
					time_search_range(t1, t2);
					cout << "Timestamps search: " << recents.size() << " entries found\n";
				}
			}
			
			// Time search
			else if(cmd == 'm')
			{
				std::string t1;
				cin >> t1;
				time_search_range(t1, t1); // use same function as above
				cout << "Timestamp search: " << recents.size() << " entries found\n";
			}

			// Print recents
			else if(cmd == 'g')
			{
				print_recents();
			}
		} while (cmd != 'q');
	}

private:
	/** HELPER
	 * Helper Function uses lower bound and upper bound to generate search results in recent
	 */
	void time_search_range(std::string &lower, std::string &upper)
	{
		recents.clear();
		
		EntryTimeLessLower e1;
		EntryTimeLessUpper e2;
		long long int low = timeToNum(lower);
		long long int up = timeToNum(upper);
		auto it1 = std::lower_bound(master.begin(), master.end(), low, e1);
		auto it2 = std::upper_bound(it1, master.end(), up, e2);
	
		// Update recents
		int index1 = it1 - master.begin(); //find index of first match
		int num_el = it2 - it1; // total matches
		std::vector<int> temp;
		temp.resize(num_el, 0);
		std::iota(temp.begin(), temp.end(), index1);
		recents.swap(temp); // vector swap
	}



	/** HELPER
	 * Prints recent search
	 */
	void print_recents()
	{
		for(int& index : recents)
		{
			master[index]->print();
		}
	}

public:	
	/** HELPER
	 * Prints contents of master 
	 */
	void print_master()
	{
		for(int i = 0; i < int(master.size()); ++i)
		{
			cout << master[i]->id << " " << master[i]->ts << 
				" " << master[i]->cat << " " << master[i]->msg << "\n";
		}
	}
	
	// HELPER
	void test_conversion()
	{
		for (int id = 0; id < int(master.size()); ++id)
		{
			cout << master[id_to_index[id]]->id << " " << master[id_to_index[id]]->ts << 
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
	log.read_commands();
	return 0;
}