#ifndef __PREFAB_H__
#define __PREFAB_H__
#pragma once

#include "..\TheOneEngine\GameObject.h"
#include <string>

class Prefab : public GameObject
{
	Prefab() {}
	Prefab(uint pId, std::string pName);
	~Prefab();

	static void CreatePrefab(GameObject& go);

private:
	uint id = 0;
	std::string name;
};


#endif // !__PREFAB_H__