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
    const float* viewMatrix = (float*)glm::value_ptr(camera->viewMatrix);

    glPushMatrix();
    //glLoadIdentity();
    glLoadMatrixf(viewMatrix);

    vec3 cameraPosition;
    cameraPosition = camera->GetContainerGO()->GetComponent<Transform>()->GetPosition();

    vec3 particlePosition = particle->position + owner->owner->GetTransform()->GetPosition();

    Billboard::BeginSphericalBillboard(particlePosition, cameraPosition);

    // render
    glBegin(GL_TRIANGLES);
    glColor3ub(particle->color.r, particle->color.g, particle->color.b);

    glVertex3d(particlePosition.x - (1 * particle->scale.x), particlePosition.y + (1 * particle->scale.y), particlePosition.z);
    glVertex3d(particlePosition.x - (1 * particle->scale.x), particlePosition.y - (1 * particle->scale.y), particlePosition.z);
    glVertex3d(particlePosition.x + (1 * particle->scale.x), particlePosition.y + (1 * particle->scale.y), particlePosition.z);

    glVertex3d(particlePosition.x - (1 * particle->scale.x), particlePosition.y - (1 * particle->scale.y), particlePosition.z);
    glVertex3d(particlePosition.x + (1 * particle->scale.x), particlePosition.y - (1 * particle->scale.y), particlePosition.z);
    glVertex3d(particlePosition.x + (1 * particle->scale.x), particlePosition.y + (1 * particle->scale.y), particlePosition.z);

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
