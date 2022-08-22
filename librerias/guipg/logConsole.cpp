#include <imgui.h>
#include <cstdlib>
#include "logConsole.h"

struct PGUPV::LogConsole::LogImpl
{
	ImGuiTextBuffer     Buf;
	ImVector<int>       LineOffsets;        // Index to lines offset
	bool                ScrollToBottom;

	void    Clear() { Buf.clear(); LineOffsets.clear(); }

	void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size);
		ScrollToBottom = true;
	}

	void    Draw(const char* title, bool* p_open = NULL)
	{
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}
		if (ImGui::Button("Clear")) Clear();
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (copy) ImGui::LogToClipboard();

		const char* buf_begin = Buf.begin();
		const char* line = buf_begin;
		for (int line_no = 0; line != NULL; line_no++)
		{
			const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
			if (strncmp(line, "ERR", 3) == 0)
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%.*s", 
				(line_end == nullptr ? static_cast<int>(strlen(line)) : static_cast<int>(line_end - line)), line);
			else if (strncmp(line, "WAR", 3) == 0)
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.*s", 
				(line_end == nullptr ? static_cast<int>(strlen(line)) : static_cast<int>(line_end - line)), line);
			else
				ImGui::TextUnformatted(line, line_end);

			line = line_end && line_end[1] ? line_end + 1 : NULL;
		}

		if (ScrollToBottom)
			ImGui::SetScrollHereY(1.0f);
		ScrollToBottom = false;
		ImGui::EndChild();
		ImGui::End();
	}
};
//
//// Demonstrate creating a simple log window with basic filtering.
//void PGUPV::ShowExampleAppLog(bool* p_open)
//{
//	static ExampleAppLog log;
//
//	// Demo: add random items (unless Ctrl is held)
//	static double last_time = -1.0;
//	double time = ImGui::GetTime();
//	if (time - last_time >= 0.20f && !ImGui::GetIO().KeyCtrl)
//	{
//		const char* random_words[] = { "system", "info", "warning", "error", "fatal", "notice", "log" };
//		log.AddLog("[%s] Hello, time is %.1f, frame count is %d\n", random_words[rand() % IM_ARRAYSIZE(random_words)], time, ImGui::GetFrameCount());
//		last_time = time;
//	}
//
//	log.Draw("Example: Log", p_open);
//}

using PGUPV::LogConsole;

LogConsole::LogConsole() : impl(new PGUPV::LogConsole::LogImpl()) {
}

LogConsole::~LogConsole() {}

void LogConsole::clear() {
	impl->Clear();
}

void LogConsole::add(const std::string &msg) {
	impl->AddLog("%s", msg.c_str());
}

void LogConsole::render() {
	impl->Draw("Log");
}
