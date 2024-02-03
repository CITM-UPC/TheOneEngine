#ifndef __DEMOFUNCTIONS_H__
#define __DEMOFUNCTIONS_H__

#include <memory>
class GameObject;

namespace Demo
{

std::shared_ptr<GameObject> CreateTank();
std::shared_ptr<GameObject> CreateBullet();

}

#endif