#include "InspectorParticleSystems.h"

#include "imgui.h"

void UIEmmiterWriteNode(Emmiter* emmiter)
{
	ImGui::InputInt("maxParticles", &emmiter->maxParticles);
	if (ImGui::Button("Reset Pool")) {
		emmiter->RestartParticlePool();
	}
	ImGui::InputFloat("Duration", &emmiter->duration);
	ImGui::InputFloat("Delay", &emmiter->delay);
	ImGui::Checkbox("Loop", &emmiter->isLooping);

	ImGui::SeparatorText("Spawn Module");
	
	if (emmiter->spawnModule) {
		switch (emmiter->spawnModule->type) {
		case SpawnEmmiterModule::CONSTANT:
			UIInspectorEmmiterSpawnModule((ConstantSpawnRate*)emmiter->spawnModule.get());
			break;

		case SpawnEmmiterModule::SINGLE_BURST:
			UIInspectorEmmiterSpawnModule((SingleBurstSpawn*)emmiter->spawnModule.get());
			break;

		case SpawnEmmiterModule::CONSTANT_BURST:
			UIInspectorEmmiterSpawnModule((ConstantBurstSpawn*)emmiter->spawnModule.get());
			break;

		default:
			break;
		}
		ImGui::Separator();
	}
	
	ImGui::SeparatorText("Initialize Modules");

	for (auto m = emmiter->initializeModules.begin(); m != emmiter->initializeModules.end(); ++m) {
		switch ((*m)->type) {
		case InitializeEmmiterModule::SET_SPEED:
			UIInspectorEmmiterInitializeModule((SetSpeed*)(*m).get());
			break;

		case InitializeEmmiterModule::SET_COLOR:
			UIInspectorEmmiterInitializeModule((SetColor*)(*m).get());
			break;

		default:
			break;
		}
	}

	ImGui::Separator();

	ImGui::SeparatorText("Update Modules");

	for (auto m = emmiter->updateModules.begin(); m != emmiter->updateModules.end(); ++m) {
		switch ((*m)->type) {
		case UpdateEmmiterModule::CHANGE_COLOR:
			break;
		default:
			break;
		}
	}

	ImGui::Separator();


	ImGui::SeparatorText("Render Modules");

	if (emmiter->renderModule) {
		switch (emmiter->spawnModule->type) {
		case RenderEmmiterModule::BILLBOARD:
			break;
		default:
			break;
		}
	}

	ImGui::Separator();

}

// spawn modules ---------------------------------------------------------------------------------------------------------------
void UIInspectorEmmiterSpawnModule(ConstantSpawnRate* spawnModule) {
	ImGui::SeparatorText("Constant Spawn Rate");
	ImGui::InputFloat("Spawn Rate", &spawnModule->spawnRate);
	ImGui::InputFloat("Particle Duration", &spawnModule->duration);
}

void UIInspectorEmmiterSpawnModule(SingleBurstSpawn* spawnModule) {
	ImGui::SeparatorText("Single Burst");
	ImGui::InputFloat("Amount", &spawnModule->amount);
	ImGui::InputFloat("Particle Duration", &spawnModule->duration);
}

void UIInspectorEmmiterSpawnModule(ConstantBurstSpawn* spawnModule) {
	ImGui::SeparatorText("Constant Burst");
	ImGui::InputFloat("Amount", &spawnModule->amount);
	ImGui::InputFloat("Spawn Rate", &spawnModule->spawnRate);
	ImGui::InputFloat("Particle Duration", &spawnModule->duration);
}


// initialize modules ----------------------------------------------------------------------------------------------------------
void UIInspectorEmmiterInitializeModule(SetSpeed* initModule)
{
	ImGui::SeparatorText("Set Initial Speed: ");

	ImGui::Checkbox("Single Value", &initModule->speed.usingSingleValue);

	if (initModule->speed.usingSingleValue) {
		ImGui::PushID("set_speed_single_PS");
		ImGui::InputDouble("X", &initModule->speed.singleValue.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->speed.singleValue.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->speed.singleValue.z, 0, 0, "%.2f");
		ImGui::PopID();
	}
	else {
		ImGui::PushID("set_speed_min_PS");
		ImGui::InputDouble("X", &initModule->speed.rangeValue.lowerLimit.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->speed.rangeValue.lowerLimit.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->speed.rangeValue.lowerLimit.z, 0, 0, "%.2f");
		ImGui::PopID();

		ImGui::PushID("set_speed_max_PS");
		ImGui::InputDouble("X", &initModule->speed.rangeValue.upperLimit.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->speed.rangeValue.upperLimit.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->speed.rangeValue.upperLimit.z, 0, 0, "%.2f");
		ImGui::PopID();
	}

}

void UIInspectorEmmiterInitializeModule(SetColor* initModule)
{
	ImGui::SeparatorText("Set Initial Color: ");

	ImGui::Checkbox("Single Value", &initModule->color.usingSingleValue);

	if (initModule->color.usingSingleValue) {
		ImGui::PushID("set_color_single_PS");
		ImGui::InputDouble("R", &initModule->color.singleValue.r, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("G", &initModule->color.singleValue.g, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("B", &initModule->color.singleValue.b, 0, 0, "%.2f");
		ImGui::PopID();
	}
	else {
		ImGui::PushID("set_color_min_PS");
		ImGui::InputDouble("R", &initModule->color.rangeValue.lowerLimit.r, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("G", &initModule->color.rangeValue.lowerLimit.g, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("B", &initModule->color.rangeValue.lowerLimit.b, 0, 0, "%.2f");
		ImGui::PopID();

		ImGui::PushID("set_color_max_PS");
		ImGui::InputDouble("R", &initModule->color.rangeValue.upperLimit.r, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("G", &initModule->color.rangeValue.upperLimit.g, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("B", &initModule->color.rangeValue.upperLimit.b, 0, 0, "%.2f");
		ImGui::PopID();
	}

}


// update modules --------------------------------------------------------------------------------------------------------------
// void UIInspectorEmmiterUpdateModule(SetColor* updateModule) {}





// render modules --------------------------------------------------------------------------------------------------------------
void UIInspectorEmmiterRenderModule(BillboardRender* renderModule)
{
}
