#include "stopWatch.h"
#include "app.h"

using PGUPV::MicroSecStopWatch;
using PGUPV::App;

MicroSecStopWatch::MicroSecStopWatch() : begin(App::getCurrentMicroSecs())
{
}

int64_t MicroSecStopWatch::getElapsedAndRestart() {
	int64_t res = getElapsed();
	restart();
	return res;
}

int64_t MicroSecStopWatch::getElapsed() {
	return App::getCurrentMicroSecs() - begin;
}

void MicroSecStopWatch::restart() {
	begin = App::getCurrentMicroSecs();
}
