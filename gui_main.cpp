
#include "raylib.h"
#include "pixfind_backend.h"
#include "SearchHelper.h"

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>

// Release previously loaded thumbnails.
void unloadGalleryTextures(std::vector<Texture2D>& textures)
{
    for (auto& texture : textures)
    {
        if (IsTextureValid(texture))
            UnloadTexture(texture);
    }

    textures.clear();
}

// Load gallery thumbnails.
void loadGalleryTextures(
    const std::vector<MediaInfo>& media,
    std::vector<Texture2D>& textures)
{
    unloadGalleryTextures(textures);

    for (const auto& item : media)
    {
        Texture2D texture = {};

        if (strcmp(item.fileType, "image") == 0 &&
            item.fileName[0] != '\0')
        {
            std::string imagePath = item.fileName;

            if (!FileExists(imagePath.c_str()))
            {
                imagePath =
                    "D:/PBL/raylib folder/TestImages/" + imagePath;
            }

            if (FileExists(imagePath.c_str()))
            {
                texture = LoadTexture(imagePath.c_str());

                if (!IsTextureValid(texture))
                    texture = {};
            }
        }

        textures.push_back(texture);
    }
}

int main()
{
    const int screenWidth = 1200;
    const int screenHeight = 750;

    InitWindow(screenWidth, screenHeight, "PixFind - Digital Gallery");
    SetTargetFPS(60);

    bool darkMode = true;

    // 0 = Home, 1 = Gallery, 2 = Favourites,
    // 3 = Search, 4 = Recommendations, 5 = Settings
    int currentPage = 0;

    std::string selectedFile = "";

    std::vector<MediaInfo> galleryMedia = getAllMedia();
    std::vector<Texture2D> galleryTextures;

    // Load saved favourite IDs.
    std::vector<int> favoriteIDs = getFavoriteIDs();

    loadGalleryTextures(galleryMedia, galleryTextures);

    bool showMetadataForm = false;
    bool mediaSaved = false;

    int activeInput = -1;

    char statusMessage[100] = "";

    char titleInput[50] = "";
    char tagsInput[100] = "";
    char dateInput[20] = "";
    char descriptionInput[100] = "";

    float galleryScroll = 0;

    // Search variables
    char searchInput[100] = "";
    std::vector<int> searchResults;

    bool searchFocused = false;
    float searchScroll = 0;

    Color background;
    Color sidebar;
    Color card;
    Color text;
    Color secondary;
    Color accent;

    while (!WindowShouldClose())
    {
        // Theme colours
        if (darkMode)
        {
            background = {21, 21, 31, 255};
            sidebar = {29, 29, 43, 255};
            card = {37, 37, 56, 255};
            text = WHITE;
            secondary = {160, 160, 181, 255};
            accent = {139, 92, 246, 255};
        }
        else
        {
            background = {245, 246, 250, 255};
            sidebar = WHITE;
            card = {230, 233, 242, 255};
            text = {35, 35, 50, 255};
            secondary = {110, 110, 130, 255};
            accent = {105, 75, 220, 255};
        }

        // Add Media button
        Rectangle addMediaButton = {960, 155, 190, 45};

        if (currentPage == 1 &&
            !showMetadataForm &&
            CheckCollisionPointRec(GetMousePosition(), addMediaButton) &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            selectedFile = selectMediaFile();

            if (!selectedFile.empty())
            {
                showMetadataForm = true;
                mediaSaved = false;
                activeInput = -1;

                statusMessage[0] = '\0';

                titleInput[0] = '\0';
                tagsInput[0] = '\0';
                dateInput[0] = '\0';
                descriptionInput[0] = '\0';
            }
        }

        // Metadata form input
        if (showMetadataForm)
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                Vector2 mouse = GetMousePosition();

                if (CheckCollisionPointRec(
                    mouse, (Rectangle){340, 210, 520, 40}))
                    activeInput = 0;

                else if (CheckCollisionPointRec(
                    mouse, (Rectangle){340, 295, 520, 40}))
                    activeInput = 1;

                else if (CheckCollisionPointRec(
                    mouse, (Rectangle){340, 380, 520, 40}))
                    activeInput = 2;

                else if (CheckCollisionPointRec(
                    mouse, (Rectangle){340, 465, 520, 55}))
                    activeInput = 3;

                else
                    activeInput = -1;
            }

            char* activeBuffer = nullptr;
            int bufferSize = 0;

            switch (activeInput)
            {
                case 0:
                    activeBuffer = titleInput;
                    bufferSize = sizeof(titleInput);
                    break;

                case 1:
                    activeBuffer = tagsInput;
                    bufferSize = sizeof(tagsInput);
                    break;

                case 2:
                    activeBuffer = dateInput;
                    bufferSize = sizeof(dateInput);
                    break;

                case 3:
                    activeBuffer = descriptionInput;
                    bufferSize = sizeof(descriptionInput);
                    break;
            }

            // Keyboard input
            if (activeBuffer != nullptr)
            {
                int key = GetCharPressed();

                while (key > 0)
                {
                    int length = strlen(activeBuffer);

                    if (key >= 32 && key <= 126 &&
                        length < bufferSize - 1)
                    {
                        activeBuffer[length] = (char)key;
                        activeBuffer[length + 1] = '\0';
                    }

                    key = GetCharPressed();
                }

                if (IsKeyPressed(KEY_BACKSPACE))
                {
                    int length = strlen(activeBuffer);

                    if (length > 0)
                        activeBuffer[length - 1] = '\0';
                }
            }

            // Save and Cancel
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                Vector2 mouse = GetMousePosition();

                Rectangle saveButton = {430, 560, 120, 45};
                Rectangle cancelButton = {570, 560, 120, 45};

                if (CheckCollisionPointRec(mouse, saveButton))
                {
                    if (!selectedFile.empty() &&
                        strlen(titleInput) > 0)
                    {
                        mediaSaved = saveMediaFile(
                            selectedFile,
                            titleInput,
                            tagsInput,
                            dateInput,
                            descriptionInput
                        );

                        if (mediaSaved)
                        {
                            galleryMedia = getAllMedia();
                            favoriteIDs = getFavoriteIDs();

                            loadGalleryTextures(
                                galleryMedia,
                                galleryTextures
                            );

                            galleryScroll = 0;

                            strcpy(
                                statusMessage,
                                "Media saved successfully!"
                            );

                            showMetadataForm = false;
                            selectedFile = "";

                            searchResults.clear();
                        }
                        else
                        {
                            strcpy(
                                statusMessage,
                                "Could not save media. Try again."
                            );
                        }
                    }
                    else
                    {
                        strcpy(
                            statusMessage,
                            "Please enter a title!"
                        );
                    }
                }
                else if (CheckCollisionPointRec(mouse, cancelButton))
                {
                    showMetadataForm = false;
                    selectedFile = "";
                    mediaSaved = false;
                    activeInput = -1;
                }
            }
        }

        // Sidebar navigation
        Rectangle homeBtn = {20, 140, 190, 45};
        Rectangle galleryBtn = {20, 195, 190, 45};
        Rectangle favBtn = {20, 250, 190, 45};
        Rectangle searchBtn = {20, 305, 190, 45};
        Rectangle recommendBtn = {20, 360, 190, 45};
        Rectangle settingsBtn = {20, 670, 190, 45};

        Vector2 mouse = GetMousePosition();

        if (!showMetadataForm &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (CheckCollisionPointRec(mouse, homeBtn))
                currentPage = 0;

            else if (CheckCollisionPointRec(mouse, galleryBtn))
                currentPage = 1;

            else if (CheckCollisionPointRec(mouse, favBtn))
                currentPage = 2;

            else if (CheckCollisionPointRec(mouse, searchBtn))
                currentPage = 3;

            else if (CheckCollisionPointRec(mouse, recommendBtn))
                currentPage = 4;

            else if (CheckCollisionPointRec(mouse, settingsBtn))
                currentPage = 5;
        }

        // Theme toggle
        Rectangle themeButton = {960, 35, 190, 42};

        if (!showMetadataForm &&
            CheckCollisionPointRec(GetMousePosition(), themeButton) &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            darkMode = !darkMode;
        }

        // Search input handling
        if (currentPage == 3 && !showMetadataForm)
        {
            Rectangle searchInputRect = {270, 190, 650, 45};

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                searchFocused = CheckCollisionPointRec(
                    GetMousePosition(),
                    searchInputRect
                );
            }

            if (searchFocused)
            {
                int key = GetCharPressed();

                while (key > 0)
                {
                    int length = strlen(searchInput);

                    if (key >= 32 && key <= 126 &&
                        length < (int)sizeof(searchInput) - 1)
                    {
                        searchInput[length] = (char)key;
                        searchInput[length + 1] = '\0';
                    }

                    key = GetCharPressed();
                }

                if (IsKeyPressed(KEY_BACKSPACE))
                {
                    int length = strlen(searchInput);

                    if (length > 0)
                        searchInput[length - 1] = '\0';
                }
            }

            searchResults = searchMediaRecords(
                galleryMedia,
                searchInput
            );

            int searchRows = (
                (int)searchResults.size() + 2
            ) / 3;

            float searchContentHeight = searchRows * 220.0f;

            float searchMaxScroll = std::max(
                0.0f,
                searchContentHeight - 430.0f
            );

            searchScroll -= GetMouseWheelMove() * 45;

            if (searchScroll < 0)
                searchScroll = 0;

            if (searchScroll > searchMaxScroll)
                searchScroll = searchMaxScroll;
        }
        else
        {
            searchFocused = false;
        }

        // Gallery scrolling
        int columns = 3;

        int rows = (int)(
            (galleryMedia.size() + columns - 1) / columns
        );

        float cardHeight = 205;
        float rowSpacing = 220;
        float visibleHeight = 475;

        float contentHeight = rows * rowSpacing;

        float maxScroll = std::max(
            0.0f,
            contentHeight - visibleHeight
        );

        if (currentPage == 1 && !showMetadataForm)
        {
            galleryScroll -= GetMouseWheelMove() * 45;

            if (galleryScroll < 0)
                galleryScroll = 0;

            if (galleryScroll > maxScroll)
                galleryScroll = maxScroll;
        }

        BeginDrawing();

        ClearBackground(background);

        // Sidebar
        DrawRectangle(0, 0, 230, screenHeight, sidebar);

        DrawText("PIXFind", 35, 35, 30, accent);
        DrawText("DIGITAL GALLERY", 35, 75, 12, secondary);

        Rectangle navButtons[] = {
            homeBtn, galleryBtn, favBtn,
            searchBtn, recommendBtn, settingsBtn
        };

        int pageButtons[] = {0, 1, 2, 3, 4, 5};

        for (int i = 0; i < 6; i++)
        {
            if (currentPage == pageButtons[i])
                DrawRectangleRec(navButtons[i], card);
        }

        DrawText("HOME", 40, 153, 18,
                 currentPage == 0 ? accent : text);

        DrawText("Gallery", 40, 208, 18,
                 currentPage == 1 ? accent : text);

        DrawText("Favourites", 40, 263, 18,
                 currentPage == 2 ? accent : text);

        DrawText("Search", 40, 318, 18,
                 currentPage == 3 ? accent : text);

        DrawText("Recommendations", 40, 373, 16,
                 currentPage == 4 ? accent : text);

        DrawText("Settings", 40, 683, 18,
                 currentPage == 5 ? accent : text);

        // Header
        DrawText("Welcome to PixFind!", 270, 35, 28, text);

        DrawText(
            "Manage your memories in one place.",
            270, 75, 17, secondary
        );

        // Theme button
        DrawRectangleRec(themeButton, card);

        DrawText(
            darkMode ? "Dark Mode" : "Light Mode",
            985, 47, 18, text
        );

        DrawText(
            "Click to switch theme",
            975, 90, 13, secondary
        );

        // Page content
        if (currentPage == 0)
        {
            DrawText("Welcome to PixFind!", 270, 160, 28, text);

            DrawText(
                "Your personal digital gallery.",
                270, 205, 18, secondary
            );

            DrawText(
                "Organise and explore your memories.",
                270, 240, 18, secondary
            );
        }

        // GALLERY PAGE
        else if (currentPage == 1)
        {
            DrawText("Your Gallery", 270, 160, 28, text);

            DrawText(
                TextFormat(
                    "Total media loaded: %i",
                    (int)galleryMedia.size()
                ),
                270, 210, 22, RED
            );

            DrawRectangleRec(addMediaButton, accent);

            DrawText("+ Add Media", 985, 168, 18, WHITE);

            if (strlen(statusMessage) > 0)
            {
                DrawText(
                    statusMessage,
                    800, 210, 16,
                    mediaSaved ? GREEN : RED
                );
            }

            if (galleryMedia.empty())
            {
                DrawText(
                    "No media added yet!",
                    300, 270, 22, secondary
                );

                DrawText(
                    "Click + Add Media to get started.",
                    300, 310, 18, secondary
                );
            }
            else
            {
                BeginScissorMode(255, 225, 930, 485);

                for (int i = 0; i < (int)galleryMedia.size(); i++)
                {
                    int column = i % columns;
                    int row = i / columns;

                    float x = 270 + column * 285;
                    float y = 240 + row * rowSpacing - galleryScroll;

                    Rectangle mediaCard = {
                        x, y, 260, cardHeight
                    };

                    DrawRectangleRec(mediaCard, card);

                    DrawRectangleLinesEx(
                        mediaCard, 1, secondary
                    );

                    Rectangle imageArea = {
                        x + 10, y + 10, 240, 145
                    };

                    DrawRectangleRec(
                        imageArea,
                        darkMode
                            ? Color{25, 25, 38, 255}
                            : Color{210, 214, 225, 255}
                    );

                    // Image display
                    if (strcmp(
                        galleryMedia[i].fileType, "image") == 0)
                    {
                        if (i < (int)galleryTextures.size() &&
                            IsTextureValid(galleryTextures[i]) &&
                            galleryTextures[i].width > 0 &&
                            galleryTextures[i].height > 0)
                        {
                            Texture2D texture = galleryTextures[i];

                            float scaleX =
                                imageArea.width / texture.width;

                            float scaleY =
                                imageArea.height / texture.height;

                            float scale = std::min(scaleX, scaleY);

                            float drawWidth = texture.width * scale;
                            float drawHeight = texture.height * scale;

                            Rectangle source = {
                                0, 0,
                                (float)texture.width,
                                (float)texture.height
                            };

                            Rectangle destination = {
                                imageArea.x +
                                    (imageArea.width - drawWidth) / 2,

                                imageArea.y +
                                    (imageArea.height - drawHeight) / 2,

                                drawWidth,
                                drawHeight
                            };

                            DrawTexturePro(
                                texture,
                                source,
                                destination,
                                {0, 0},
                                0,
                                WHITE
                            );
                        }
                        else
                        {
                            DrawText(
                                "Image preview unavailable",
                                (int)x + 30,
                                (int)y + 75,
                                15,
                                secondary
                            );
                        }
                    }
                    else if (strcmp(
                        galleryMedia[i].fileType, "video") == 0)
                    {
                        DrawText(
                            "VIDEO",
                            (int)x + 95,
                            (int)y + 70,
                            25,
                            accent
                        );

                        DrawText(
                            "Video preview",
                            (int)x + 80,
                            (int)y + 105,
                            15,
                            secondary
                        );
                    }
                    else
                    {
                        DrawText(
                            "Unsupported media",
                            (int)x + 55,
                            (int)y + 75,
                            16,
                            secondary
                        );
                    }

                    DrawText(
                        galleryMedia[i].title,
                        (int)x + 12,
                        (int)y + 165,
                        18,
                        text
                    );

                    DrawText(
                        galleryMedia[i].fileType,
                        (int)x + 12,
                        (int)y + 187,
                        13,
                        secondary
                    );

                    // FAVOURITE BUTTON
                    bool isFavorite =
                        std::find(
                            favoriteIDs.begin(),
                            favoriteIDs.end(),
                            galleryMedia[i].id
                        ) != favoriteIDs.end();

                    Rectangle favButton = {
                        x + 215,
                        y + 10,
                        35,
                        30
                    };

                    DrawRectangleRec(
                        favButton,
                        isFavorite ? GOLD : DARKGRAY
                    );

                    DrawText(
                        isFavorite ? "*" : "+",
                        (int)favButton.x + 11,
                        (int)favButton.y + 4,
                        22,
                        WHITE
                    );

                    // Favourite button click handling
                    if (CheckCollisionPointRec(
                            GetMousePosition(), favButton) &&
                        CheckCollisionPointRec(
                            GetMousePosition(),
                            (Rectangle){255, 225, 930, 485}) &&
                        IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    {
                        if (toggleFavoriteByID(galleryMedia[i].id))
                        {
                            favoriteIDs = getFavoriteIDs();
                        }
                    }
                }

                EndScissorMode();

                if (maxScroll > 0)
                {
                    DrawText(
                        "Scroll to see more media",
                        850, 710, 13, secondary
                    );
                }
            }
        }

        // SEARCH PAGE
        else if (currentPage == 3)
        {
            DrawText(
                "Search Your Gallery",
                270, 145, 28, text
            );

            DrawText(
                "Search by title, filename, tags or description:",
                270, 170, 15, secondary
            );

            Rectangle searchBox = {270, 190, 650, 45};

            DrawRectangleRec(searchBox, card);

            DrawRectangleLinesEx(
                searchBox,
                2,
                searchFocused ? GREEN : accent
            );

            if (strlen(searchInput) == 0)
            {
                DrawText(
                    "Type here...",
                    285, 202, 18, secondary
                );
            }
            else
            {
                DrawText(
                    searchInput,
                    285, 202, 18, text
                );
            }

            if (strlen(searchInput) == 0)
            {
                DrawText(
                    "Start typing to find your media.",
                    270, 270, 18, secondary
                );
            }
            else if (searchResults.empty())
            {
                DrawText(
                    "No matching media found.",
                    270, 270, 22, RED
                );
            }
            else
            {
                DrawText(
                    TextFormat(
                        "%i result(s) found",
                        (int)searchResults.size()
                    ),
                    270, 250, 18, accent
                );

                int searchColumns = 3;

                BeginScissorMode(255, 280, 930, 430);

                for (int j = 0; j < (int)searchResults.size(); j++)
                {
                    int i = searchResults[j];

                    int column = j % searchColumns;
                    int row = j / searchColumns;

                    float x = 270 + column * 285;
                    float y = 290 + row * 220 - searchScroll;

                    Rectangle mediaCard = {
                        x, y, 260, 205
                    };

                    Rectangle imageArea = {
                        x + 10, y + 10, 240, 135
                    };

                    DrawRectangleRec(mediaCard, card);

                    DrawRectangleLinesEx(
                        mediaCard, 1, secondary
                    );

                    DrawRectangleRec(imageArea, DARKGRAY);

                    if (strcmp(
                        galleryMedia[i].fileType, "image") == 0)
                    {
                        if (i < (int)galleryTextures.size() &&
                            IsTextureValid(galleryTextures[i]) &&
                            galleryTextures[i].width > 0 &&
                            galleryTextures[i].height > 0)
                        {
                            Texture2D texture = galleryTextures[i];

                            float scale = std::min(
                                imageArea.width / texture.width,
                                imageArea.height / texture.height
                            );

                            float width = texture.width * scale;
                            float height = texture.height * scale;

                            Rectangle source = {
                                0, 0,
                                (float)texture.width,
                                (float)texture.height
                            };

                            Rectangle destination = {
                                imageArea.x +
                                    (imageArea.width - width) / 2,

                                imageArea.y +
                                    (imageArea.height - height) / 2,

                                width,
                                height
                            };

                            DrawTexturePro(
                                texture,
                                source,
                                destination,
                                {0, 0},
                                0,
                                WHITE
                            );
                        }
                        else
                        {
                            DrawText(
                                "Preview unavailable",
                                (int)x + 45,
                                (int)y + 70,
                                15,
                                secondary
                            );
                        }
                    }
                    else
                    {
                        DrawText(
                            "VIDEO",
                            (int)x + 95,
                            (int)y + 65,
                            24,
                            accent
                        );
                    }

                    DrawText(
                        galleryMedia[i].title,
                        (int)x + 12,
                        (int)y + 155,
                        18,
                        text
                    );

                    DrawText(
                        galleryMedia[i].fileType,
                        (int)x + 12,
                        (int)y + 180,
                        13,
                        secondary
                    );
                }

                EndScissorMode();
            }
        }

        // FAVOURITES PAGE
        else if (currentPage == 2)
        {
            DrawText("Favourites", 270, 160, 28, text);

            std::vector<int> favIndexes;

            // Find the gallery indexes of all favourite media.
            for (int i = 0; i < (int)galleryMedia.size(); i++)
            {
                if (std::find(
                        favoriteIDs.begin(),
                        favoriteIDs.end(),
                        galleryMedia[i].id
                    ) != favoriteIDs.end())
                {
                    favIndexes.push_back(i);
                }
            }

            if (favIndexes.empty())
            {
                DrawText(
                    "No favourite media yet!",
                    270, 220, 20, secondary
                );

                DrawText(
                    "Go to Gallery and click + on a media card.",
                    270, 260, 17, secondary
                );
            }
            else
            {
                DrawText(
                    TextFormat(
                        "Total favourites: %i",
                        (int)favIndexes.size()
                    ),
                    270, 205, 18, accent
                );

                int favColumns = 3;

                BeginScissorMode(255, 225, 930, 485);

                for (int j = 0; j < (int)favIndexes.size(); j++)
                {
                    int i = favIndexes[j];

                    int column = j % favColumns;
                    int row = j / favColumns;

                    float x = 270 + column * 285;
                    float y = 240 + row * 220;

                    Rectangle mediaCard = {
                        x, y, 260, 205
                    };

                    DrawRectangleRec(mediaCard, card);

                    DrawRectangleLinesEx(
                        mediaCard, 1, secondary
                    );

                    Rectangle imageArea = {
                        x + 10, y + 10, 240, 145
                    };

                    DrawRectangleRec(imageArea, DARKGRAY);

                    // Display favourite image thumbnail.
                    if (strcmp(
                        galleryMedia[i].fileType, "image") == 0)
                    {
                        if (i < (int)galleryTextures.size() &&
                            IsTextureValid(galleryTextures[i]) &&
                            galleryTextures[i].width > 0 &&
                            galleryTextures[i].height > 0)
                        {
                            Texture2D texture = galleryTextures[i];

                            float scale = std::min(
                                imageArea.width / texture.width,
                                imageArea.height / texture.height
                            );

                            float width = texture.width * scale;
                            float height = texture.height * scale;

                            Rectangle source = {
                                0, 0,
                                (float)texture.width,
                                (float)texture.height
                            };

                            Rectangle destination = {
                                imageArea.x +
                                    (imageArea.width - width) / 2,

                                imageArea.y +
                                    (imageArea.height - height) / 2,

                                width,
                                height
                            };

                            DrawTexturePro(
                                texture,
                                source,
                                destination,
                                {0, 0},
                                0,
                                WHITE
                            );
                        }
                        else
                        {
                            DrawText(
                                "Preview unavailable",
                                (int)x + 45,
                                (int)y + 75,
                                15,
                                secondary
                            );
                        }
                    }
                    else if (strcmp(
                        galleryMedia[i].fileType, "video") == 0)
                    {
                        DrawText(
                            "VIDEO",
                            (int)x + 95,
                            (int)y + 65,
                            25,
                            accent
                        );

                        DrawText(
                            "Video preview",
                            (int)x + 80,
                            (int)y + 100,
                            15,
                            secondary
                        );
                    }

                    DrawText(
                        galleryMedia[i].title,
                        (int)x + 12,
                        (int)y + 165,
                        18,
                        text
                    );

                    DrawText(
                        galleryMedia[i].fileType,
                        (int)x + 12,
                        (int)y + 187,
                        13,
                        secondary
                    );
                }

                EndScissorMode();
            }
        }

        // RECOMMENDATIONS PAGE
        else if (currentPage == 4)
        {
            DrawText("Recommendations", 270, 160, 28, text);

            DrawText(
                "Discover related photos and videos.",
                270, 205, 18, secondary
            );
        }

        // SETTINGS PAGE
        else if (currentPage == 5)
        {
            DrawText("Settings", 270, 160, 28, text);

            DrawText(
                "Customise your PixFind experience.",
                270, 205, 18, secondary
            );
        }

        // Metadata popup
        if (showMetadataForm)
        {
            DrawRectangle(
                0, 0, screenWidth, screenHeight,
                Fade(BLACK, 0.65f)
            );

            Rectangle popup = {300, 100, 600, 550};

            DrawRectangleRec(popup, RAYWHITE);
            DrawRectangleLinesEx(popup, 2, DARKGRAY);

            DrawText(
                "Add Media Details",
                450, 125, 28, DARKGRAY
            );

            // Title
            DrawText("Title", 340, 185, 18, DARKGRAY);

            DrawRectangleLines(
                340, 210, 520, 40,
                activeInput == 0 ? PURPLE : GRAY
            );

            DrawText(titleInput, 350, 220, 18, BLACK);

            // Tags
            DrawText("Tags", 340, 270, 18, DARKGRAY);

            DrawRectangleLines(
                340, 295, 520, 40,
                activeInput == 1 ? PURPLE : GRAY
            );

            DrawText(tagsInput, 350, 305, 18, BLACK);

            // Date
            DrawText("Date", 340, 355, 18, DARKGRAY);

            DrawRectangleLines(
                340, 380, 520, 40,
                activeInput == 2 ? PURPLE : GRAY
            );

            DrawText(dateInput, 350, 390, 18, BLACK);

            // Description
            DrawText(
                "Description",
                340, 440, 18, DARKGRAY
            );

            DrawRectangleLines(
                340, 465, 520, 55,
                activeInput == 3 ? PURPLE : GRAY
            );

            DrawText(
                descriptionInput,
                350, 480, 18, BLACK
            );

            if (strlen(statusMessage) > 0 && !mediaSaved)
            {
                DrawText(
                    statusMessage,
                    340, 530, 16, RED
                );
            }

            // Save and Cancel buttons
            DrawRectangle(
                430, 560, 120, 45, DARKGREEN
            );

            DrawText("Save", 465, 573, 20, WHITE);

            DrawRectangle(
                570, 560, 120, 45, MAROON
            );

            DrawText("Cancel", 595, 573, 20, WHITE);
        }

        EndDrawing();
    }

    // Release image textures before closing.
    unloadGalleryTextures(galleryTextures);

    CloseWindow();

    return 0;
}
