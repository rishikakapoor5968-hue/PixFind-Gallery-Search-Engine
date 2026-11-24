#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <iostream>

using namespace std;

class HomePage {

public:

    void display() {

        cout << "\n\n";
        cout << "          DIGITAL GALLERY MANAGEMENT SYSTEM\n";
        cout << "               PBL PROJECT - DSA\n";

        cout << " Features Available:\n";
        cout << " 1. Gallery Management\n";
        cout << " 2. Fast ID Search (Hash Table)\n";
        cout << " 3. Keyword Search (Trie)\n";
        cout << " 4. Priority Ranking\n";
        cout << " 5. Recommendation System\n";
        cout << " 6. Metadata Storage\n\n";

        cout << " Data Structures Used:\n";
        cout << " * Vector\n";
        cout << " * Hash Table\n";
        cout << " * Trie\n";
        cout << " * Priority Queue / Heap\n";
        cout << " * Graph (Tag-Image Mapping)\n\n";

        cout << "\nPress Enter to Continue...";
        cin.get();
    }
};

#endif