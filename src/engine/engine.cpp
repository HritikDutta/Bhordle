#include "engine.h"

#include "core/application.h"
#include "platform/platform.h"
#include "transform.h"
#include "renderer2D.h"
#include "imgui.h"
#include "transform_data.h"

extern TransformList* transforms;

namespace Engine
{

void Init(const Application& app)
{
    R2D::Init();
    Imgui::Init(app);

    transforms = (TransformList*) PlatformAllocate(sizeof(TransformList));
    PlatformZeroMemory(transforms, sizeof(TransformList));
}

void Shutdown()
{
    PlatformFree(transforms);

    Imgui::Shutdown();
    R2D::Shutdown();
}

} // namespace Engine
