#include "RenderEmmiterModule.h"
#include "Emmiter.h"
#include "ParticleSystem.h"
#include "Transform.h"
#include "Billboard.h"
#include "GL/glew.h"

BillboardRender::BillboardRender(Emmiter* owner)
{
    this->owner = owner;
    type = BILLBOARD;
}

void BillboardRender::Update(Particle* particle, Camera* camera)
{
    const float* projectionMatrix = (float*)glm::value_ptr(camera->projectionMatrix);

    glPushMatrix();
    glLoadMatrixf(projectionMatrix);

    vec3 cameraPosition;
    cameraPosition.x = camera->projectionMatrix[3].x;
    cameraPosition.y = camera->projectionMatrix[3].y;
    cameraPosition.z = camera->projectionMatrix[3].z;

    vec3 particlePosition = particle->position + owner->owner->GetTransform()->GetPosition();

    Billboard::BeginSphericalBillboard(particlePosition, cameraPosition);

    // render
    glBegin(GL_TRIANGLES);
    glColor3ub(particle->color.r, particle->color.g, particle->color.b);

    glVertex3d(-(1 * particle->scale.x), +(1 * particle->scale.y), 0);
    glVertex3d(-(1 * particle->scale.x), -(1 * particle->scale.y), 0);
    glVertex3d(+(1 * particle->scale.x), +(1 * particle->scale.y), 0);

    glVertex3d(-(1 * particle->scale.x), -(1 * particle->scale.y), 0);
    glVertex3d(+(1 * particle->scale.x), -(1 * particle->scale.y), 0);
    glVertex3d(+(1 * particle->scale.x), +(1 * particle->scale.y), 0);

    glEnd();

    // particle->Render();

    Billboard::EndBillboard();

    //glPopMatrix();

}

json BillboardRender::SaveModule()
{
    json moduleJSON;

    moduleJSON["Type"] = type;

    moduleJSON["BillboardType"] = billboardType;

    return moduleJSON;
}

void BillboardRender::LoadModule(const json& moduleJSON)
{
    if (moduleJSON.contains("Type"))
    {
        type = moduleJSON["Type"];
    }

    if (moduleJSON.contains("BillboardType"))
    {
        billboardType = moduleJSON["BillboardType"];
    }
}
