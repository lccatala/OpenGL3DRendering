#ifndef _LOG_H
#define _LOG_H 2014

#include <fstream>
#include <stdexcept>
#include <observable.h>
#include <string>

namespace PGUPV {

#define LOG_FILE_NAME "log.txt"

  enum NOTIFICATION_LEVEL {
    NO_LOG_MSGS = 0,
    ERROR_LEVEL,
    WARNING_LEVEL,
    INFO_LEVEL,
    LIBINFO_LEVEL,
    FRAME_LEVEL
  };

#define FRAME(s) do PGUPV::Log::getInstance().writeErrorLog( (s), __FILE__, __LINE__, PGUPV::FRAME_LEVEL); while(0)
#define LIBINFO(s) do PGUPV::Log::getInstance().writeErrorLog( (s), __FILE__, __LINE__, PGUPV::LIBINFO_LEVEL); while(0)
#define INFO(s) do PGUPV::Log::getInstance().writeErrorLog( (s), __FILE__, __LINE__, PGUPV::INFO_LEVEL); while(0)
#define WARN(s) do PGUPV::Log::getInstance().writeErrorLog( (s), __FILE__, __LINE__, PGUPV::WARNING_LEVEL); while(0)
#define ERR(s) do  PGUPV::Log::getInstance().writeErrorLog( (s), __FILE__, __LINE__, PGUPV::ERROR_LEVEL); while(0)
#define ERRT(s) do { PGUPV::Log::getInstance().writeErrorLogAndMessageBox( (s), __FILE__, __LINE__, PGUPV::ERROR_LEVEL);  throw std::runtime_error(s); } while(0)

  class Log : public Observable<std::string> {
  public:
    static Log &getInstance();
    void writeErrorLog(const std::string &str, const char *file, int line, int level);
    void writeErrorLogAndMessageBox(const std::string &str, const char *file, int line, int level);
    void writeErrorLog(const std::string &str, int level);
    void setNotificationLevel(NOTIFICATION_LEVEL level);
    NOTIFICATION_LEVEL getNotificationLevel();
    /**
    Por defecto sólo se guarda el nombre del fichero donde se produjo el mensaje. Si quieres guardar toda la
    ruta, llama a esta función
    \param logFullFilepath si true, guarda en el log la ruta completa de los ficheros
    */
    void setLogFilepath(bool logFullPath) { logFullFilepath = logFullPath; };
	~Log();
  private:
    Log();

    bool openErrorLog();
    void showLogFileInEditor();
    //Log de la aplicación
    std::ofstream errLog;
    // Nivel de notificación (desde NO_LOG_MSGS: no msgs hasta FRAME_LEVEL: máximo detalle)
    NOTIFICATION_LEVEL notification_level;
    bool logFullFilepath;
    std::string logFileFullPathName;
	static Log *theInstance;
  };


};
#endif


