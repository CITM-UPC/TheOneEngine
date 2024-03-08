#include "CollisionSolver.h"
#include "Defs.h"
#include "Collider2D.h"
#include "Transform.h"


CollisionSolver::CollisionSolver() {}

CollisionSolver::~CollisionSolver() {}

vec2 CollisionSolver::Clamp(vec2 origin, vec2 min, vec2 max)
{
    origin.x <= min.x ? origin.x = min.x : origin.x = origin.x;
    origin.y <= min.y ? origin.y = min.y : origin.y = origin.y;

    origin.x >= max.x ? origin.x = max.x : origin.x = origin.x;
    origin.y >= max.y ? origin.y = max.y : origin.y = origin.y;
    return origin;
}

double DistanceXZ(const vec3 posA, const vec3 posB)
{
    vec2 posA_XZ = vec2(posA.x, posA.z);
    vec2 posB_XZ = vec2(posB.x, posB.z);

    return glm::distance(posA_XZ, posB_XZ);
}


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
    else if (colliderA->colliderType == ColliderType::Rect && colliderB->colliderType == ColliderType::Rect)
    {
        // Check collision between two rectangles
        double xDist = abs(transformA->GetPosition().x - transformB->GetPosition().x);
        double yDist = abs(transformA->GetPosition().y - transformB->GetPosition().y);
        double xOverlap = (colliderA->w + colliderB->w) / 2 - xDist;
        double yOverlap = (colliderA->h + colliderB->h) / 2 - yDist;
        return (xOverlap > 0 && yOverlap > 0);
    }
    else
    {
        // Swap so that objA is the circle
        GameObject* circle;
        GameObject* rectangle;
        if (colliderA->colliderType == ColliderType::Circle) {
            circle = objA;
            rectangle = objB;
        }
        else {
            circle = objB;
            rectangle = objA;
        }

        const Transform* circleT = circle->GetComponent<Transform>();
        const Collider2D* circleC = circle->GetComponent<Collider2D>();

        const Transform* rectangleT = rectangle->GetComponent<Transform>();
        const Collider2D* rectangleC = rectangle->GetComponent<Collider2D>();

        // Calculate distance between circle center and rectangle center
        double circleDistanceX = std::abs(circleT->GetPosition().x - rectangleT->GetPosition().x);
        double circleDistanceY = std::abs(circleT->GetPosition().z - rectangleT->GetPosition().z);

        // Check if circle is too far away from rectangle to collide
        if (circleDistanceX > (rectangleC->w / 2.0 + circleC->radius)) { return false; }
        if (circleDistanceY > (rectangleC->h / 2.0 + circleC->radius)) { return false; }

        // Check if circle is close enough to collide with rectangle
        if (circleDistanceX <= (rectangleC->w / 2.0)) { return true; }
        if (circleDistanceY <= (rectangleC->h / 2.0)) { return true; }

        // Check if circle collides with corner of rectangle
        double cornerDistanceSq = pow(circleDistanceX - rectangleC->w / 2.0, 2) + pow(circleDistanceY - rectangleC->h / 2.0, 2);
        return (cornerDistanceSq <= pow(circleC->radius, 2));
    }
}

void CollisionSolver::RectRectCollision(GameObject* objA, GameObject* objB)
{
    // Currently not needed
}

void CollisionSolver::CirCirCollision(GameObject* objA, GameObject* objB)
{
    Transform* transformA = objA->GetComponent<Transform>();
    Collider2D* colliderA = objA->GetComponent<Collider2D>();

    Transform* transformB = objB->GetComponent<Transform>();
    Collider2D* colliderB = objB->GetComponent<Collider2D>();


    // Vector from player circle center to NPC circle center
    vec2 collisionVector = { transformA->GetPosition().x - transformB->GetPosition().x, transformA->GetPosition().z - transformB->GetPosition().z };

    // Distance between the centers of the two circles
    vec2 pos2dA = { transformA->GetPosition().x, transformA->GetPosition().z };
    vec2 pos2dB = { transformB->GetPosition().x, transformB->GetPosition().z };
    double distance = glm::distance(pos2dA, pos2dB);

    // Sum of the radii of the two circles
    double totalRadius = colliderA->radius + colliderB->radius;

    // Calculate mtv if the is overlap
    if (distance <= totalRadius)
    {
        vec2 mtv = glm::normalize(collisionVector);
        mtv.x *= totalRadius - distance;
        mtv.y *= totalRadius - distance;

        transformA->SetPosition({ mtv.x, transformA->GetPosition().y, mtv.y });
    }
}



void CollisionSolver::CirRectCollision(GameObject* objA, GameObject* objB)
{
    Transform* transformA = objA->GetComponent<Transform>();
    Collider2D* colliderA = objA->GetComponent<Collider2D>();

    Transform* transformB = objB->GetComponent<Transform>();
    Collider2D* colliderB = objB->GetComponent<Collider2D>();
    
    // Closest point on the rectangle to the circle center
    vec2 topLeft = { transformB->GetPosition().x - colliderB->w / 2, transformB->GetPosition().z - colliderB->h / 2 };
    vec2 botRight = { transformB->GetPosition().x + colliderB->w / 2, transformB->GetPosition().z + colliderB->h / 2 };
    vec2 temp = { transformA->GetPosition().x, transformA->GetPosition().z };
    vec2 closestPoint = Clamp(temp, topLeft, botRight);

    // Vector from the closest point on the rectangle to the circle center
    vec2 rectToCircle = temp - closestPoint;

    // Distance between the closest point and the circle center
    double distance = glm::distance(temp, closestPoint);

    // Overlap distance between the player and the obstacle
    double overlapDistance = colliderA->radius - distance;

    // Calculate mtv if there is overlap
    if (overlapDistance > 0.0)
    {
        vec2 mtv = glm::normalize(rectToCircle);
        mtv.x *= overlapDistance;
        mtv.y *= overlapDistance;

        transformA->SetPosition({ mtv.x, transformA->GetPosition().y, mtv.y });
    }
}

