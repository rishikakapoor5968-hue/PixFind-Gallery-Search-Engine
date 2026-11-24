#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <iostream>
#include <vector>
#include <string>

#include "GalleryManager.h"
#include "SearchEngine.h"
#include "PriorityRanking.h"
#include "RecommendationSystem.h"

using namespace std;

class Dashboard {

private:

    GalleryManager gallery;
    SearchEngine searchEngine;
    PriorityRanking ranking;
    RecommendationSystem recommendation;

public:

    void start() {

        int choice;

        do {

            cout << "\n====================================";
            cout << "\n DIGITAL GALLERY MANAGEMENT SYSTEM";
            cout << "\n====================================";

            cout << "\n1. Add Image/Video";
            cout << "\n2. Add Multiple Images";
            cout << "\n3. View Gallery";
            cout << "\n4. Delete Media";
            cout << "\n5. Build Search Indexes";
            cout << "\n6. Search Engine";
            cout << "\n7. Recommendation System";
            cout << "\n8. Priority Ranking";
            cout << "\n9. Exit";

            cout << "\n\nEnter Choice: ";
            cin >> choice;

            switch(choice) {

            case 1:
            {
                string fileName;
                string title;
                string tags;
                string date;
                string description;

                cout << "File Name: ";
                cin >> fileName;

                cin.ignore();

                cout << "Title: ";
                getline(cin,title);

                cout << "Tags: ";
                getline(cin,tags);

                cout << "Date: ";
                getline(cin,date);

                cout << "Description: ";
                getline(cin,description);

                gallery.addImage(
                    fileName,
                    title,
                    tags,
                    date,
                    description
                );

                break;
            }

            case 2:
            {
                int n;

                cout << "Number of Files: ";
                cin >> n;

                vector<string> files;

                for(int i=0;i<n;i++) {

                    string name;

                    cout << "File "
                         << i+1
                         << ": ";

                    cin >> name;

                    files.push_back(name);
                }

                cin.ignore();

                string titlePrefix;
                string tags;
                string date;
                string description;

                cout << "Title Prefix: ";
                getline(cin,titlePrefix);

                cout << "Tags: ";
                getline(cin,tags);

                cout << "Date: ";
                getline(cin,date);

                cout << "Description: ";
                getline(cin,description);

                gallery.addMultipleImages(
                    files,
                    titlePrefix,
                    tags,
                    date,
                    description
                );

                break;
            }

            case 3:

                gallery.viewGallery();
                break;

            case 4:
            {
                int id;

                cout << "Enter ID: ";
                cin >> id;

                gallery.deleteImage(id);

                break;
            }

            case 5:
            {
                vector<Metadata> data =
                    gallery.getMetadataForOtherModules();

                searchEngine.buildIndex(data);

                recommendation.buildGraph(data);

                ranking.buildRanking(data);

                cout << "\nAll Indexes Built Successfully!\n";

                break;
            }

            case 6:

                searchEngine.searchMenu();

                break;

            case 7:
            {
                string tag;

                cout << "Enter Tag: ";
                cin >> tag;

                recommendation.recommendByTag(tag);

                break;
            }

            case 8:
            {
                vector<Metadata> data =
                    gallery.getMetadataForOtherModules();

                ranking.buildRanking(data);

                string keyword;

                cin.ignore();

                cout << "Enter Keyword: ";

                getline(cin, keyword);

                ranking.rankImages(keyword);

                break;
            }

            case 9:

                cout << "Thank You!\n";
                break;

            default:

                cout << "Invalid Choice\n";
            }

        } while(choice != 9);
    }
};

#endif