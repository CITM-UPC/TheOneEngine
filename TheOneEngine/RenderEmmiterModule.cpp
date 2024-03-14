#include "RenderEmmiterModule.h"
#include "Emmiter.h"
#include "ParticleSystem.h"
#include "TransformComponent.h"
#include "Billboard.h"
#include "GL/glew.h"

BillboardRender::BillboardRender(Emmiter* owner)
{
    this->owner = owner;
    type = BILLBOARD;
}

void BillboardRender::Update(Particle* particle, vec3 cameraPosition)
{
    glPushMatrix();
    glTranslatef(particle->position.x, particle->position.y, particle->position.z);

    vec3 particlePosition = particle->position + owner->owner->GetTransform()->getPosition();

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
