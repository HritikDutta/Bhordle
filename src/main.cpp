#include "core/application.h"
#include "core/input.h"
#include "engine/imgui.h"
#include "game/wordlist.h"

enum struct LetterState
{
    UNTOUCHED,
    WRONG,
    PLACED,
    CORRECT
};

namespace FontSizes {
    constexpr f32 LARGE = 64;
    constexpr f32 SMALL = 24;
}

struct GameState
{
    // Game Settings
    Imgui::Font font;
    
    char guesses[6][5]        = {};
    u8   correctMasks[6]      = {};
    u8   placedMasks[6]       = {};
    s32  placedCharacters[26] = {};
    LetterState letterStates[26] = {};
    bool invalidWord = false;

    s32 currentGuessIndex = 0;
    s32 filled = 0;
    s32 wordIndex = 0;
} gGameState;

constexpr u8 correctWordMask = 0x1F;

void ResetGame(GameState& state)
{
    PlatformZeroMemory(state.guesses, sizeof(state.guesses));
    PlatformZeroMemory(state.correctMasks, sizeof(state.correctMasks));
    PlatformZeroMemory(state.placedMasks, sizeof(state.placedMasks));
    PlatformZeroMemory(state.placedCharacters, sizeof(state.placedCharacters));
    PlatformZeroMemory(state.letterStates, sizeof(state.letterStates));
    state.invalidWord = false;
    state.currentGuessIndex = 0;
    state.filled = 0;
    state.wordIndex = 0;

    state.wordIndex = answersEnd * Math::Random();

    for (int i = 0; i < 5; i++)
        state.placedCharacters[wordList[state.wordIndex][i] - 'A']++;
}

void OnEventCheckInput(Application& app, Key key)
{
    GameState& state = *(GameState*) app.data;

    if (state.correctMasks[state.currentGuessIndex] != correctWordMask)
    {
        if (key == Key::BACKSPACE)
        {
            state.filled = (state.filled > 0) ? state.filled - 1 : 0;
            state.invalidWord = false;
        }

        if (key == Key::ESCAPE)
        {
            state.filled = 0;
            state.invalidWord = false;
        }

        if (key >= Key::A && key <= Key::Z)
        {
            if (state.currentGuessIndex < 6 && state.filled < 5)
                state.guesses[state.currentGuessIndex][state.filled++] = (char) key;
        }

        if (key == Key::ENTER)
        {
            if (state.filled == 5)
            {
                bool found = false;

                for (int i = 0; !found && i < wordListSize; i++)
                {
                    found = true;

                    for (int s = 0; s < 5; s++)
                    {
                        if (state.guesses[state.currentGuessIndex][s] != wordList[i][s])
                        {
                            found = false;
                            break;
                        }
                    }
                }

                if (found)
                {
                    s32 charsLeft[26];
                    PlatformCopyMemory(charsLeft, state.placedCharacters, sizeof(charsLeft));

                    // Check Correctness
                    for (int i = 0; i < 5; i++)
                    {
                        char guessChar = state.guesses[state.currentGuessIndex][i];

                        bool correct = wordList[state.wordIndex][i] == guessChar;
                        state.correctMasks[state.currentGuessIndex] |= ((s32) correct << i);

                        if (correct)
                        {
                            charsLeft[guessChar - 'A']--;
                            state.letterStates[guessChar - 'A'] = LetterState::CORRECT;
                        }
                    }

                    if (state.correctMasks[state.currentGuessIndex] != correctWordMask)
                    {
                        // Check for placed
                        for (int i = 0; i < 5; i++)
                        {
                            // Ignore correct guesses
                            if (state.correctMasks[state.currentGuessIndex] & (1 << i))
                                continue;
                            
                            char guessChar = state.guesses[state.currentGuessIndex][i];

                            bool placed = charsLeft[guessChar - 'A'] > 0;
                            state.placedMasks[state.currentGuessIndex] |= ((s32) placed << i);
                            charsLeft[guessChar - 'A']--;

                            // Save state based on priority (1. Correct, 2. Placed, 3. Wrong, 4. Untouched)
                            if (placed)
                            {
                                if (state.letterStates[guessChar - 'A'] != LetterState::CORRECT)
                                    state.letterStates[guessChar - 'A'] = LetterState::PLACED;
                            }
                            else
                            {
                                if (state.letterStates[guessChar - 'A'] == LetterState::UNTOUCHED)
                                    state.letterStates[guessChar - 'A'] = LetterState::WRONG;
                            }

                        }

                        state.currentGuessIndex++;
                        state.filled = 0;
                    }
                }
                else
                {
                    state.invalidWord = true;
                }
            }
        }
    }
}

void OnInit(Application& app)
{
    Input::RegisterKeyDownEventCallback(OnEventCheckInput);

    GameState& state = *(GameState*) app.data;

    state.font.Load("assets/fonts/atlas.font.png", "assets/fonts/atlas.font.json");

    ResetGame(state);
}

void OnRender(Application& app)
{
    GameState& state = *(GameState*) app.data;

    Imgui::Begin();

    constexpr s32 rectSize = 100;
    constexpr s32 startX = 10;
    constexpr s32 startY = 10;

    const Vector4 invalidColor = Vector4(0.5f, 0.0f, 0.0f, 1.0f);
    const Vector4 colors[] = {
        Vector4(0.5f, 0.5f, 0.5f, 1.0f),
        Vector4(0.2f, 0.2f, 0.2f, 1.0f),
        Vector4(0.88f, 0.66f, 0.0f, 1.0f),
        Vector4(0.003f, 0.71f, 0.003f, 1.0f)
    };

    {   // Render Rects
        Imgui::Rect rect;
        rect.size = Vector2(rectSize);
        rect.topLeft = Vector3(startX, startY, 0);

        for (int y = 0; y < 6; y++)
        {
            const Vector4& defaultColor = (y < state.currentGuessIndex) ? colors[(s32) LetterState::WRONG] : colors[(s32) LetterState::UNTOUCHED];

            for (int x = 0; x < 5; x++)
            {
                Vector4 color = defaultColor;
                s32 flag = 1 << x;

                if (state.invalidWord && y == state.currentGuessIndex)
                    color = invalidColor;
                else if (state.placedMasks[y] & flag)
                    color = colors[(s32) LetterState::PLACED];
                else if (state.correctMasks[y] & flag)
                    color = colors[(s32) LetterState::CORRECT];

                Imgui::RenderRect(rect, color);

                rect.topLeft.x += rectSize + 10;
            }

            rect.topLeft.y += rectSize + 10;
            rect.topLeft.x = startX;
        }
    }

    {   // Render Text
        Vector3 rectTopLeft = Vector3(startX, startY, 0);

        // Render previous guesses
        for (int g = 0; g < state.currentGuessIndex; g++)
        {
            for (int i = 0; i < 5; i++)
            {
                char ch = state.guesses[g][i];

                Vector2 size = Imgui::GetRenderedCharSize(ch, state.font, FontSizes::LARGE);

                Vector3 topLeft = rectTopLeft;
                topLeft.x += ((rectSize - size.x) / 2);
                topLeft.y += ((rectSize - size.y) / 2);
                
                Imgui::RenderChar(ch, state.font, topLeft, FontSizes::LARGE);

                rectTopLeft.x += rectSize + 10;
            }

            rectTopLeft.y += rectSize + 10;
            rectTopLeft.x = startX;
        }

        // Render current guess
        if (state.currentGuessIndex < 6)
        {
            for (int i = 0; i < state.filled; i++)
            {
                char ch = state.guesses[state.currentGuessIndex][i];

                Vector2 size = Imgui::GetRenderedCharSize(ch, state.font, FontSizes::LARGE);

                Vector3 topLeft = rectTopLeft;
                topLeft.x += ((rectSize - size.x) / 2);
                topLeft.y += ((rectSize - size.y) / 2);
                
                Imgui::RenderChar(ch, state.font, topLeft, FontSizes::LARGE);

                rectTopLeft.x += rectSize + 10;
            }
        }
    }

    {   // Render Keyboard
        constexpr char keyboardLayout[] = "QWERTYUIOP_ASDFGHJKL_ZXCVBNM";

        s32 startX = 580;
        s32 startY = 475;
        
        f32 offsetsX[2] = { 0.75f, 1.75f };

        Imgui::Rect rect;
        rect.size = Vector2(35, 55);
        rect.topLeft = Vector3(startX, startY, 0);

        s32 offsetIndex = 0;
        for (int i = 0; keyboardLayout[i]; i++)
        {
            if (keyboardLayout[i] == '_')
            {
                rect.topLeft.x = startX + offsetsX[offsetIndex++] * rect.size.x;
                rect.topLeft.y += rect.size.y + 10;
                continue;
            }

            {   // Render Background Rect
                LetterState lstate = state.letterStates[keyboardLayout[i] - 'A'];
                if (Imgui::RenderButton(GenImguiIDWithSecondary(i), rect, colors[(s32) lstate], colors[(s32) lstate] + Vector4(0.25f, 0.25f, 0.25f, 0.0f), colors[(s32) lstate]))
                    OnEventCheckInput(app, (Key)(keyboardLayout[i] - 'A' + (s32) Key::A));
            }
            
            {
                Vector2 size = Imgui::GetRenderedCharSize(keyboardLayout[i], state.font, FontSizes::SMALL);

                Vector3 topLeft = rect.topLeft;
                topLeft.x += ((rect.size.x - size.x) / 2);
                topLeft.y += ((rect.size.y - size.y) / 2);
                
                Imgui::RenderChar(keyboardLayout[i], state.font, topLeft, FontSizes::SMALL);
            }

            rect.topLeft.x += rect.size.x + 10;
        }

        {   // Render backspace and enter keys
            {   // Backspace
                StringView text = "DEL";
                Vector2 size = Imgui::GetRenderedTextSize(text, state.font, FontSizes::SMALL);

                Imgui::Rect btnRect = rect;

                {   // Button Background
                    btnRect.size.x += size.x;

                    if (Imgui::RenderButton(GenImguiID(), btnRect, colors[0], colors[0] + Vector4(0.25f, 0.25f, 0.25f, 0.0f), colors[0]))
                        OnEventCheckInput(app, Key::BACKSPACE);
                }

                {   // Button Text
                    Vector3 topLeft = btnRect.topLeft;
                    topLeft.x += ((btnRect.size.x - size.x) / 2);
                    topLeft.y += ((btnRect.size.y - size.y) / 2);
                    Imgui::RenderText(text, state.font, topLeft, FontSizes::SMALL);
                }
            }

            {   // Backspace
                StringView text = "RET";
                Vector2 size = Imgui::GetRenderedTextSize(text, state.font, FontSizes::SMALL);

                Imgui::Rect btnRect = rect;

                {   // Button Background
                    btnRect.size.x += size.x;
                    btnRect.topLeft.x = startX + offsetsX[offsetIndex - 1] * rect.size.x - btnRect.size.x - 10;

                    if (Imgui::RenderButton(GenImguiID(), btnRect, colors[0], colors[0] + Vector4(0.25f, 0.25f, 0.25f, 0.0f), colors[0]))
                        OnEventCheckInput(app, Key::ENTER);
                }

                {   // Button Text
                    Vector3 topLeft = btnRect.topLeft;
                    topLeft.x += ((btnRect.size.x - size.x) / 2);
                    topLeft.y += ((btnRect.size.y - size.y) / 2);
                    Imgui::RenderText(text, state.font, topLeft, FontSizes::SMALL);
                }
            }
        }
    }

    // Render answer if player is out of guesses but hasn't guess the correct word yet
    if (state.currentGuessIndex >= 6 && state.correctMasks[5] != correctWordMask)
    {
        s32 y = 10;

        {   // Game Finished Toast!
            char buffer[256];
            sprintf(buffer, "The word was: %s", wordList[state.wordIndex]);

            Vector2 size = Imgui::GetRenderedTextSize(buffer, state.font, FontSizes::SMALL);
            Imgui::Rect rect;

            {   // Render Background
                rect.size = size + Vector2(50, 20);
                rect.topLeft = Vector3((1618 - rect.size.x) / 2, y, 0);
                Imgui::RenderRect(rect, Vector4(1));
            }

            {   // Render Text
                Vector3 topLeft = rect.topLeft;
                topLeft.x += 25;
                topLeft.y += 10;
                Imgui::RenderText(buffer, state.font, topLeft, FontSizes::SMALL, Vector4(0.3f, 0.3f, 0.3f, 1.0f));
            }

            y += rect.size.y + 10;
        }

        {   // Play Again Button
            StringView text = "Play Again";
        
            Vector2 size = Imgui::GetRenderedTextSize(text, state.font, FontSizes::SMALL);
            Imgui::Rect rect;

            {   // Render Button
                rect.size = size + Vector2(50, 20);
                rect.topLeft = Vector3((1618 - rect.size.x) / 2, y, 0);
                if (Imgui::RenderButton(GenImguiID(), rect, colors[0], colors[0] + Vector4(0.25f, 0.25f, 0.25f, 1.0f), colors[0]))
                    ResetGame(state);
            }

            {   // Render Text
                Vector3 topLeft = rect.topLeft;
                topLeft.x += 25;
                topLeft.y += 10;
                Imgui::RenderText(text, state.font, topLeft, FontSizes::SMALL);
            }

            y += rect.size.y + 10;
        }
    }

    // Show a celebration text if the player has guessed the correct word
    if (state.correctMasks[state.currentGuessIndex] == correctWordMask)
    {
        s32 y = 10;

        {   // Victory Toast!
            StringView text = "You guessed it!";
        
            Vector2 size = Imgui::GetRenderedTextSize(text, state.font, FontSizes::SMALL);
            Imgui::Rect rect;

            {   // Render Background
                rect.size = size + Vector2(50, 20);
                rect.topLeft = Vector3((1618 - rect.size.x) / 2, y, 0);
                Imgui::RenderRect(rect, Vector4(1));
            }

            {   // Render Text
                Vector3 topLeft = rect.topLeft;
                topLeft.x += 25;
                topLeft.y += 10;
                Imgui::RenderText(text, state.font, topLeft, FontSizes::SMALL, Vector4(0.3f, 0.3f, 0.3f, 1.0f));
            }

            y += rect.size.y + 10;
        }

        {   // Play Again Button
            StringView text = "Play Again";
        
            Vector2 size = Imgui::GetRenderedTextSize(text, state.font, FontSizes::SMALL);
            Imgui::Rect rect;

            {   // Render Button
                rect.size = size + Vector2(50, 20);
                rect.topLeft = Vector3((1618 - rect.size.x) / 2, y, 0);
                if (Imgui::RenderButton(GenImguiID(), rect, colors[0], colors[0] + Vector4(0.25f, 0.25f, 0.25f, 1.0f), colors[0]))
                    ResetGame(state);
            }

            {   // Render Text
                Vector3 topLeft = rect.topLeft;
                topLeft.x += 25;
                topLeft.y += 10;
                Imgui::RenderText(text, state.font, topLeft, FontSizes::SMALL);
            }

            y += rect.size.y + 10;
        }
    }

    // Show a toast for invalid words
    if (state.invalidWord)
    {
        StringView text = "Not in word list";
        
        Vector2 size = Imgui::GetRenderedTextSize(text, state.font, FontSizes::SMALL);
        Imgui::Rect rect;

        {   // Render Background
            rect.size = size + Vector2(50, 20);
            rect.topLeft = Vector3((1618 - rect.size.x) / 2, 10, 0);
            Imgui::RenderRect(rect, Vector4(1));
        }

        {   // Render Text
            Vector3 topLeft = rect.topLeft;
            topLeft.x += 25;
            topLeft.y += 10;
            Imgui::RenderText(text, state.font, topLeft, FontSizes::SMALL, Vector4(0.3f, 0.3f, 0.3f, 1.0f));
        }
    }

    Imgui::End();
}

void CreateApp(Application& app)
{
    app.window.name = "Wordle!";
    app.window.iconPath = "assets/icons/wordle-icon.ico";
    app.window.x = 100;
    app.window.y = 100;
    app.window.width  = 1038;
    app.window.height = 670;

    app.data = (void*) &gGameState;

    app.OnInit = OnInit;
    app.OnRender = OnRender;
}