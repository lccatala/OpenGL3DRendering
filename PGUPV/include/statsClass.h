#pragma once

#include "stopWatch.h"

namespace PGUPV {
	/**
	 \class StatsClass

	 Esta clase permite almacenar las estadísticas de la ejecución de una aplicación.
	 Esta clase no hace nada, por lo que actúa como un sumidero nulo. Tendrás que usar
	 una de sus descendientes para almacenar los resultados, por ejemplo, en un fichero
	 (FileStats).

	 */


	class StatsClass {
	public:
		StatsClass() : separator(";") {};
		virtual ~StatsClass() = default;
		virtual StatsClass &pushValue(const std::string &)  { return *this; };
		virtual void endFrame() {};
        
		void setSeparator(const std::string &sep) { separator = sep; };
		std::string getSeparator() const { return separator; };
        
        /** 
         Devuelve un nuevo objeto StopWatch para usarlo para medir tiempos.
         En el caso de un objeto Stats nulo, devuelve un cronómetro que no hace nada.
         
         */
        virtual std::shared_ptr<StopWatch> makeStopWatch()  {
            return std::make_shared<NullStopWatch>();
        };
        
	protected:
		std::string separator;
	};
};

