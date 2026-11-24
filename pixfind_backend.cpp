
#include "GalleryManager.h"
#include "pixfind_backend.h"

#include <cstring>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cctype>

GalleryManager& getGalleryManager()
{
    static GalleryManager manager;
    return manager;
}

std::vector<int> getFavoriteIDs()
{
    std::vector<int> ids;

    std::vector<Metadata> favorites =
        getGalleryManager().getFavorites();

    for (const auto& item : favorites)
    {
        ids.push_back(item.id);
    }

    return ids;
}

bool toggleFavoriteByID(int id)
{
    return getGalleryManager().toggleFavorite(id);
}

std::string selectMediaFile()
{
    return browseMediaFile();
}

bool saveMediaFile(
    std::string path,
    std::string title,
    std::string tags,
    std::string date,
    std::string description)
{
    return getGalleryManager().addImage(
        path, title, tags, date, description
    );
}

std::vector<MediaInfo> getAllMedia()
{
    namespace fs = std::filesystem;

    std::vector<MediaInfo> result;

    fs::path folder =
        "D:/PBL/raylib folder/TestImages/Media";

    if (!fs::exists(folder))
    {
        std::cout << "ERROR: Media folder not found!"
                  << std::endl;
        return result;
    }

    std::vector<Metadata> savedData =
        getGalleryManager().getMetadataForOtherModules();

    int nextID = 1;

    for (const auto& entry : fs::directory_iterator(folder))
    {
        if (!entry.is_regular_file())
            continue;

        std::string ext =
            entry.path().extension().string();

        std::transform(
            ext.begin(), ext.end(), ext.begin(),
            [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            }
        );

        bool isImage =
            ext == ".jpg" || ext == ".jpeg" ||
            ext == ".png" || ext == ".bmp" ||
            ext == ".gif" || ext == ".webp";

        bool isVideo =
            ext == ".mp4" || ext == ".avi" ||
            ext == ".mov" || ext == ".mkv";

        if (!isImage && !isVideo)
            continue;

        MediaInfo media = {};

        std::string fullPath =
            fs::absolute(entry.path()).string();

        std::string title =
            entry.path().stem().string();

        media.id = nextID++;

        std::strncpy(
            media.fileName,
            fullPath.c_str(),
            sizeof(media.fileName) - 1
        );

        std::strncpy(
            media.fileType,
            isImage ? "image" : "video",
            sizeof(media.fileType) - 1
        );

        std::strncpy(
            media.title,
            title.c_str(),
            sizeof(media.title) - 1
        );

        std::strncpy(
            media.description,
            "Gallery media",
            sizeof(media.description) - 1
        );

        for (const auto& saved : savedData)
        {
            if (fs::path(saved.fileName).filename() ==
                entry.path().filename())
            {
                media.id = saved.id;

                std::strncpy(
                    media.title, saved.title,
                    sizeof(media.title) - 1
                );

                std::strncpy(
                    media.tags, saved.tags,
                    sizeof(media.tags) - 1
                );

                std::strncpy(
                    media.date, saved.date,
                    sizeof(media.date) - 1
                );

                std::strncpy(
                    media.description, saved.description,
                    sizeof(media.description) - 1
                );

                break;
            }
        }

        result.push_back(media);
    }

    std::cout << "Gallery files found: "
              << result.size() << std::endl;

    return result;
}
