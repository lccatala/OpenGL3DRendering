// dear imgui: standalone example application for SDL2 + OpenGL
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

#include "../../PGUPV/include/treeNodeFlags.h"

#include "guipg.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imGuIZMOquat.h"


#include "imgui_internal.h"
#include "IconsFontAwesome5.h"

#include "icons.cpp"

#include <SDL.h>
#include <glm/gtc/quaternion.hpp>
#include <gsl/gsl>
#include <sstream>

using PGUPV::GUILib;

bool GUILib::imgui_initialized = false;
bool GUILib::ignore_state = false;

std::string GUILib::getVersion() {
	return "ImGui: " IMGUI_VERSION;
}

bool GUILib::initGUI(SDL_Window* window, SDL_GLContext *gl_context, const std::string &glsl_version) {
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version.c_str());

	io.Fonts->AddFontDefault();

	ImFontConfig config;
	config.MergeMode = true;
	config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
	static const ImWchar icon_ranges[] = { 0xf000, 0xF300, 0 };
	/*ImFont* font = */io.Fonts->AddFontFromMemoryCompressedTTF(Icons_compressed_data, Icons_compressed_size, 13.0f, &config, icon_ranges);

	// Setup style
	ImGui::StyleColorsDark();
	imgui_initialized = true;
	if (ignore_state) {
		forgetState();
		ignore_state = false;
	}
	return true;
}

bool GUILib::processEvent(SDL_Event *event) {
	ImGui_ImplSDL2_ProcessEvent(event);

	auto io = ImGui::GetIO();
	if (io.WantCaptureKeyboard && (event->type >= SDL_KEYDOWN && event->type <= SDL_KEYMAPCHANGED))
		return true;
	if (io.WantCaptureMouse && (event->type >= SDL_MOUSEMOTION && event->type <= SDL_MOUSEWHEEL))
		return true;
	return false;
}

void GUILib::startFrame(SDL_Window *window) {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();
}

void GUILib::finishFrame() {
	// Rendering
	ImGui::Render();
}

void GUILib::renderFrame() {
	auto data = ImGui::GetDrawData();
	ImGui_ImplOpenGL3_RenderDrawData(data);
}

void GUILib::shutdown() {
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}


void GUILib::forgetState() {
	if (imgui_initialized) 
		ImGui::GetIO().IniFilename = nullptr;
	else {
		ignore_state = true;
	}
}

bool GUILib::Buttom(const std::string &label, const glm::vec2 &size) {
	return ImGui::Button(label.c_str(), ImVec2(size.x, size.y));
}

bool GUILib::Checkbox(const std::string &label, bool *status) {
	return ImGui::Checkbox(label.c_str(), status);
}

bool GUILib::ColorEdit(const std::string &label, glm::vec3 &color, bool hdr) {
	return ImGui::ColorEdit3(label.c_str(), (float*)&color.r, hdr ? ImGuiColorEditFlags_HDR : 0);
}

bool GUILib::ColorEdit(const std::string &label, glm::vec4 &color, bool hdr) {
	return ImGui::ColorEdit4(label.c_str(), (float*)&color.r, hdr ? ImGuiColorEditFlags_HDR : 0);
}


bool GUILib::DirectionGizmo(const std::string &label, glm::vec3 &direction) {
	return DirectionGizmo(label, direction, direction);
}

bool GUILib::DirectionGizmo(const std::string &label, glm::vec3 &direction, glm::vec3 &directionShown) {
	auto result = ImGui::gizmo3D(label.c_str(), direction);
	std::ostringstream os;
	os.precision(4);
	os << "("
		<< directionShown.x << ", "
		<< directionShown.y << ", "
		<< directionShown.z << ")";

	ImGui::LabelText("", "%s", os.str().c_str());
	return result;
}

bool GUILib::SliderFloat(const std::string &label, float &value, float min, float max, const std::string &display_format) {
	return ImGui::SliderFloat(label.c_str(), &value, min, max, display_format.c_str());
}

bool GUILib::SliderFloat(const std::string &label, glm::vec2 &value, float min, float max, const std::string &display_format)
{
	return ImGui::SliderFloat2(label.c_str(), &value.x, min, max, display_format.c_str());
}

void GUILib::ProgressBar(float fraction) {
	ImGui::ProgressBar(fraction);
}

bool GUILib::SliderFloat(const std::string &label, glm::vec3 &value, float min, float max, const std::string &display_format) {
	return ImGui::SliderFloat3(label.c_str(), &value.x, min, max, display_format.c_str());
}
bool GUILib::SliderFloat(const std::string &label, glm::vec4 &value, float min, float max, const std::string &display_format) {
	return ImGui::SliderFloat4(label.c_str(), &value.x, min, max, display_format.c_str());
}

bool GUILib::InputInt(const std::string &label, int &value) {
	return ImGui::InputInt(label.c_str(), &value);
}

bool GUILib::InputInt(const std::string &label, glm::ivec2 &value) {
	return ImGui::InputInt2(label.c_str(), &value.x);
}
bool GUILib::InputInt(const std::string &label, glm::ivec3 &value) {
	return ImGui::InputInt3(label.c_str(), &value.x);
}
bool GUILib::InputInt(const std::string &label, glm::ivec4 &value) {
	return ImGui::InputInt4(label.c_str(), &value.x);
}


bool GUILib::SliderInt(const std::string &label, int &value, int min, int max, const std::string &display_format) {
	return ImGui::SliderInt(label.c_str(), &value, min, max, display_format.c_str());
}

bool GUILib::SliderInt(const std::string &label, glm::ivec2 &value, int min, int max, const std::string &display_format) {
	return ImGui::SliderInt2(label.c_str(), &value.x, min, max, display_format.c_str());
}
bool GUILib::SliderInt(const std::string &label, glm::ivec3 &value, int min, int max, const std::string &display_format) {
	return ImGui::SliderInt3(label.c_str(), &value.x, min, max, display_format.c_str());
}
bool GUILib::SliderInt(const std::string &label, glm::ivec4 &value, int min, int max, const std::string &display_format){
	return ImGui::SliderInt4(label.c_str(), &value.x, min, max, display_format.c_str());
}


void GUILib::Text(const std::string &label) {
	ImGui::Text("%s", label.c_str());
}

void GUILib::PlotLines(const std::string &label, float *values, size_t count, size_t offset, const std::string &overlay_text, float scale_min, float scale_max, const glm::vec2 &size) {
	ImGui::PlotLines(label.c_str(), values, static_cast<int>(count), static_cast<int>(offset), overlay_text.c_str(), scale_min, scale_max, ImVec2(size.x, size.y));
}

static ImGuiCond toImGUI(GUILib::WindowPosSizeFlags flag) {
	switch (flag) {
	case GUILib::WindowPosSizeFlags::Always:
		return ImGuiCond_Always;
		case GUILib::WindowPosSizeFlags::Once:
			return ImGuiCond_Once;
		case GUILib::WindowPosSizeFlags::FirstUseEver:
			return ImGuiCond_FirstUseEver;
		case GUILib::WindowPosSizeFlags::Appearing:
			return ImGuiCond_Appearing;
		default:
			return ImGuiCond_Always;
	}
}

void GUILib::SetNextWindowPos(const glm::vec2 &pos, WindowPosSizeFlags flag, const glm::vec2 &pivot) {
	ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y), toImGUI(flag), ImVec2(pivot.x, pivot.y));
}

void GUILib::SetNextWindowSize(const glm::vec2 &size, WindowPosSizeFlags flag) {
	ImGui::SetNextWindowSize(ImVec2(size.x, size.y), toImGUI(flag));
}

void GUILib::Begin(const std::string &label) {
	ImGui::Begin(label.c_str());
}
void GUILib::End() {
	ImGui::End();
}

void GUILib::Separator() {
	ImGui::Separator();
}

bool GUILib::QuaternionGizmo(const std::string &label, glm::quat &q) {
	return ImGui::gizmo3D(label.c_str(), q, IMGUIZMO_DEF_SIZE, imguiGizmo::mode3Axes | imguiGizmo::sphereAtOrigin);
}

bool GUILib::ListBox(const std::string &label, int *current, const char * const items[], size_t count, int height_items) {
	bool res = ImGui::ListBox(label.c_str(), current, items, gsl::narrow<int>(count), height_items);
	return res;
}

bool GUILib::MultiListBoxBegin(const std::string & label)
{
	return ImGui::BeginListBox(label.c_str());
}

bool GUILib::MultiListBoxItem(const std::string & label, bool * selected)
{
	return ImGui::Selectable(label.c_str(), selected);
}

void GUILib::MultiListBoxEnd()
{
	ImGui::EndListBox();
}


bool GUILib::TreeNodeEx(const void *ptr_id, PGUPV::TreeNodeFlags flags, const std::string &label) {
	std::string tmp;

	if (flags & static_cast<unsigned int>(PGUPV::TreeNodeFlag::TreeNodeFlags_PGUPV_Visible)) {
		tmp = ICON_FA_EYE + std::string(" ");
		flags &= ~static_cast<unsigned int>(PGUPV::TreeNodeFlag::TreeNodeFlags_PGUPV_Visible);
	}
	else if (flags & static_cast<unsigned int>(PGUPV::TreeNodeFlag::TreeNodeFlags_PGUPV_Invisible)) {
		tmp = ICON_FA_EYE_SLASH + std::string(" ");
		flags &= ~static_cast<unsigned int>(PGUPV::TreeNodeFlag::TreeNodeFlags_PGUPV_Invisible);
	}
	if (flags & static_cast<unsigned int>(PGUPV::TreeNodeFlag::TreeNodeFlags_PGUPV_Movie)) {
		tmp += ICON_FA_FILM + std::string(" ");
		flags &= ~static_cast<unsigned int>(PGUPV::TreeNodeFlag::TreeNodeFlags_PGUPV_Movie);
	}

	tmp += label;
	return ImGui::TreeNodeEx(ptr_id, flags, "%s", tmp.c_str());
}

void GUILib::TreePop() {
	ImGui::TreePop();
}

bool GUILib::IsItemClicked() {
	return ImGui::IsItemClicked();
}

void GUILib::StartDisabledWidgets() {
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
}
void GUILib::StopDisabledWidgets() {
	ImGui::PopItemFlag();
	ImGui::PopStyleVar();
}

void GUILib::NextItemWidthPixels(float size)
{
	ImGui::SetNextItemWidth(size);
}

void GUILib::NextItemGrowWidth(float pct)
{
	if (pct < 0.0f) pct = 1.0f;
	else if (pct > 1.0f) pct = 1.f;

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * pct);
}

float PGUPV::GUILib::GetAvailableWidth()
{
	return ImGui::GetContentRegionAvail().x;
}



void GUILib::SameLine(float spaceInPixels) {
	ImGui::SameLine(spaceInPixels);
}

#include "nfd.h"

GUILib::DialogResult processResponseAndWriteResult(const nfdresult_t outCode, const nfdchar_t *selection, std::string &result) {
	if (outCode == NFD_OKAY) {
		result = selection;
		return GUILib::DialogResult::OK;
	}
	if (outCode == NFD_CANCEL) {
		return GUILib::DialogResult::CANCEL;
	}
	result = NFD_GetError();
	return GUILib::DialogResult::ERROR;
}


GUILib::DialogResult processResponseAndWriteMultipleResult(const nfdresult_t outCode, nfdpathset_t *selection, std::vector<std::string> &result) {
	if (outCode == NFD_OKAY) {
		for (size_t i = 0; i < NFD_PathSet_GetCount(selection); i++) {
			result.push_back(NFD_PathSet_GetPath(selection, i));
		}
		NFD_PathSet_Free(selection);
		return GUILib::DialogResult::OK;
	}
	if (outCode == NFD_CANCEL) {
		return GUILib::DialogResult::CANCEL;
	}
	result.push_back(NFD_GetError());
	return GUILib::DialogResult::ERROR;
}


GUILib::DialogResult GUILib::SelectFolder(const std::string & initialPath, std::string & result)
{
	nfdchar_t *outPath = nullptr;
	auto r = NFD_PickFolder(initialPath.empty() ? nullptr : initialPath.c_str(), &outPath);
	return processResponseAndWriteResult(r, outPath, result);
}

std::string buildFilterList(const std::vector<std::string> &filterList) {
	std::ostringstream os;
	bool first{ true };
	for (const auto &f : filterList) {
		if (first)
			first = false;
		else
			os << ";";
		os << f;
	}
	return os.str();
}

GUILib::DialogResult GUILib::OpenFile(const std::vector<std::string> &filterList, const std::string &initialPath, std::string &result)
{
	nfdchar_t *outPath = nullptr;
	auto fl = buildFilterList(filterList);
	auto r = NFD_OpenDialog(fl.empty() ? nullptr : fl.c_str() , initialPath.empty() ? nullptr : initialPath.c_str(), &outPath);
	return processResponseAndWriteResult(r, outPath, result);
}

GUILib::DialogResult GUILib::OpenFile(const std::vector<std::string> &filterList, const std::string &initialPath, std::vector<std::string> &result)
{
	nfdpathset_t filenames;
	auto fl = buildFilterList(filterList);
	auto r = NFD_OpenDialogMultiple(fl.empty() ? nullptr : fl.c_str(), initialPath.empty() ? nullptr : initialPath.c_str(), &filenames);
	return processResponseAndWriteMultipleResult(r, &filenames, result);
}

GUILib::DialogResult GUILib::SaveFile(const std::vector<std::string>& filterList, const std::string & initialPath, std::string & result)
{
	nfdchar_t *outPath = nullptr;
	auto fl = buildFilterList(filterList);
	auto r = NFD_SaveDialog(fl.empty() ? nullptr : fl.c_str(), initialPath.empty() ? nullptr : initialPath.c_str(), &outPath);
	return processResponseAndWriteResult(r, outPath, result);
}
