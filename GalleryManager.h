void loadFavorites() {
    favoriteIDs.clear();

    ifstream fin(favoritesFile);
    int id;

    while (fin >> id) {
        favoriteIDs.push_back(id);
    }

    fin.close();
}

bool saveFavorites() {
    ofstream fout(favoritesFile, ios::trunc);

    if (!fout.is_open()) {
        return false;
    }

    for (int id : favoriteIDs) {
        fout << id << endl;
    }

    fout.close();
    return fout.good();
}