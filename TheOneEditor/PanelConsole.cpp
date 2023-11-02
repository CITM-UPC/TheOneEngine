#include "PanelConsole.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"

#include "log.h"

PanelConsole::PanelConsole(PanelType type) : Panel(type) {}

PanelConsole::~PanelConsole() {}

bool PanelConsole::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags |= ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin("Console", &enabled, settingsFlags))
	{
		ImGuiWindowFlags scrollFlags = 0;
		scrollFlags |= ImGuiWindowFlags_HorizontalScrollbar;
		scrollFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;

		if (ImGui::BeginChild("Scrollbar", ImVec2(0, 0), false, scrollFlags))
		{
			if (ImGui::SmallButton("Clear"))
				app->CleanLogs();

			ImGui::Separator();

			for (const auto& log : app->GetLogs())
			{
				if (log[1] == *LOG_ERROR)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 0, 0, 255));
				else if (log[1] == *LOG_WARNING)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 255, 0, 255));
				else
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 255, 255, 255));

				ImGui::TextUnformatted(log.data());

				ImGui::PopStyleColor();
			}

			if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);

			ImGui::EndChild();
		}

		ImGui::End();
	}

	return true;
}