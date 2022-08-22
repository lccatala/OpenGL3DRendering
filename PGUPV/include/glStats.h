#pragma once

#include <GL/glew.h>
#include "stopWatch.h"
#include "glQuery.h"
#include "utils.h"

// 2018

namespace PGUPV {
	class GLStats {
	public:
		/**
		The queries ending with Ext are only collected when requested with collectExtendedStats(true);
		*/
		enum class Query {
			SamplesPassed, PrimitivesGenerated, VerticesSubmittedExt, PrimitivesSubmittedExt,
			VertexShaderInvocationsExt, TessControlShaderPatchesExt, TessEvalShaderInvocationsExt,
			GeometryShaderInvocationsExt, GeometryShaderPrimitivesEmittedExt, FragmentShaderInvocationsExt,
			ComputeShaderInvocationsExt, ClippingInputPrimitivesExt, ClippingOutputPrimitivesExt
		};
		GLStats();
		void beginFrame();
		void endFrame();
		/**
		Starts collecting extended statistics, if available
		\param if true, start collecting extended stats, stop otherwise
		\return true if the current environment supports extended stats (extension GL_ARB_pipeline_statistics_query)
		*/
		bool collectExtendedStats(bool extendedStats);
		uint64_t getValue(Query query);
		float getFrameDuration() { return elapsedMs; }
	private:
		constexpr static unsigned int NQueries{ static_cast<unsigned int>(PGUPV::to_underlying(Query::ClippingOutputPrimitivesExt)) + 1 };
		uint64_t values[NQueries];
		GLQuery queries[NQueries];
		bool collectExtendedStatsFlag;
		MicroSecStopWatch stopwatch;
		float elapsedMs;
	};
};

