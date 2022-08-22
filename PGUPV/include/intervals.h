
#ifndef _INTERVALS_H
#define _INTERVALS_H 2013

#include <list>
#include "common.h"

namespace PGUPV {
	struct Interval {
		ulong start, end;
	};

	class Intervals {
	public:
		// Stores the given interval (e.g., addInterval(0,0), addInterval(0, 99), etc.)
		void addInterval(ulong start, ulong end);
		void addIntervals(Intervals other);
		// Checks whether v is in an interval, and removes all elements less or equal to v
		// Returns true if v was in an interval, or false otherwise
		bool popValue(ulong v);
		// Returns true if there is no intervals left
		bool empty();
	private:
		std::list<Interval> intervals;
		void consolidate();
	};

	// argv[pos] should contain a '{'. Then it parses arguments until it finds a '}'
	// This function recognizes individual values [...], 123, [...], and ranges [...], 10-200, [...]
	// It also advances pos to the argument following the one with '}'
	Intervals readIntervalsFromCommandLine(int &pos, int argc, const char *argv[]);
	// Parses a string like 1,2,3-10,20-30, 40, and returns the corresponding intervals
	Intervals readIntervalsFromString(const std::string &str);
};

#endif
