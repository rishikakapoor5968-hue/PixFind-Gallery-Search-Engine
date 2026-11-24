
#ifndef GALLERYMANAGER_H
#define GALLERYMANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <system_error>

#include <windows.h>
#include <commdlg.h>

using namespace std;
namespace fs = std::filesystem;

struct Metadata {
    int id;
    char fileName[260];       // Stored media path
    char fileType[10];        // image or video
    char title[50];
    char tags[100];
    char date[20];
    char description[100];
};

// Opens Windows file picker and returns selected file path.
// Returns an empty string if the user cancels.
inline string browseMediaFile() {
    char filePath[4096] = {};

    OPENFILENAMEA ofn = {};

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = sizeof(filePath);

    ofn.lpstrFilter =
        "Media Files\0"
        "*.jpg;*.jpeg;*.png;*.bmp;*.gif;*.webp;*.mp4;*.avi;*.mov;*.mkv\0"
        "Image Files\0"
        "*.jpg;*.jpeg;*.png;*.bmp;*.gif;*.webp\0"
        "Video Files\0"
        "*.mp4;*.avi;*.mov;*.mkv\0"
        "All Files\0"
        "*.*\0";

    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
        return string(filePath);
    }

    return "";
}

class GalleryManager {

private:

    vector<Metadata> data;
    string file = "metadata.dat";

    // Detect image or video using file extension.
    string getMediaType(const string& fileName) {
        string extension = fs::path(fileName).extension().string();

        transform(
            extension.begin(),
            extension.end(),
            extension.begin(),
            [](unsigned char c) {
                return static_cast<char>(tolower(c));
            }
        );

        if (extension == ".mp4" ||
            extension == ".avi" ||
            extension == ".mov" ||
            extension == ".mkv") {
            return "video";
        }

        if (extension == ".jpg" ||
            extension == ".jpeg" ||
            extension == ".png" ||
            extension == ".bmp" ||
            extension == ".gif" ||
            extension == ".webp") {
            return "image";
        }

        return "";
    }

    // Generate the next available ID.
    int generateID() {
        int maxID = 0;

        for (const auto& item : data) {
            if (item.id > maxID) {
                maxID = item.id;
            }
        }

        return maxID + 1;
    }

    // Load metadata from file.
    void loadFile() {
        data.clear();

        ifstream fin(file, ios::binary);

        if (!fin) {
            return;
        }

        Metadata m;

        while (fin.read(reinterpret_cast<char*>(&m), sizeof(Metadata))) {
            data.push_back(m);
        }

        fin.close();
    }

    // Save metadata to file.
    void saveFile() {
        ofstream fout(file, ios::binary | ios::trunc);

        if (!fout) {
            cout << "Error saving metadata file.\n";
            return;
        }

        for (const auto& item : data) {
            fout.write(
                reinterpret_cast<const char*>(&item),
                sizeof(Metadata)
            );
        }

        fout.close();
    }

    // Copy selected media into the project's Media folder.
    string copyMediaFile(const string& sourcePath, int id) {
        try {
            fs::path source(sourcePath);

            if (!fs::exists(source) || !fs::is_regular_file(source)) {
                cout << "Selected file does not exist.\n";
                return "";
            }

            fs::create_directories("Media");

            string newName =
                to_string(id) + "_" + source.filename().string();

            fs::path destination = fs::path("Media") / newName;

            error_code ec;

            fs::copy_file(
                source,
                destination,
                fs::copy_options::none,
                ec
            );

            if (ec) {
                cout << "Could not copy file: " << ec.message() << endl;
                return "";
            }

            return destination.string();
        }
        catch (const fs::filesystem_error& e) {
            cout << "File error: " << e.what() << endl;
            return "";
        }
    }

public:

    GalleryManager() {
        loadFile();
    }

    // Add selected image or video.
    bool addImage(
        string sourcePath,
        string title,
        string tags,
        string date,
        string description
    ) {
        if (sourcePath.empty()) {
            cout << "No file selected.\n";
            return false;
        }

        if (!fs::exists(sourcePath)) {
            cout << "File does not exist.\n";
            return false;
        }

        string type = getMediaType(sourcePath);

        if (type.empty()) {
            cout << "Unsupported file format.\n";
            return false;
        }

        int newID = generateID();

        // Copy file into the project's Media folder.
        string storedPath = copyMediaFile(sourcePath, newID);

        if (storedPath.empty()) {
            return false;
        }

        if (storedPath.length() >= sizeof(Metadata::fileName)) {
            error_code ec;
            fs::remove(storedPath, ec);

            cout << "File path is too long.\n";
            return false;
        }

        Metadata m = {};

        m.id = newID;

        strncpy(m.fileName, storedPath.c_str(), sizeof(m.fileName) - 1);
        strncpy(m.fileType, type.c_str(), sizeof(m.fileType) - 1);
        strncpy(m.title, title.c_str(), sizeof(m.title) - 1);
        strncpy(m.tags, tags.c_str(), sizeof(m.tags) - 1);
        strncpy(m.date, date.c_str(), sizeof(m.date) - 1);
        strncpy(m.description, description.c_str(), sizeof(m.description) - 1);

        data.push_back(m);

        saveFile();

        cout << type << " added successfully! ID = " << newID << endl;

        return true;
    }

    // Open file picker and add the selected media.
    bool browseAndAdd(
        string title,
        string tags,
        string date,
        string description
    ) {
        string selectedFile = browseMediaFile();

        if (selectedFile.empty()) {
            cout << "File selection cancelled.\n";
            return false;
        }

        return addImage(
            selectedFile,
            title,
            tags,
            date,
            description
        );
    }

    // Add multiple files when their paths are already available.
    void addMultipleImages(
        vector<string> fileNames,
        string titlePrefix,
        string tags,
        string date,
        string description
    ) {
        for (size_t i = 0; i < fileNames.size(); i++) {
            string title = titlePrefix + " " + to_string(i + 1);

            addImage(
                fileNames[i],
                title,
                tags,
                date,
                description
            );
        }
    }

    // Display all saved media.
    void viewGallery() {
        if (data.empty()) {
            cout << "Gallery is empty.\n";
            return;
        }

        cout << "\n------ PIXFIND GALLERY ------\n";

        for (const auto& item : data) {
            cout << "\nID: " << item.id
                 << "\nFile: " << item.fileName
                 << "\nType: " << item.fileType
                 << "\nTitle: " << item.title
                 << "\nTags: " << item.tags
                 << "\nDate: " << item.date
                 << "\nDescription: " << item.description
                 << "\n----------------------------\n";
        }
    }

    // Search media by tag.
    vector<Metadata> searchByTag(string tag) {
        vector<Metadata> result;

        for (const auto& item : data) {
            if (string(item.tags).find(tag) != string::npos) {
                result.push_back(item);
            }
        }

        return result;
    }

    // Search media by date.
    vector<Metadata> searchByDate(string date) {
        vector<Metadata> result;

        for (const auto& item : data) {
            if (string(item.date).find(date) != string::npos) {
                result.push_back(item);
            }
        }

        return result;
    }

    // Search media by title.
    vector<Metadata> searchByTitle(string title) {
        vector<Metadata> result;

        for (const auto& item : data) {
            if (string(item.title).find(title) != string::npos) {
                result.push_back(item);
            }
        }

        return result;
    }

    // Delete media using its ID.
    bool deleteImage(int id) {
        for (auto it = data.begin(); it != data.end(); ++it) {
            if (it->id == id) {

                // Remove the copied media file.
                error_code ec;
                fs::remove(it->fileName, ec);

                data.erase(it);

                saveFile();

                cout << "Media deleted successfully.\n";
                return true;
            }
        }

        cout << "Media ID not found.\n";
        return false;
    }

    // Return metadata for other modules.
    vector<Metadata> getMetadataForOtherModules() {
        return data;
    }

    // Return total number of media files.
    int getTotalMedia() {
        return static_cast<int>(data.size());
    }
};

#endif
