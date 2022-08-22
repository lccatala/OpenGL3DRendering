#include <guipg.h>

#include "progressBar.h"


using PGUPV::ProgressBar;

void ProgressBar::setFraction(float f) { 
	fraction = f; 
}

void ProgressBar::renderWidget() {
	GUILib::ProgressBar(fraction);
}
