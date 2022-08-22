#pragma once

#include <string>
#include <SDL_events.h>
#include <SDL_video.h>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

#include <vector>

namespace PGUPV {
	class GUILib {
	public:
		static std::string getVersion();
		static bool initGUI(SDL_Window* window, SDL_GLContext *gl_context, const std::string &glsl_version);
		static bool processEvent(SDL_Event *event);
		static void startFrame(SDL_Window *window);
		static void renderFrame();
		static void finishFrame();
		static void shutdown();
		static void forgetState();

		static bool Buttom(const std::string &label, const glm::vec2 &size);
		static bool Checkbox(const std::string &label, bool *status);

		static bool ColorEdit(const std::string &label, glm::vec3 &color, bool hdr = false);
		static bool ColorEdit(const std::string &label, glm::vec4 &color, bool hdr = false);

		static bool DirectionGizmo(const std::string &label, glm::vec3 &direction);
		// directionShown is used only in the label
		static bool DirectionGizmo(const std::string &label, glm::vec3 &direction, glm::vec3 &directionShown);

		static bool SliderFloat(const std::string &label, float &value, float min, float max, const std::string &display_format);
		static bool SliderFloat(const std::string &label, glm::vec2 &value, float min, float max, const std::string &display_format);
		static bool SliderFloat(const std::string &label, glm::vec3 &value, float min, float max, const std::string &display_format);
		static bool SliderFloat(const std::string &label, glm::vec4 &value, float min, float max, const std::string &display_format);

		static bool InputInt(const std::string &label, int &value);
		static bool InputInt(const std::string &label, glm::ivec2 &value);
		static bool InputInt(const std::string &label, glm::ivec3 &value);
		static bool InputInt(const std::string &label, glm::ivec4 &value);

		static bool SliderInt(const std::string &label, int &value, int min, int max, const std::string &display_format);
		static bool SliderInt(const std::string &label, glm::ivec2 &value, int min, int max, const std::string &display_format);
		static bool SliderInt(const std::string &label, glm::ivec3 &value, int min, int max, const std::string &display_format);
		static bool SliderInt(const std::string &label, glm::ivec4 &value, int min, int max, const std::string &display_format);

		static void Text(const std::string &label);

		static void PlotLines(const std::string &label, float *values, size_t count, size_t offset, const std::string &overlay_text, float scale_min, float scale_max, const glm::vec2 &size);

		// Panel
		enum class WindowPosSizeFlags {
			Always,   // Set the variable
			Once,   // Set the variable once per runtime session (only the first call with succeed)
			FirstUseEver,   // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
			Appearing    // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
		};

		static void SetNextWindowPos(const glm::vec2 &pos, WindowPosSizeFlags flag, const glm::vec2 &pivot = glm::vec2(0.f, 0.f));
		static void SetNextWindowSize(const glm::vec2 &size, WindowPosSizeFlags flag);
		static void Begin(const std::string &label);
		static void End();

		static void Separator();
		static void SameLine(float spaceInPixels = 0.0f);

		static bool QuaternionGizmo(const std::string &label, glm::quat &q);

		static bool ListBox(const std::string &label, int *current, const char * const items[], size_t count, int height_items);

		static bool MultiListBoxBegin(const std::string &label);
		static bool MultiListBoxItem(const std::string &label, bool *selected);
		static void MultiListBoxEnd();

		static void ProgressBar(float fraction);

		// Tree
		static void TreePop();
		static bool IsItemClicked();
		static bool TreeNodeEx(const void *ptr_id, unsigned int flags, const std::string &label);

		static void StartDisabledWidgets();
		static void StopDisabledWidgets();

		// If size > 0, it indicates size in pixels. If size < 0, indicates leave size pixels to the right
		static void NextItemWidthPixels(float size);
		// Use pct (between 0 and 1) of the available space
		static void NextItemGrowWidth(float pct);

		static float GetAvailableWidth();

		enum class DialogResult {
			OK, CANCEL, ERROR
		};

		/**
		Show a dialog for selecting a folder
		\param initialPath the initial folder shown in the dialog (watch out! in windows, do not use '/')
		\param result the folder path selected by the user, if the function returns OK, or an error message if return ERROR
		\return OK if the users click on Accept, CANCEL if she cancels or closes the dialog, or ERROR if there was some error
		*/
		static DialogResult SelectFolder(const std::string &initialPath, std::string &result);

		/**
		Show a dialog for selecting a file
		\param initialPath the initial folder shown in the dialog
		\param result the file path selected by the user, if the function returns OK, or an error message if return ERROR
		\return OK if the users click on Accept, CANCEL if she cancels or closes the dialog, or ERROR if there was some error
		*/
		static DialogResult OpenFile(const std::vector<std::string> &filterList, const std::string &initialPath, std::string &result);

		/**
		Show a dialog for selecting one or more files
		\param initialPath the initial folder shown in the dialog
		\param result the paths selected by the user, if the function returns OK, or an error message if return ERROR
		\return OK if the users click on Accept, CANCEL if she cancels or closes the dialog, or ERROR if there was some error
		*/
		static DialogResult OpenFile(const std::vector<std::string> &filterList, const std::string &initialPath, std::vector<std::string> &result);

		/**
		Show a dialog for selecting a file for saving
		\param initialPath the initial folder shown in the dialog
		\param result the file path selected by the user, if the function returns OK, or an error message if return ERROR
		\return OK if the users click on Accept, CANCEL if she cancels or closes the dialog, or ERROR if there was some error
		*/
		static DialogResult SaveFile(const std::vector<std::string> &filterList, const std::string &initialPath, std::string &result);
	private:
		static bool imgui_initialized, ignore_state;
	};

};
