#include <sstream>
#include <guipg.h>
#include <cfloat>

#include "lineChartWidget.h"

using PGUPV::LineChartWidget;


LineChartWidget::LineChartWidget(const std::string &label, size_t size, size_t height, size_t size_summary) :
	nextPos(0), widgetHeight(height), showSummaryLastNValues(size_summary) {
	assert(size >= size_summary);

	values.resize(size);
	setLabel(label);
}

void LineChartWidget::pushValue(float v) {
	values[nextPos++] = v;
	if (nextPos >= values.size())
		nextPos = 0;
}

void LineChartWidget::clear() {
	std::fill(values.begin(), values.end(), 0.0f);
	nextPos = 0;
}

void LineChartWidget::renderWidget() {
	std::string summ;
	if (showSummaryLastNValues > 0) {
		float accum = 0.0f;
		long i = static_cast<long>(nextPos - showSummaryLastNValues);
		if (i < 0) {
			size_t j = i + values.size();
			for (; j < values.size(); j++) accum += values[j];
			for (j = 0; j < nextPos; j++) accum += values[j];
		}
		else {
			for (size_t j = i; j < nextPos; j++) accum += values[j];
		}
		std::ostringstream out;
		out << "Avg: ";
		out.precision(2);
		out << std::fixed << (accum / showSummaryLastNValues);
		summ = out.str();
	}
	GUILib::PlotLines(summ, &values[0], values.size(), nextPos, label, FLT_MAX, FLT_MAX, glm::vec2(0, widgetHeight));
}
