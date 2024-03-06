#include "CollisionSolver.h"
#include "Defs.h"
#include "Collider2D.h"
#include "Transform.h"


CollisionSolver::CollisionSolver() {}

CollisionSolver::~CollisionSolver() {}

bool CollisionSolver::CheckCollision(GameObject* objA, GameObject* objB)
{
    const Transform* transformA = objA->GetComponent<Transform>();
    const Collider2D* colliderA = objA->GetComponent<Collider2D>();

    const Transform* transformB = objB->GetComponent<Transform>();
    const Collider2D* colliderB = objB->GetComponent<Collider2D>();

    if (colliderA->colliderType == ColliderType::Circle && colliderB->colliderType == ColliderType::Circle)
    {
        // Check collision between two circles
        double distance = DistanceXZ(transformA->GetPosition(), transformB->GetPosition());
        double radiusSum = colliderA->radius + colliderB->radius;
        return (distance <= radiusSum);
    }
    //else if (colliderA->colliderType == ColliderType::Rect && colliderB->colliderType == ColliderType::Rect)
    //{
    //    // Check collision between two rectangles
    //    double xDist = abs(objA.pos.x - objB.pos.x);
    //    double yDist = abs(objA.pos.y - objB.pos.y);
    //    double xOverlap = (objA.w + objB.w) / 2 - xDist;
    //    double yOverlap = (objA.h + objB.h) / 2 - yDist;
    //    return (xOverlap > 0 && yOverlap > 0);
    //}
    //else
    //{
    //    // Swap so that objA is the circle
    //    Body circle, rectangle;
    //    if (objA.shape == ColliderType::Circle) {
    //        circle = objA;
    //        rectangle = objB;
    //    }
    //    else {
    //        circle = objB;
    //        rectangle = objA;
    //    }

    //    // Calculate distance between circle center and rectangle center
    //    double circleDistanceX = std::abs(circle.pos.x - rectangle.pos.x);
    //    double circleDistanceY = std::abs(circle.pos.y - rectangle.pos.y);

    //    // Check if circle is too far away from rectangle to collide
    //    if (circleDistanceX > (rectangle.w / 2.0 + circle.r)) { return false; }
    //    if (circleDistanceY > (rectangle.h / 2.0 + circle.r)) { return false; }

    //    // Check if circle is close enough to collide with rectangle
    //    if (circleDistanceX <= (rectangle.w / 2.0)) { return true; }
    //    if (circleDistanceY <= (rectangle.h / 2.0)) { return true; }

    //    // Check if circle collides with corner of rectangle
    //    double cornerDistanceSq = pow(circleDistanceX - rectangle.w / 2.0, 2) + pow(circleDistanceY - rectangle.h / 2.0, 2);
    //    return (cornerDistanceSq <= pow(circle.r, 2));
    //}
}

void CollisionSolver::RectRectCollision(GameObject* objA, GameObject* objB)
{
    // Currently not needed
}

void CollisionSolver::CirCirCollision(GameObject* objA, GameObject* objB)
{
    //const Transform* transformA = objA->GetComponent<Transform>();
    //const Collider2D* colliderA = objA->GetComponent<Collider2D>();

    //const Transform* transformB = objB->GetComponent<Transform>();
    //const Collider2D* colliderB = objB->GetComponent<Collider2D>();


    //// Vector from player circle center to NPC circle center
    //vec2 collisionVector = objA->pos - objB->pos;

    //// Distance between the centers of the two circles
    //double distance = objA->pos.DistanceTo(objB->pos);

    //// Sum of the radii of the two circles
    //double totalRadius = objA->r + objB->r;

    //// Calculate mtv if the is overlap
    //if (distance <= totalRadius)
    //{
    //    dPoint mtv = collisionVector.Normalize();
    //    mtv.x *= totalRadius - distance;
    //    mtv.y *= totalRadius - distance;
    //    objA->pos += mtv;
    //}
}

void CollisionSolver::CirRectCollision(GameObject* objA, GameObject* objB)
{
    Transform* transformA = objA->GetComponent<Transform>();
    Collider2D* colliderA = objA->GetComponent<Collider2D>();

    Transform* transformB = objB->GetComponent<Transform>();
    Collider2D* colliderB = objB->GetComponent<Collider2D>();

    // Closest point on the rectangle to the circle center
    dPoint topLeft = { objB->pos.x - objB->w / 2, objB->pos.y - objB->h / 2 };
    dPoint botRight = { objB->pos.x + objB->w / 2, objB->pos.y + objB->h / 2 };
    dPoint closestPoint = objA->pos.Clamp(topLeft, botRight);

    // Vector from the closest point on the rectangle to the circle center
    dPoint rectToCircle = objA->pos - closestPoint;

    // Distance between the closest point and the circle center
    double distance = objA->pos.DistanceTo(closestPoint);

    // Overlap distance between the player and the obstacle
    double overlapDistance = objA->r - distance;

    // Calculate mtv if there is overlap
    if (overlapDistance > 0.0)
    {
        dPoint mtv = rectToCircle.Normalize();
        mtv.x *= overlapDistance;
        mtv.y *= overlapDistance;

        objA->pos += mtv;
    }
}

double DistanceXZ(const vec3 posA, const vec3 posB)
{
    vec2 posA_XZ = vec2(posA.x, posA.z);
    vec2 posB_XZ = vec2(posB.x, posB.z);

    return glm::distance(posA_XZ, posB_XZ);
}