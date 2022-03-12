#include "core/application.h"
#include "core/input.h"
#include "engine/imgui.h"
#include "engine/animated_sprite.h"
#include "engine/camera.h"
#include "engine/transform.h"
#include "engine/renderer2d.h"
#include "math/math.h"
#include "physics/rigidbody.h"
#include "physics/object.h"
#include "physics/trigger.h"
#include "fileio/fileio.h"
#include "serialization/json.h"
#include "physics/physics.h"

struct GameState
{
    Camera camera;
    const f32 zoomLevel = 100.0f;

    String playerName;

    // Rendering
    Imgui::Font font;

    AnimationGroup playerAnimGroup;
    AnimationGroup tileset;
    u32 animState = 0;

    Transform player;
    Transform obstacle;
    Transform movable;

    // Physics
    Physics::Object playerObject;
    Physics::Object obstacleObject;
    Physics::Object movableObject;

    Physics::Trigger waterTrigger;

    f32 force = 100.0f;

    // Debug Settings
    bool showColliders = false;

} gGameState;

static void OhNoWater(Physics::Object& object)
{
    object.rigidbody().Reset(Vector3(0, 0, object.transform().position().z));
}

void OnInit(Application& app)
{
    GameState& state = *(GameState*) app.data;

    f32 halfWidth = 0.5f * app.window.width / state.zoomLevel;
    f32 halfHeight = 0.5f * app.window.height / state.zoomLevel;
    state.camera = Camera::Orthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.1f, 100);
    state.camera.position = Vector3(0, 0, 1);

    state.font.Load("assets/fonts/bell.font.png", "assets/fonts/bell.font.json");

    state.playerAnimGroup.Load("assets/art/test/Player Spsheet.json");
    state.tileset.Load("assets/art/test/Overworld.json");

    {   // Setup Physics
        String contents;
        LoadFile("assets/data/settings.json", contents);

        json::Document document;
        json::ParseJsonString(contents, document);

        json::Object data = document.Start().object();
        
        state.playerName = data["name"].string();
        state.force = data["force"].float64();
        
        {
            state.player = Transform(Vector3(-1, 0, -0.1f));

            json::Object obj = data["player"].object();
            Physics::Object o(state.player, Physics::CollisionShape::AABB, Vector4(-0.25f, 1, 0.25f, 0));
            o.rigidbody().SetMass(obj["mass"].float64());
            o.rigidbody().SetDynamicFriction(obj["dynamicFriction"].float64());
            o.rigidbody().SetStaticFriction(obj["staticFriction"].float64());
            o.rigidbody().restitution() = obj["restitution"].float64();

            state.playerObject = o;
        }

        {
            state.obstacle = Transform(Vector3(2, 0.0f, 0.1f));

            json::Object obj = data["obstacle"].object();
            Physics::Object o(state.obstacle, Physics::CollisionShape::AABB, Vector4(-0.5f, 0.8f, 0.5f, 0.1f));

            state.obstacleObject = o;
        }

        {
            state.movable = Transform(Vector3());

            json::Object obj = data["movable"].object();
            Physics::Object o = Physics::Object(state.movable, Physics::CollisionShape::AABB, Vector4(-0.5f, 1, 0.5f, 0));
            o.rigidbody().SetMass(obj["mass"].float64());
            o.rigidbody().SetDynamicFriction(obj["dynamicFriction"].float64());
            o.rigidbody().SetStaticFriction(obj["staticFriction"].float64());
            o.rigidbody().restitution() = obj["restitution"].float64();

            state.movableObject = o;
        }

        {
            Transform waterPosition = Transform(Vector3(3.1f, 0, -0.2f));

            Physics::Trigger trigger = Physics::Trigger(waterPosition, Physics::CollisionShape::AABB, Vector4(-0.5f, 1, 0.5f, 0));
            trigger.SetTriggerEnterCallback(OhNoWater);

            state.waterTrigger = trigger;
        }
    }
}

void OnUpdate(Application& app)
{
    static GameState& state = *(GameState*) app.data;

    if (Input::GetKeyDown(Key::ESCAPE))
        app.Exit();

    {   // Debug Input
        if (Input::GetKeyDown(Key::GRAVE))
            state.showColliders = !state.showColliders;
    }

    {   // Move the image
        Vector3 direction = Vector3(
            (s32) Input::GetKey(Key::D) - (s32) Input::GetKey(Key::A),
            (s32) Input::GetKey(Key::W) - (s32) Input::GetKey(Key::S),
            0
        );

        direction.Normalize();
        state.animState = (direction.SqrLength() < 0.01f);

        state.playerObject.rigidbody().SetForce(state.force * direction);
    }

    // {   // Follow player
    //     state.camera.position.x = state.player.position().x;
    //     state.camera.position.y = state.player.position().y;
    // }
}

void OnRender(Application& app)
{
    static GameState& state = *(GameState*) app.data;

    R2D::Begin(state.camera);
    Imgui::Begin();

    {   // Render water
        const Animation& animation = state.tileset.animations[0];
        R2D::RenderSprite(animation.frames[38], state.waterTrigger.transform().transformMatrix());
    }

    {   // Render character
        const Animation& animation = state.playerAnimGroup.animations[state.animState];
        R2D::RenderSprite(animation.GetCurrentFrame(app.time), state.player.transformMatrix());
    }

    {   // Render movable
        const Animation& animation = state.tileset.animations[0];
        R2D::RenderSprite(animation.frames[32], state.movable.transformMatrix());
    }

    {   // Render obstacle
        const Animation& animation = state.tileset.animations[0];
        R2D::RenderSprite(animation.frames[26], state.obstacle.transformMatrix());
    }
    
    {   // Render Text
        StringView text = state.playerName;
        Vector2 size = Imgui::GetRenderedTextSize(text, state.font);

        Vector3 topLeft = Vector3(0.5f * (app.window.refWidth - size.x), 0, 0);
        Imgui::RenderText(text, state.font, topLeft);
    }

    R2D::End();
    Imgui::End();

    if (state.showColliders)
        Physics::RenderColliders(app, state.camera);
}

void OnShutdown(Application& app)
{
    static GameState& state = *(GameState*) app.data;
 
    state.font.Free();
    state.playerAnimGroup.Free();
}

void OnWindowResize(Application& app)
{
    static GameState& state = *(GameState*) app.data;
    f32 halfWidth = 0.5f * app.window.width / state.zoomLevel;
    f32 halfHeight = 0.5f * app.window.height / state.zoomLevel;
    state.camera.projection = Matrix4::Orthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.1f, 100);
}

// Set up the app
void CreateApp(Application& app)
{
    // Window settings
    app.window.name = "My Window";
    app.window.x = 100;
    app.window.y = 100;
    app.window.width  = 1024;
    app.window.height = 720;

    // App data
    app.data = (void*) &gGameState;

    // App functions
    app.OnInit = OnInit;
    app.OnUpdate = OnUpdate;
    app.OnRender = OnRender;
    app.OnShutdown = OnShutdown;
    app.OnWindowResize = OnWindowResize;
}