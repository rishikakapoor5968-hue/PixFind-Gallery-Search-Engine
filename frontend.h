
#include "GalleryManager.h"

GalleryManager gallery;

// Call this when the user clicks Browse Files.
string selectedFile = browseMediaFile();

// After the user fills in the metadata:
if (!selectedFile.empty()) {
    gallery.addImage(
        selectedFile,
        "My Photo",
        "Nature, Travel",
        "21-11-2026",
        "A beautiful photograph"
    );
}
