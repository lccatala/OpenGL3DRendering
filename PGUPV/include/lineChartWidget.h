#pragma once
// 2018

#include "widget.h"
#include <vector>

/**
\class LineChartWidget
*/

namespace PGUPV {
	class LineChartWidget : public Widget {
	public:
		LineChartWidget(const std::string &label, size_t size, size_t height, size_t size_summary = 0);
		void pushValue(float v);
		void clear();
		void renderWidget() override;
	protected:
		std::vector<float> values;
		size_t nextPos, widgetHeight, showSummaryLastNValues;
	};
};
