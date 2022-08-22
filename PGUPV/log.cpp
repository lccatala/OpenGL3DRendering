#include <sstream>
#ifdef _DEBUG
#include <iostream>
#endif

#ifdef _WIN32
#include <Windows.h>
#else
#include <stdlib.h>
#endif

#include "utils.h"
#include "log.h"
#include "app.h"
#include "lifetimeManager.h"

using PGUPV::Log;
using PGUPV::App;

using std::string;

#define MAX_LINES_IN_ERROR_DIALOG 35

Log *Log::theInstance = nullptr;

Log &Log::getInstance() {
	if (theInstance == nullptr) { // not thread safe!!!
		theInstance = new Log();
		SetLongevity(theInstance, 100, Private::Deleter<Log>::Delete);
	}
	return *theInstance;
}

Log::Log() : logFullFilepath(false) {
#ifdef _DEBUG
	notification_level = INFO_LEVEL;
#else
	notification_level = WARNING_LEVEL;
#endif
	if (!openErrorLog()) {
		throw std::runtime_error("No se puede abrir el fichero de log " LOG_FILE_NAME);
	}
}

Log::~Log() {
	INFO("Closing Log");
	errLog.close();
	theInstance = nullptr;
}

void Log::writeErrorLog(const std::string &str, const char *file, int line,
	int level) {
	if (level > notification_level)
		return;

	std::ostringstream msg;
	msg << "[";
	if (logFullFilepath)
		msg << file;
	else {
		std::string tmp = file;
		auto i = tmp.find_last_of("\\/");
		if (i == std::string::npos)
			msg << file;
		else
			msg << (file + i + 1);
	}
	msg << " (" << line << ")] ";
	msg << str;
	writeErrorLog(msg.str(), level);

	if (level == ERROR_LEVEL) {
		/*
		Puedes parar la ejecución de tu programa aquí, cada vez que se genere un
		error con una llamada a ERR() o ERRT(). Si tienes parada la ejecución con un
		depurador, puedes ver en la pila de llamadas (panel Pila de llamadas) las
		funciones que se estaban ejecutando en el momento de detectarse el error.
		Ten en cuenta que la fuente del error puede no encontrarse en la función que
		ha lanzado la excepción, sino en algún punto previo de la ejecución.

		INSERTA EN LA LINEA SIGUIENTE EL PUNTO DE RUPTURA ->
		*/
		errLog.flush();
	}
}


void Log::writeErrorLogAndMessageBox(const std::string &str, const char *file, int line,
	int level) {
	writeErrorLog(str, file, line, level);
	// Clamp the number of lines in the dialog to MAX_LINES_IN_ERROR_DIALOG
	std::stringstream ss(str);
	std::string to, out;

	if (!str.empty())
	{
		int i = 0;
		while (i < MAX_LINES_IN_ERROR_DIALOG && std::getline(ss, to, '\n')) {
			out += to + "\n";
			i++;
		}
	}

	int button = App::getInstance().showMessageDialog(PGUPV::DialogType::DLG_ERROR, "Error fatal", out);
	if (button == 100)
		showLogFileInEditor();
	if (button == 100 || button == 101) {
		exit(-1);
	}
}

#ifdef _WIN32
std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}
#endif

void Log::writeErrorLog(const std::string &str, int level) {

	if (level > notification_level)
		return;
	std::ostringstream msg;
	if (level == ERROR_LEVEL)
		msg << "ERR: ";
	else if (level == WARNING_LEVEL)
		msg << "WAR: ";
	else if (level == INFO_LEVEL)
		msg << "INF: ";
	else if (level == LIBINFO_LEVEL)
		msg << "LIB: ";
	else
		msg << "FRM: ";
	msg << str;

	msg << std::endl;

#ifdef _DEBUG
#ifdef _WIN32
	OutputDebugString(s2ws("   " + msg.str()).c_str());
#else
	// Si estamos compilando en debug, mostrar por la consola los mensajes
	std::cerr << msg.str();
#endif
#endif
	errLog << "[" << PGUPV::getCurrentDateTimeString() << "] " << msg.str();

	notify(msg.str());
}

bool Log::openErrorLog() {
	errLog.open(LOG_FILE_NAME, std::ios_base::app);
	if (errLog.is_open()) {
		errLog << std::endl;
	}
	logFileFullPathName = getCurrentWorkingDir() + LOG_FILE_NAME;
	return errLog.is_open();
}

void Log::setNotificationLevel(PGUPV::NOTIFICATION_LEVEL level) {
	notification_level = level;
}

PGUPV::NOTIFICATION_LEVEL Log::getNotificationLevel() {
	return notification_level;
}

void Log::showLogFileInEditor() {
#ifdef _WIN32
	std::wstring stemp = std::wstring(logFileFullPathName.begin(), logFileFullPathName.end());
	ShellExecute(0, 0, stemp.c_str(), 0, 0, SW_SHOW);
#elif __APPLE__
	std::string cmd = "open -t " + logFileFullPathName;
	if (system(cmd.c_str()) < 0) {
		writeErrorLog("Error al ejecutar " + cmd, ERROR_LEVEL);
	}
#else
	std::string cmd = "xdg-open " + logFileFullPathName;
	if (system(cmd.c_str()) < 0) {
		writeErrorLog("Error al ejecutar " + cmd, ERROR_LEVEL);
	}
#endif
}
