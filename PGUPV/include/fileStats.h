#pragma once

#include "outputStreamStats.h"

namespace PGUPV {

class FileStats : public OutputStreamStats {
public:
	explicit FileStats(const std::string &filename);
    ~FileStats();
};
};
