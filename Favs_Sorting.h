#include "GalleryManager.h"
#include "SearchEngine.h"
#include "PriorityRanking.h"
#include "RecommendationSystem.h"
#include "Home.h"
#include "Dashboard.h"
#include "Hidden_Search_History.h"

#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <cctype>
using namespace std;

// Toggle Favorite - if favorite, remove. If not, make favorite.
    bool toggleFavorite(int id) {
        for (auto &item : data) {
            if (item.id == id) {
                item.isFavorite = !item.isFavorite;
                saveFile();
                cout << (item.isFavorite ? "Added to Favorites ⭐ : " : "Removed from Favorites : ") << item.title << endl;
                return true;
            }
        }
        cout << "ID not found!\n";
        return false;
    }

    // Get only favorite items
    vector<Metadata> getFavorites() {
        vector<Metadata> fav;
        for (auto &item : data) {
            if (item.isFavorite) fav.push_back(item);
        }
        return fav;
    }

    void viewFavorites() {
        vector<Metadata> fav = getFavorites();
        if (fav.empty()) {
            cout << "No favorites yet. Use toggleFavorite(id) to add.\n";
            return;
        }
        cout << "\n--- ⭐ FAVORITES (" << fav.size() << ") ---\n";
        for (auto &item : fav) {
            cout << "ID: " << item.id << " | " << item.title << " | " << item.fileName << " | " << item.date << endl;
        }
    }

    // ================= NEW FEATURE 2: ADVANCED SORTING =================
    // Sort by Newest Date First (YYYY-MM-DD works with string compare)
    vector<Metadata> sortByNewest() {
        vector<Metadata> sorted = data;
        sort(sorted.begin(), sorted.end(), [](Metadata &a, Metadata &b) {
            return string(a.date) > string(b.date); // Descending
        });
        return sorted;
    }
