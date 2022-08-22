//
//  outputStreamStats.cpp
//  PG
//
//  Created by Paco Abad on 24/12/15.
//
//

#include <string>
//#include <iostream>

#include "include/outputStreamStats.h"


using PGUPV::OutputStreamStats;
using PGUPV::StatsClass;

OutputStreamStats::OutputStreamStats(std::shared_ptr<std::ostream> stream) : stream(stream) {
    
}

StatsClass &OutputStreamStats::pushValue(const std::string &v) {
	*stream << v << separator;
	return *this;
}

void OutputStreamStats::endFrame() {
    stream->seekp(-static_cast<long>(separator.length()), std::ios_base::cur);
    *stream << std::endl;
}

