#include "glStats.h"

using PGUPV::GLStats;

GLStats::GLStats() : queries{ GL_SAMPLES_PASSED, GL_PRIMITIVES_GENERATED, GL_VERTICES_SUBMITTED_ARB, GL_PRIMITIVES_SUBMITTED_ARB,
GL_VERTEX_SHADER_INVOCATIONS_ARB, GL_TESS_CONTROL_SHADER_PATCHES_ARB, GL_TESS_EVALUATION_SHADER_INVOCATIONS_ARB,
GL_GEOMETRY_SHADER_INVOCATIONS, GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED_ARB, GL_FRAGMENT_SHADER_INVOCATIONS_ARB,
GL_COMPUTE_SHADER_INVOCATIONS_ARB, GL_CLIPPING_INPUT_PRIMITIVES_ARB, GL_CLIPPING_OUTPUT_PRIMITIVES_ARB }, collectExtendedStatsFlag(false) {
	std::fill(values, values + NQueries, 0);
}

void GLStats::beginFrame() {
	unsigned int end = 2;
	if (collectExtendedStatsFlag)
		end = NQueries;
	for (unsigned int i = 0; i < end; i++) {
		queries[i].begin();
	}
	stopwatch.restart();
}

void GLStats::endFrame() {
	unsigned int end = 2;

	elapsedMs = stopwatch.getElapsed() * 0.001f; // us -> ms
	if (collectExtendedStatsFlag)
		end = NQueries;
	for (unsigned int i = 0; i < end; i++) {
		queries[i].end();
	}
}


bool GLStats::collectExtendedStats(bool extendedStats)
{
	if (extendedStats) {
		if (GLEW_ARB_pipeline_statistics_query)
		{
			collectExtendedStatsFlag = extendedStats;
		}
		else
			return false;
	}
	return true;
}

uint64_t GLStats::getValue(Query query) {
	return queries[PGUPV::to_underlying(query)].getValueU64();
}
