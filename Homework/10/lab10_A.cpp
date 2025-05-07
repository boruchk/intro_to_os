/*
	lab10_A.cpp
	
	Boruch Khazanovich
	bk2780
*/

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <ctime>

using namespace std;


vector <int> pageTraceGenerator();
int simulate(const vector <int>& trace, int f);
size_t findVictim(vector <unsigned long int>& page_table);
size_t next_ptr = 0;

int n = 0;
int p = 0;


int main(const int argc, char* argv[]) {
	if (argc < 3) {
		cout << "usage " << argv[0] << "<n> <p>" << endl;
		return -1;
	}

	n = atoi(argv[1]);
	p = atoi(argv[2]);
	if (n < 16 || p < 8) {
		cout << "n must be >= 16" << endl << "p must be >= 8" << endl;
		return -1;
	}
	cout << "n = " << n << endl;
	cout << "p = " << p << endl;

	const vector <int> trace = pageTraceGenerator();
	cout << "trace = ";
	for (int i = 0; i < trace.size(); i++) cout << trace[i] << " ";
	cout << endl;

	int page_faults = 0;

	ofstream file("output.csv");
	for (int f = 4; f < p; f++) {
		const int faults = simulate(trace, f);
		// file << f << "," << faults << endl;
		cout << "faults with f = " << f << " : " << faults << endl;
		page_faults += faults;
		next_ptr = 0;
	}
	file.close();

	cout << "The total number of page faults is: " << page_faults << endl;
	return 0;
}


vector <int> pageTraceGenerator() {
	vector <int> trace(n, 0);
	srand(time(NULL));
	for (int i = 0; i < n+1; i++) {
		trace[i] = rand() % p;
	}
	return trace;
}


int simulate(const vector <int>& trace, const int f) {
	vector <unsigned long int> page_table(p, 0);
	int num_page_faults = 0;
	int frame_ct = f;

	for (const size_t cur_page_num : trace) {
		// check if the page is currently in memory
		// present bit set = no fault
		if (page_table[cur_page_num] & 0x00000002) {
			// set reference bit
			page_table[cur_page_num] |= 0x00000001;
			continue;
		}

		unsigned long int frame_number = 0;
		// find a free frame if available.
		// available frame = no page fault
		if (frame_ct) {
			frame_number = frame_ct << 2;
			frame_ct--;
		}
		else {
			// no available frame = page fault
			// find a victim page to evict
			const size_t victim = findVictim(page_table);
			frame_number = page_table[victim] >> 2;
			// reset the victim page
			page_table[victim] = 0;
		}

		// set present and reference bits
		page_table[cur_page_num] |= 0x00000003;
		// assign frame (load into memory)
		page_table[cur_page_num] |= (frame_number << 2);
		num_page_faults++;
	}
	return num_page_faults;
}


size_t findVictim(vector <unsigned long int>& page_table) {
	while (true) {
		// page is not in memory (does not have a frame) so ignore it
		if (!(page_table[next_ptr])) {
			next_ptr = (next_ptr + 1) % p;
			continue;
		}

		// check if the reference bit is not set (found a victim)
		if (!(page_table[next_ptr] & 0x00000001)) return next_ptr;

		// reference bit was set.
		// unset the reference bit and continue searching for a victim
		page_table[next_ptr] &= 0xFFFFFFFE;
		next_ptr = (next_ptr + 1) % p;
	}
}