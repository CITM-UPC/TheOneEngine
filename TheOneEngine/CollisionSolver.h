#ifndef __COLLISION_SOLVER_H__
#define __COLLISION_SOLVER_H__
#pragma once


class GameObject;
enum class ColliderType;

class CollisionSolver
{
public:
	CollisionSolver();
	~CollisionSolver();

	bool CheckCollision(GameObject* objA, GameObject* objB);

	void RectRectCollision(GameObject* objA, GameObject* objB);
	void CirCirCollision(GameObject* objA, GameObject* objB);
	void CirRectCollision(GameObject* objA, GameObject* objB);


private:

};

#endif // !__COLLISION_SOLVER_H__