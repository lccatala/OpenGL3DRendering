#include <sstream>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <iostream>

#include <algorithm>

#include "commandLineProcessor.h"
#include "log.h"
#include "utils.h"
#include "fileStats.h"
#include "app.h"
#include "eventProcessor.h"
#include "eventSource.h"
#include "videoDevice.h"

using std::string;
using PGUPV::CommandLineProcessor;
using PGUPV::Log;
using PGUPV::FileStats;
using PGUPV::App;
using PGUPV::SaveToFileAndDispatchEventProcessor;
using PGUPV::LoadFileEventSource;

static void processSize(std::list<std::string> &args, PGUPV::App &instance) {
	uint w, h;
	if (args.size() < 3)
		ERRT("Faltan argumentos para la opción -size");
	args.pop_front();
	w = stoi(args.front());
	args.pop_front();
	h = stoi(args.front());
	args.pop_front();

	std::ostringstream msg;
	msg << "Tamaño inicial de ventana: " << w << ", " << h;
	INFO(msg.str());
	if (w == 0 || w > 1920 * 2)
		ERRT("Ancho de la ventana no válido");
	if (h == 0 || h > 1024 * 2)
		ERRT("Alto de la ventana no válido");

	instance.setInitWindowSize(w, h);
}

static void processPos(std::list<std::string> &args, PGUPV::App &instance) {
	uint x, y;
	if (args.size() < 3)
		ERRT("Faltan argumentos para la opción -pos");
	args.pop_front();
	x = stoi(args.front());
	args.pop_front();
	y = stoi(args.front());
	args.pop_front();
	if (x > 1920)
		ERRT("Posición horizontal de la ventana no válida");
	if (y > 1200)
		ERRT("Posición vertical de la ventana no válida");
	instance.setInitWindowPos(x, y);
}

static void processFramesToLive(std::list<std::string> &args, PGUPV::App &instance) {
	if (args.size() < 2)
		ERRT("Faltan argumentos para la opción -ftl");
	args.pop_front();
	long ftl = stol(args.front());
	args.pop_front();
	if (ftl < 0) {
		ERRT("El número de fotogramas a dibujar (-ftl <n>) tiene que ser mayor o "
			"igual a cero");
	}
	instance.setFramesToLive(ftl);
}

static void processSetSeedPRNG(std::list<std::string> &args, PGUPV::App &/*instance*/) {
	if (args.size() < 2)
		ERRT("Faltan argumentos para la opción -srand");
	args.pop_front();
	int seed = stoi(args.front());
	args.pop_front();
	std::srand(static_cast<unsigned int>(seed));
}


static void processSnapShots(std::list<std::string> &args, PGUPV::App &instance) {
	// merge all the arguments in one string
	std::ostringstream str;

	do {
		str << args.front();
		args.pop_front();
	} while (!args.empty() && args.front().at(0) != '-');

	// check that there is both a { and a }, and that the former is before the
	// latter

	string line = str.str();
	string::size_type open = line.find_first_of('{');
	string::size_type close = line.find_last_of('}');

	if (open == string::npos || close == string::npos) {
		ERRT("La lista de intervalos se debe definir entre llaves: " + line);
	}

	string clean = line.substr(open + 1, close);

	PGUPV::Intervals ints = PGUPV::readIntervalsFromString(clean);
	instance.captureSnapshots(ints);
}

static void processLoggingLevel(std::list<std::string> &args, PGUPV::App &/*instance*/) {
	if (args.size() < 2)
		ERRT("Faltan argumentos para la opción -loglevel");

	args.pop_front();

	string level = PGUPV::to_lower(args.front());
	args.pop_front();
	if (level == "frame")
		Log::getInstance().setNotificationLevel(PGUPV::FRAME_LEVEL);
	else if (level == "libinfo")
		Log::getInstance().setNotificationLevel(PGUPV::LIBINFO_LEVEL);
	else if (level == "info")
		Log::getInstance().setNotificationLevel(PGUPV::INFO_LEVEL);
	else if (level == "warning")
		Log::getInstance().setNotificationLevel(PGUPV::WARNING_LEVEL);
	else if (level == "error")
		Log::getInstance().setNotificationLevel(PGUPV::ERROR_LEVEL);
	else {
		ERRT("Debes especificar el nivel de logging entre 'frame', 'libinfo', 'info', 'warning' y 'error");
	}
}

static void processPauseApplication(std::list<std::string> &args,
	PGUPV::App &instance) {
	args.pop_front();
	instance.pause();
}

static void processChangeWorkingDirectory(std::list<std::string> &args,
	const PGUPV::App &/*instance*/) {
	if (args.size() < 2)
		ERRT("Falta el directorio para la opción -cwd");

	args.pop_front();
	string path = args.front();
	args.pop_front();
	PGUPV::changeCurrentDir(path);

	INFO("Directorio actual cambiado a " + path);
}

static void processSaveStats(std::list<std::string> &args,
	const PGUPV::App &instance) {
	if (args.size() < 2)
		ERRT("Falta el nombre del fichero para la opción -stats");

	args.pop_front();
	string path = args.front();
	args.pop_front();

	instance.setStatsObj(std::make_shared<FileStats>(path));
	INFO("Almacenando las estadísticas de ejecución en " + path);
}

static void processSaveEvents(std::list<std::string> &args,
	PGUPV::App &instance) {
	if (args.size() < 2)
		ERRT("Falta el nombre del fichero para la opción -saveevents");

	args.pop_front();
	string path = args.front();
	args.pop_front();

	auto proc = std::unique_ptr<SaveToFileAndDispatchEventProcessor>(
		new SaveToFileAndDispatchEventProcessor(instance, path));
	instance.setEventProcessor(std::move(proc));
	INFO("Almacenando los eventos en " + path);
}

static void processReplayEvents(std::list<std::string> &args,
	PGUPV::App &instance) {
	if (args.size() < 2)
		ERRT("Falta el nombre del fichero para la opción -replay");

	args.pop_front();
	string path = args.front();
	args.pop_front();

	auto proc = std::unique_ptr<LoadFileEventSource>(
		new LoadFileEventSource(path));
	instance.setEventSource(std::move(proc));
	INFO("Reproduciendo los eventos de " + path);
}

static void processUserParams(std::list<std::string> &args) {
	if (args.size() < 2)
		ERRT("Falta el parámetro de usuario en la opción -o");

	args.pop_front(); // -o
	// Saltar todas las cadenas que no empiecen por -
	INFO("Opción de usuario: " + args.front());

	while (!args.empty() && !PGUPV::starts_with(args.front(), "-"))
		args.pop_front();
}

static void processListCameras() {
	const auto cameras = media::VideoDevice::getAvailableCameras();
	int i = 0;
	for (auto c : cameras) {
		std::cout << "[" << i++ << "] " << c << std::endl;
	}
	exit(0);
}

static void processIgnoreCameras(std::list<std::string> &args, App &instance) {

	args.pop_front(); // -nocameras
	INFO("Ignorando las cámaras conectadas al sistema");

	instance.setIgnoreCameras(true);
}

static void processNoGuiState(std::list<std::string> &args, App &instance) {

  args.pop_front(); // -noguistate
  INFO("El estado del GUI no se cargará ni guardará");

  instance.setForgetGUIState(true);
}


static void processListCameraOpts(std::list<std::string> &args) {
	if (args.size() < 2)
		ERRT("Falta el identificador de la cámara");

	args.pop_front();

	unsigned int idx = std::stoi(args.front());
	
	if (idx >= media::VideoDevice::getAvailableCameras().size()) {
		ERRT("Identificador de cámara no válido");
	}

	auto opts = media::VideoDevice::listOptions(idx);
	int i = 0;
	for (auto o : opts) {
		std::cout << "[" << i++ << "] " << o.first << " " << o.second.size
			<< " " << o.second.minfps << "-" << o.second.maxfps << std::endl;
	}
	exit(0);
}

std::string CommandLineProcessor::getHelp() {
	std::stringstream o;

	o << "Opciones disponibles:\n";
	o << "  -help muestra esta ayuda\n";
	o << "  -size <ancho> <alto> establece el tamaño inicial de ventana\n";
	o << "  -pos <x> <y> establece la posición inicial de la ventana\n";
	o << "  -ftl <n>   dibuja los primeros <n> frames, y termina la "
		"aplicación normalmente\n";
	o << "  -snap {10,11,13-15} hace una captura de los frames 10, 11, "
		"13, 14 y 15 y la guarda en ficheros (las llaves son "
		"obligatorias)\n";
	o << "  -loglevel {FRAME, LIBINFO, INFO, WARNING, ERROR} cuánta "
		"información se almacena en el fichero de log (más a menos)\n";
	o << "  -cwd path cambia el directorio actual al indicado antes de "
		"llamar a setup\n";
	o << "  -pause   arranca la aplicación en modo pausa\n";
	o << "  -srand <s>   establece la semilla del PRNG a <s>\n";
	o << "  -stats <filename> almacena en el fichero las estadísticas de ejecución\n";
	o << "  -saveevents <filename> almacena en el fichero los eventos de la ejecución\n";
	o << "  -replay <filename> reproduce los eventos almacenados en el fichero\n";
	o << "  -o <useropt> establece opciones de usuario\n";
	o << "  -listCameras muestra las cámaras conectadas y termina\n";
	o << "  -listOpts <cam> muestra las opciones de la cámara indicada y termina\n";
  o << "  -nocameras actúa como si no hubiera cámaras conectadas en el sistema\n";
  o << "  -noguistate no guarda ni carga el estado anterior del GUI\n";
  o << "  -libs muestra las versiones de las librerías utilizadas y termina\n";

	return o.str();
}

void processHelp(std::list<std::string> &args, PGUPV::App &/*instance*/) {
	args.pop_front();
	std::cout << CommandLineProcessor::getHelp();
	exit(0);
}


void processShowLibs(std::list<std::string> &args, PGUPV::App &instance) {
  args.pop_front();
  std::cout << instance.collectLibVersions() << std::endl;
  exit(0);
}


CommandLineProcessor::CommandLineProcessor(int argc, char *argv[]) {
	if (argc == 0 || argv == nullptr) return;
	executableName = argv[0];
	for (int i = 1; i < argc; i++) {
		args.push_back(argv[i]);
	}
}



bool CommandLineProcessor::process(App & instance) {
	auto targs = args;

	while (!targs.empty()) {
		auto arg = targs.front();
		if (arg[0] != '-') {
			std::string s = "Error en la opción de la línea de comandos. Comando no "
				"reconocido: " + arg + ". Usa la opción -help";
			ERRT(s);
		}

    if (arg == "-size") // Parámetro -size <ancho> <alto>
      processSize(targs, instance);
    else if (arg == "-pos") // Parámetro -pos <x> <y>
      processPos(targs, instance);
    else if (arg == "-ftl") // Parámetro -ftl <n>: el programa acabará
      // normalmente después de dibujar n frames
      // (se puede usar para tests)
      processFramesToLive(targs, instance);
    else if (arg == "-snap") // Parámetro -snap {<i>, <j>-<k>}:
      // capturar el frame i, los frames entre j
      // y k
      processSnapShots(targs, instance);
    else if (arg == "-help" || arg == "-h") // Parámetro -help
      processHelp(targs, instance);
    else if (arg ==
      "-loglevel") // Parámetro -loglevel {INFO|WARNING|ERROR}
      processLoggingLevel(targs, instance);
    else if (arg == "-cwd") // Parámetro -cwd path
      processChangeWorkingDirectory(targs, instance);
    else if (arg == "-pause") // Parámetro -pause
      processPauseApplication(targs, instance);
    else if (arg == "-srand") // Parámetro -srand <s>
      processSetSeedPRNG(targs, instance);
    else if (arg == "-stats") // Parámetro -stats <filename>
      processSaveStats(targs, instance);
    else if (arg == "-o") // Parámetros del usuario (ignorar)
      processUserParams(targs);
    else if (arg == "-saveevents") // Volcar los eventos a fichero
      processSaveEvents(targs, instance);
    else if (arg == "-replay") // Reproducir los eventos del fichero
      processReplayEvents(targs, instance);
    else if (arg == "-listCameras") // Mostrar las cámaras conectadas al sistema
      processListCameras();
    else if (arg == "-listOpts") // Mostrar las opciones de la cámara indicada
      processListCameraOpts(targs);
    else if (arg == "-nocameras") // Ignorar las cámaras del sistema
      processIgnoreCameras(targs, instance);
    else if (arg == "-noguistate") // Ignorar el estado almacenado del GUI
      processNoGuiState(targs, instance);
    else if (arg == "-libs") // Mostrar versiones librerías
      processShowLibs(targs, instance);
    else if (arg == "-ignore") // Ignorar las opciones de aquí en adelante
      break;
		else
			ERRT(std::string("Opción no reconocida ") + arg);
	}
	return true;
}

bool CommandLineProcessor::process(int argc, char *argv[],
	PGUPV::App &instance) {
	CommandLineProcessor cmd(argc, argv);
	return cmd.process(instance);
}


bool CommandLineProcessor::hasUserOption(const std::string &opt) const {
	auto it = args.cbegin();
	while (it != args.cend()) {
		it = std::find(it, args.cend(), "-o");
		if (it == args.cend()) return false;
		++it;
		if (it == args.cend()) return false;
		if (opt == *it) return true;
		++it;
	}
	return false;
}

std::string CommandLineProcessor::getUserOptionParam(const std::string &/*opt*/, unsigned int /*pos*/) const {
	ERRT("Funcionalidad no implementada. Habla con Paco si la necesitas");
}
