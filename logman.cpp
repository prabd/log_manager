// Project Identifier : 01BD41C3BF016AD7E8B6F837DF18926EC3E83350

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
inline int strcasecmp(const char *s1, const char *s2) {
	return _stricmp(s1, s2);
}
#endif // _MSC_VER

/**
 * Struct to store entries
 */
struct Entry {
	unsigned int id;
	std::string ts;
	long long int time;
	std::string cat;
	std::string msg;

	// Ctor
	Entry(const unsigned int & i, const std::string &ti, const long long int & t, const std::string & c, const std::string & m)
		: id{ i }, ts{ ti }, time{ t }, cat{ c }, msg{ m } {}

	void print() {
		cout << id << "|" << ts << "|" << cat << "|" << msg << "\n";
	}
};


// Helper function to convert from mm:dd:hh:mm:ss to a value
long long int timeToNum(std::string &ts) {
	std::istringstream input_str(ts);
	std::string str;
	for(int i = 0; i < 4; ++i) {
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
void toLower(std::string &s) {
	// Transform
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

/**
 * Function converts a given string into an alphanumeric string
 * Returns a vector of split words, transformed to lowercase
 */
void convert_alnum(const std::string &s, std::vector<std::string> &vec) {
	std::string word;
	for(char c : s) {
		// If not alphanumeric
		if(!isalnum(c)) {
			// If word is not empty, append to vec
			if(!word.empty()) {
				toLower(word); // convert to lower case
				vec.push_back(word);
			}
			// Flush word
			word.clear();
		}
		// If alphanumeric, append character to word
		else {
			word += c;
		}
	}
	// append last word if it exists
	if(!word.empty()) {
		toLower(word);
		vec.push_back(word);
	}
}

/**
 * Predicate for case insensitivity
 */
class stringPred {
public:
	bool operator()(const std::string &lhs, const std::string &rhs) const {
		return strcasecmp(lhs.c_str(), rhs.c_str()) == 0;
	}
};

/**
 * Custom Less to sort entries
 */
class EntryLess {
public:
	// Compares by timestamp, category, then ID
	// Entry is less if timestamp is less, cat is less, entry id is less
	bool operator()(const Entry* lhs, const Entry* rhs) const {
		if(lhs->time < rhs-> time) {
			return true;
		}
		else if(lhs->time > rhs->time) {
			return false;
		}
		// If timestamps are equal
		else {
			int val = strcasecmp(lhs->cat.c_str(), rhs->cat.c_str());
			if(val < 0) {
				return true;
			}
			else if(val > 0) {
				return false;
			}
			// If cats are equal
			else {
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
	bool operator()(long long int val, Entry* rhs) const {
		return val < rhs->time;
	}
};
class EntryTimeLessLower
{
public:
	bool operator()(Entry* lhs, long long int val) const {
		return lhs->time < val;
	}
};

/**
 * Main structure, used to organize functions
 */
class Logger {
	
	std::string logfile; // stores filename
	std::vector<Entry*> master;
	std::vector<unsigned int> id_to_index; // stores where an entryID can be found in master
	std::unordered_map<std::string, std::vector<unsigned int>> cats; // unordered map of categories to entries
	std::unordered_map<std::string, std::vector<unsigned int>> keywords;
	std::deque<unsigned int> excerpts; // stores indices to master
	std::vector<unsigned int> recents;
	std::vector<Entry*>::iterator begin_time_it;
	std::vector<Entry*>::iterator end_time_it;
	std::vector<unsigned int>::iterator begin_cat_it;
	std::vector<unsigned int>::iterator end_cat_it;
	std::vector<unsigned int> key_search_vec;
	char search_type = '0';
	bool changed_recents = false; // flag to see if recents may have changed
	bool sorted_excerpts = false; // flag to see if excerpts were sorted already
	bool clear_recents = false; // Flag to clear recents
	std::string prev_key_search;
	bool created_category_map = false;
	bool created_keyword_map = false;
	
public:
	/**
	 * Custom Dtor, prevent memory leaks
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
				std::string all;
				cin >> all;
				std::string t1, t2;

				if(all[14] != '|' || all.size() != 29) {
					std::cerr << "Input formatted wrong\n";
				}
				else {
					t1 = all.substr(0, 14);
					t2 = all.substr(15, 14);
					int num = time_search_range(t1, t2);
					cout << "Timestamps search: " << num << " entries found\n";
				}
			}
			
			// Time search
			else if(cmd == 'm')
			{
				std::string t1;
				cin >> t1;
				int num = time_search_range(t1, t1); // use same function as above
				cout << "Timestamp search: " << num << " entries found\n";
			}
			else if(cmd == 'c')
			{
				if(!created_category_map) {
					create_category_map();
					created_category_map = true;
				}
				std::string str;
				getline(cin, str);

				// Erase first space
				str.erase(0, 1);
	
				category_search(str);
			}
			else if(cmd == 'k')
			{
				if(!created_keyword_map) {
					create_keyword_map();
					created_keyword_map = true;
				}
				std::string str;
				std::getline(cin, str);
				keyword_search(str);
			}
			else if(cmd == 'a')
			{
				int num;
				cin >> num;
				append_log_entry(num);
			}
			else if(cmd == 'r')
			{
				append_search_results();
			}
			else if(cmd == 'd')
			{
				int num = -1;
				cin >> num;
				delete_log_entry(num);
			}
			else if(cmd == 'b')
			{
				int num = -1;
				cin >> num;
				move_to_begin(num);
			}
			else if(cmd == 'e')
			{
				int num = -1;
				cin >> num;
				move_to_end(num);
			}
			else if(cmd == 's')
			{
				sort_excerpts();
			}
			else if(cmd == 'l')
			{
				// If empty
				if (excerpts.empty()) {
					cout << "excerpt list cleared\n(previously empty)\n";
				}
				// If not empty
				else {
					cout << "excerpt list cleared\nprevious contents:\n";
					cout << '0' << "|";
					master[excerpts[0]]->print();

					cout << "...\n";

					cout << excerpts.size() - 1 << '|';
					master[excerpts[excerpts.size() - 1]]->print();
					excerpts.clear();
				}
			}
			else if(cmd == 'g')
			{
				print_recents();
			}
			else if(cmd == 'p')
			{
				print_excerpts();
			}
			else if(cmd == '#')
			{
				std::string junk;
				std::getline(cin, junk);

			}
		} while (cmd != 'q');
	}

private:
	/*
	 * Creates category map for the first time
	 */
	void create_category_map() {
		for (int i = 0; i < int(master.size()); ++i)
		{
			Entry* entry = master[i]; // reference to current entry
			// Transform category into lower case, then hash
			std::string cat = entry->cat;
			// need lowercase for standardized hashing
			toLower(cat);
			cats[cat].push_back(i); //push as is
		}
	}

	/*
	 * Creates keyword map for the first time
	 */
	void create_keyword_map() {
		for (int i = 0; i < int(master.size()); ++i) {
			Entry* entry = master[i];
			std::string cat = entry->cat;
			// Split cat into vector of alphanumeric words, convert to lower case
			std::vector<std::string> cat_split;
			convert_alnum(cat, cat_split);
			// Split msg into vector of alphanumeric words, convert to lower case
			std::string msg = entry->msg;
			std::vector<std::string> msg_split;
			convert_alnum(msg, msg_split);
			// Merge vectors by moving cat split to msg split (cat should be less than msg)
			msg_split.reserve(msg_split.size() + cat_split.size()); // reserve
			std::move(cat_split.begin(), cat_split.end(), std::back_inserter(msg_split)); // merge into msg

			// Hash into table, if vec does not exist, or last element in vec is not current entry, push back
			for (const auto& word : msg_split)
			{
				// If key does exist
				if (keywords.find(word) != keywords.end())
				{
					auto vec = (keywords[word]);
					// If previous element in vector is not equal to current entry
					if (int(keywords[word][keywords[word].size() - 1]) != i)
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
	
	/*
	 * Helper function to update recents based on previous search
	 * MODIFIES: recents, changed_recents
	 */
	void update_recents()
	{
		if(clear_recents) {
			recents.clear();
		}
		// If recents may have changed, then update, otherwise do not.
		else if (changed_recents) {
			if (search_type == 't') {
				// Update recents
				unsigned int index1 = unsigned(begin_time_it - master.begin()); //find index of first match
				unsigned int num_el = unsigned(end_time_it - begin_time_it); // total matches

				changed_recents = false;

				std::vector<unsigned int> temp;
				temp.resize(num_el, 0);
				std::iota(temp.begin(), temp.end(), index1);
				recents.swap(temp); // vector swap
				temp.clear();
			}
			// was a category or keyword search
			else if (search_type == 'c') {
				unsigned int num_el = unsigned(end_cat_it - begin_cat_it);
				changed_recents = false;
				recents.resize(num_el, 0);
				std::copy(begin_cat_it, end_cat_it, recents.begin());
			}
			else if (search_type == 'k') {
				changed_recents = false;
				recents.swap(key_search_vec);
				key_search_vec.clear();
			}
		}
	}

	/** HELPER
	 * Helper Function uses lower bound and upper bound to generate search results in recent
	 * sets changed_recents = true
	 * MODIFIES: search_made, changed_recents, begin/end iterators, was_time
	 */
	int time_search_range(std::string &lower, std::string &upper) {	
		long long int low = timeToNum(lower);
		long long int up = timeToNum(upper);
		auto it1 = std::lower_bound(master.begin(), master.end(), low, EntryTimeLessLower());
		auto it2 = std::upper_bound(it1, master.end(), up, EntryTimeLessUpper());

		// Update previous search stuff
		search_type = 't';
		changed_recents = true;
		clear_recents = false;
		begin_time_it = it1;
		end_time_it = it2;
		
		int num_el = int(it2 - it1);
		return num_el;
	}
	
	/** HELPER
	 *  Function for category search, 'c'
	 */
	void category_search(const std::string &cat) {
		search_type = 'c'; // was not time search
		changed_recents = true;
		int num_el = 0;
		
		// Convert to lower case for search
		std::string converted(cat);
		toLower(converted);
		// Check if string exist in map
		if (cats.find(converted) != cats.end()) {
			begin_cat_it = cats[converted].begin();
			end_cat_it = cats[converted].end();
			num_el = int(end_cat_it - begin_cat_it);
			clear_recents = false;
		}
		else {
			clear_recents = true;
		}
		cout << "Category search: " << num_el << " entries found\n";
	}
	
	/** HELPER
	 * Function for keywords search, 'k'
	 */
	void keyword_search(const std::string &words) {
		// If previous search was exact same, return
		if(prev_key_search == words && search_type == 'k') {
			changed_recents = false;
			clear_recents = false;
		}
		
		search_type = 'k';
		clear_recents = false;
		changed_recents = true;
		
		// Get vector of words
		std::vector<std::string> word_vec;
		convert_alnum(words, word_vec);

		// Start results with first word
		// If first word isn't found, just clear key_search_vec
		if(keywords.find(word_vec[0]) == keywords.end()) {
			key_search_vec.clear();
		}
		else {
			std::vector<unsigned int> result = keywords[word_vec[0]];
			// Loop through all words starting at index 1, and perform set intersection
			for(int i = 1; i < int(word_vec.size()); ++i) {
				// If result vector is empty, or current word is not found, clear result, and break
				if(result.empty() || (keywords.find(word_vec[i]) == keywords.end())) {
					result.clear();
					break;
				}
				// Else word is found and result is non empty
				else {
					// Get vec of word, then perform union with result
					auto it1 = keywords[word_vec[i]].begin();
					auto it2 = keywords[word_vec[i]].end();
					std::vector<unsigned int> temp;
					// Write to temp
					std::set_intersection(result.begin(), result.end(), it1, it2, std::back_inserter(temp));
					result.swap(temp); // swap with temp
				}
			}
			key_search_vec.swap(result); // swap with main vec
		}
		cout << "Keyword search: " << key_search_vec.size() << " entries found\n";
	}

	/* HELPER
	 * Helper function for 'a'
	 * MODIFIES: excerpts, sorted_excerpts
	 */
	void append_log_entry(const int a) {

		// Error check
		if(a < int(master.size()) && a >= 0){
			unsigned int index = id_to_index[a]; // convert
			excerpts.push_back(index);
			cout << "log entry " << a << " appended\n";
			sorted_excerpts = false;
		}
	}
	
	/** HELPER
	* Function appends search results to end of deque, 'r'
	* MODIFIES: excerpts, sorted_excerpts
	*/
	void append_search_results() {
		// If previous search was made
		if (search_type != '0') {
			update_recents();
			// Error check
			if (!recents.empty()) {
				for (const unsigned int& entry : recents) {
					excerpts.push_back(entry);
				}
				sorted_excerpts = false; // excerpts change here
			}
			// Print number of entries
			cout << recents.size() << " log entries appended\n";
		}
		// Else, do nothing
	}

	/** HELPER
	 * Deletes log entry at position a in excerpts, 'd'
	 * MODIFIES: excerpts
	 */
	void delete_log_entry(const int a) {
		// DOES NOT MODIFY sorted_excerpts B/C no change in order
		if (a < int(excerpts.size()) && a >= 0) {
			auto it = excerpts.begin() + a;
			excerpts.erase(it);
			cout << "Deleted excerpt list entry " << a << "\n";
		}
	}

	/** HELPER
	 * Moves log entry at position a to beginning, 'b'
	 * MODIFIES: excerpts, sorted_excerpts
	 */
	void move_to_begin(const int a) {
		if(a < int(excerpts.size()) && a >= 0) {
			auto it = excerpts.begin() + a;
			unsigned int val = *it;
			excerpts.erase(it);
			excerpts.push_front(val); // push to front
			sorted_excerpts = false;
			cout << "Moved excerpt list entry " << a << '\n';
		}
	}
	
	/** HELPER
	* Moves log entry at position a to end, 'e'
	* MODIFIES: excerpts, sorted_excerpts
	*/
	void move_to_end(const int a) {
		if (a < int(excerpts.size()) && a >= 0) {
			auto it = excerpts.begin() + a;
			unsigned int val = *it;
			excerpts.erase(it);
			excerpts.push_back(val); // push to end
			sorted_excerpts = false;
			cout << "Moved excerpt list entry " << a << '\n';
		}
	}

	/** HELPER
	 * Sorts excerpt list.
	 * MODIFIES: excerpts, sorted_excerpts
	 */
	void sort_excerpts() {
		
		if(excerpts.empty()) {
			cout << "excerpt list sorted\n(previously empty)\n";
			sorted_excerpts = true;
		}
		else {
			cout << "excerpt list sorted\nprevious ordering:\n";
			cout << '0' << "|";
			master[excerpts[0]]->print();
			
			cout << "...\n";

			cout << excerpts.size() - 1 << "|";
			master[excerpts[excerpts.size() - 1]]->print();
			
			// Sorts ascending integers.
			// Since the integers in excerpts refer to the sorted indices of the master,
			// we only have to sort the values in excerpts.
			if (!sorted_excerpts) {
				//std::sort(excerpts.begin(), excerpts.end());
				count_sort(); // custom sort
				sorted_excerpts = true;
			}
			
			cout << "new ordering:\n";
			cout << '0' << "|";
			master[excerpts[0]]->print();
			
			cout << "...\n";

			cout << excerpts.size() - 1 << "|";
			master[excerpts[excerpts.size() - 1]]->print();
		}
	}
	
	/** HELPER
	 * Prints recent search, 'g'
	 */
	void print_recents() {
		update_recents();
		for(const unsigned int& index : recents) {
			master[index]->print();
		}
	}

	/** HELPER
	 * Prints excerpts, 'p'
	 * 
	 */
	void print_excerpts() const {
		for (int index = 0; index < int(excerpts.size()); ++index) {
			cout << index << "|";
			master[excerpts[index]]->print();
		}
	}

	/*
	 * Implementation of counting sort for better speed
	 */
	void count_sort() {
		// Find range of indices
		unsigned int max = *std::max_element(excerpts.begin(), excerpts.end());
		unsigned int min = *std::min_element(excerpts.begin(), excerpts.end());

		// Create vector of counts
		std::vector<unsigned int> counts(max - min + 1, 0);

		// Store counts
		for(unsigned int index : excerpts) {
			++counts[index - min]; // shift by min
		}
		// sum counts at each index
		for(unsigned int i = 1; i < counts.size(); ++i) {
			counts[i] += counts[i - 1];
		}
		// Update out vector (copy into deque later)
		std::vector<unsigned int> out(excerpts.size(), 0);
		for(int i = int(excerpts.size()) - 1; i >= 0; --i) {
			out[counts[excerpts[i] - min] - 1] = excerpts[i];
			// Decrease count
			--counts[excerpts[i] - min];
		}

		// copy out vector into excerpts deque
		for(unsigned int i = 0; i < excerpts.size(); ++i) {
			excerpts[i] = out[i];
		}
		
	}

};

int main(int argc, char* argv[]) {
	std::ios_base::sync_with_stdio(false);

	Logger log;
	log.read_cmd(argc, argv);
	log.read_logfile();
	//log.create_maps();
	log.read_commands();
	return 0;
}