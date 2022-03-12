#include "animated_sprite.h"

#include "core/types.h"
#include "containers/darray.h"
#include "containers/string.h"
#include "containers/stringview.h"
#include "fileio/fileio.h"
#include "graphics/texture.h"
#include "math/math.h"
#include "serialization/json.h"

void AnimationGroup::Load(StringView filepath)
{
    json::Document document;

    {   // Parse the file
        String content;
        LoadFile(filepath, content);

        if (!json::ParseJsonString(content, document))
            return;
    }

    const json::Value& data = document.Start();

    {   // Load the texture atlas for the animation
        String atlaspath = data["directory"].string() + "\\" + data["file"].string();

        TextureSettings settings;
        settings.minFilter = settings.maxFilter = TextureSettings::Filter::NEAREST;
        atlas.Load(atlaspath, settings);
    }

    // Load animation data
    json::Array& animDatas = data["animations"].array();
    animations.Reserve(animDatas.size());
    for (const auto& animData : animDatas)
    {
        Animation animation;
        animation.group = this;

        animation.name = std::move(animData["name"].string());
        animation.frameRate = animData["frameRate"].float64();

        StringView loopType = animData["loopType"].string();
        if (loopType == "None")
            animation.loopType = Animation::LoopType::NONE;
        else if (loopType == "Cycle")
            animation.loopType = Animation::LoopType::CYCLE;
        else
            animation.loopType = Animation::LoopType::PING_PONG;

        // Load frames
        json::Array& frameDatas = animData["frames"].array();
        animation.frames.Reserve(frameDatas.size());
        for (const auto& frameData : frameDatas)
        {
            AnimationFrame frame;
            frame.atlas = atlas;

            frame.texCoords.x = frameData["left"].float64() / atlas.width();
            frame.texCoords.y = frameData["top"].float64() / atlas.height();
            frame.texCoords.z = frameData["right"].float64() / atlas.width();
            frame.texCoords.w = frameData["bottom"].float64() / atlas.height();
            
            frame.pivot.x = frameData["pivot_x"].float64();
            frame.pivot.y = frameData["pivot_y"].float64();

            animation.frames.EmplaceBack(frame);
        }

        animations.EmplaceBack(animation);
    }
}

// Keeping this explicit to allow multiple groups to share atlases
void AnimationGroup::Free()
{
    atlas.Free();
}

const AnimationFrame& Animation::GetCurrentFrame(f32 absoluteTime) const
{
    // TODO: It shouldn't consider absolute time. The timer should
    // be reset when it switches to this animation

    u32 index;

    switch (loopType)
    {
        case LoopType::NONE:
        {
            index = Min((u64) (absoluteTime * frameRate), frames.size() - 1);
        } break;

        case LoopType::CYCLE:
        {
            index = (u32) (absoluteTime * frameRate) % frames.size();
        } break;

        case LoopType::PING_PONG:
        {
            index = (u32) (absoluteTime * frameRate) % (2 * frames.size());
            if (index >= frames.size())
                index = 2 * frames.size() - index - 1;
        } break;
    }

    return frames[index];
}