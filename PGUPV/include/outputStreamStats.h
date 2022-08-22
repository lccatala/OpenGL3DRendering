#pragma once

#include <memory>
#include <ostream>
#include "statsClass.h"

namespace PGUPV {
	class OutputStreamStats : public StatsClass {
	public:
		explicit OutputStreamStats(std::shared_ptr<std::ostream> stream);
		StatsClass &pushValue(const std::string &v) override;
		void endFrame() override;
        std::shared_ptr<StopWatch> makeStopWatch() override {
            return std::make_shared<MicroSecStopWatch>();
        };
	private:
		std::shared_ptr<std::ostream> stream;
	};
};
