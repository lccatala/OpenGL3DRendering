//
//  fileStats.cpp
//  PG
//
//  Created by Paco Abad on 24/12/15.
//
//

#include <fstream>

#include "include/fileStats.h"

using PGUPV::FileStats;

FileStats::FileStats(const std::string &filename) :
OutputStreamStats(std::make_shared<std::ofstream>(filename))
{

}

PGUPV::FileStats::~FileStats() {

}
