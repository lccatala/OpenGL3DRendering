
#include <sstream>
#include <algorithm>
#include <cassert>
#include <string.h>
#include <boost/tokenizer.hpp>

#include "log.h"
#include "common.h"
#include "intervals.h"

using PGUPV::Intervals;
using PGUPV::Interval;


inline bool inInterval(ulong v, Interval i) {
	return i.start <= v && v <= i.end;
}

inline bool intersects(Interval a, Interval b) {
	return inInterval(a.start, b) || inInterval(a.end, b) || inInterval(b.start, a) || inInterval(b.end, a);
}

Interval merge(Interval a, Interval b) {
	Interval result;

	result.start = MIN(a.start, b.start);
	result.end = MAX(a.end, b.end);
	return result;
}

void Intervals::addInterval(ulong start, ulong end) {
	Interval new_interval = {start, end};
	if (intervals.empty() || end < intervals.front().start) {
		intervals.push_front(new_interval);
	} else if (start > intervals.back().end) {
		intervals.push_back(new_interval);
	} else {
		std::list<Interval>::iterator p = intervals.begin();
		while (p != intervals.end()) {
			if (intersects(*p, new_interval)) {
				*p = merge(*p, new_interval);
				consolidate();
				return;
			} else if (p->start > new_interval.end) {
				intervals.insert(p, new_interval);
			}
			++p;
		}
	}
}

void Intervals::addIntervals(Intervals other) {
	for (std::list<Interval>::const_iterator i = other.intervals.begin(); i != other.intervals.end(); ++i) {
		addInterval(i->start, i->end);
	}
}

void Intervals::consolidate() {
	if (intervals.size() < 2) return;
	std::list<Interval>::iterator p, next;
	p = next = intervals.begin();
	++next;
	while (next != intervals.end()) {
		if (intersects(*p, *next)) {
			*next = merge(*p, *next);
			intervals.erase(p);
		} 
		p = next;
		++next;
	}
}

bool Intervals::popValue(ulong v) {
	while (!intervals.empty() && v >= intervals.front().start) {
		if (inInterval(v, intervals.front())) {
			intervals.front().start = v + 1;
			if (intervals.front().end < intervals.front().start) {
				intervals.pop_front();
			}			
			return true;
		} else {
			intervals.pop_front();
		}
	}
	return false;
}

bool Intervals::empty() {
	return intervals.empty();
}

Intervals PGUPV::readIntervalsFromCommandLine(int &pos, int argc, const char *argv[]) {
	// Build a string with the contents between the { }
	// argv[i] should start with '{'
	const char *brace = strchr(argv[pos], '{');
	if (brace == NULL) 
		ERRT(std::string("Se esperaba \'{\' en ") + argv[pos]);
	
	std::ostringstream os;
	os << (brace + 1);

	if (strchr(brace+1, '}') == NULL) {
		pos++;
		while (pos < argc && strchr(argv[pos], '}') == NULL) {
			os << argv[pos++];				
		}
		if (pos < argc) {
			os << argv[pos++];
		} else {
			ERRT(std::string("Se esperaba \'}\' en ") + argv[pos]);
		}
	} else pos++;

	// Clean the string: clip off from the }
    std::string str = os.str();
	std::string::size_type p = str.find('}');
	
	assert(p != std::string::npos);
	str.erase(p);

	return readIntervalsFromString(str);
}

Intervals PGUPV::readIntervalsFromString(const std::string &str) {
	Intervals result;

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(" ", ",-", boost::keep_empty_tokens);
    tokenizer tokens(str, sep);

	//std::string::const_iterator c = str.begin();
	enum State { INIT, READ_START, READ_END};

	State state = INIT;
	int start = -1, end = -1;

    for (tokenizer::iterator tok_iter = tokens.begin();
         tok_iter != tokens.end(); ++tok_iter) {
		
		switch (state) {
		case INIT:
			if (*tok_iter == "," || tok_iter->length() == 0) continue;
			try {
				start = std::stoi(*tok_iter);
			} catch (std::exception &) {
				ERRT("No se esperaba " + *tok_iter);
			}
			state = READ_START;
			break;
		case READ_START:
			if (*tok_iter == ",") {
				result.addInterval(start, start);
				state = INIT;
			} else if (*tok_iter == "-") {
				state = READ_END;
			} else {
				ERRT("No se esperaba " + *tok_iter);
			}
			break;
		case READ_END:
			try {
				end = std::stoi(*tok_iter);
			} catch (std::exception &) {
				ERRT("No se esperaba " + *tok_iter);
			}
			result.addInterval(start, end);
			state = INIT;
			break;
		}
	}
	if (state == READ_START) {
		result.addInterval(start, start);
	} 
	if (state == READ_END) {
		ERRT("Rango sin acabar");
	}
	return result;
}

//states
//	init
//	new number > new number [0..9]
//		> new range [-]
//			> end interval [, EOL]
//