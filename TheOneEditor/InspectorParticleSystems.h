#pragma once

#include "..\TheOneEngine\ParticleSystem.h"

void UIEmmiterWriteNode(Emmiter* emmiter);

// spawn modules
void UIInspectorEmmiterSpawnModule(ConstantSpawnRate* spawnModule);

void UIInspectorEmmiterSpawnModule(SingleBurstSpawn* spawnModule);

void UIInspectorEmmiterSpawnModule(ConstantBurstSpawn* spawnModule);



// initialize modules
void UIInspectorEmmiterInitializeModule(SetSpeed* initModule);

void UIInspectorEmmiterInitializeModule(SetColor* initModule);


// update modules
// void UIInspectorEmmiterUpdateModule(SetColor* updateModule);



// render modules
void UIInspectorEmmiterRenderModule(BillboardRender* renderModule);


