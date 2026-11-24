#include <iostream>
#include <string>

#include "GalleryManager.h"
#include "SearchEngine.h"
#include "PriorityRanking.h"
#include "RecommendationSystem.h"
#include "Home.h"
#include "Dashboard.h"
#include "Favs_Sorting.h"


using namespace std;

// Node for Search History (Stack)
struct SearchNode {
    string search;
    SearchNode* next;
};

// Node for Hidden Folders (Linked List)
struct FolderNode {
    string folderName;
    FolderNode* next;
};

// Search History using Stack
class SearchHistory {
private:
    SearchNode* top;

public:
    SearchHistory() {
        top = NULL;
    }

    void addSearch(string s) {
        SearchNode* newNode = new SearchNode;
        newNode->search = s;
        newNode->next = top;
        top = newNode;

        cout << "Search added successfully!\n";
    }

    void displayHistory() {
        if (top == NULL) {
            cout << "No search history found.\n";
            return;
        }

        cout << "\n--- SEARCH HISTORY ---\n";
        SearchNode* temp = top;
        int i = 1;

        while (temp != NULL) {
            cout << i++ << ". " << temp->search << endl;
            temp = temp->next;
        }
    }

    void deleteHistory() {
        while (top != NULL) {
            SearchNode* temp = top;
            top = top->next;
            delete temp;
        }

        cout << "Search history cleared!\n";
    }

    ~SearchHistory() {
        deleteHistory();
    }
};

// Hidden Folders using Linked List
class HiddenFolders {
private:
    FolderNode* head;

public:
    HiddenFolders() {
        head = NULL;
    }

    void hideFolder(string name) {
        FolderNode* temp = head;

        while (temp != NULL) {
            if (temp->folderName == name) {
                cout << "Folder is already hidden!\n";
                return;
            }
            temp = temp->next;
        }

        FolderNode* newNode = new FolderNode;
        newNode->folderName = name;
        newNode->next = head;
        head = newNode;

        cout << "Folder hidden successfully!\n";
    }

    void displayFolders() {
        if (head == NULL) {
            cout << "No hidden folders found.\n";
            return;
        }

        cout << "\n--- HIDDEN FOLDERS ---\n";
        FolderNode* temp = head;
        int i = 1;

        while (temp != NULL) {
            cout << i++ << ". " << temp->folderName << endl;
            temp = temp->next;
        }
    }

    void unhideFolder(string name) {
        FolderNode* temp = head;
        FolderNode* prev = NULL;

        while (temp != NULL) {
            if (temp->folderName == name) {
                if (prev == NULL)
                    head = temp->next;
                else
                    prev->next = temp->next;

                delete temp;
                cout << "Folder unhidden successfully!\n";
                return;
            }

            prev = temp;
            temp = temp->next;
        }

        cout << "Folder not found!\n";
    }

    ~HiddenFolders() {
        while (head != NULL) {
            FolderNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

// Main Program
int main() {
    SearchHistory history;
    HiddenFolders folders;

    int choice;
    string name;

    do {
        cout << "\n===== FILE MANAGER =====\n";
        cout << "1. Add Search\n";
        cout << "2. View Search History\n";
        cout << "3. Clear Search History\n";
        cout << "4. Hide Folder\n";
        cout << "5. View Hidden Folders\n";
        cout << "6. Unhide Folder\n";
        cout << "7. Exit\n";

        cout << "\nEnter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "Enter search: ";
                getline(cin, name);
                history.addSearch(name);
                break;

            case 2:
                history.displayHistory();
                break;

            case 3:
                history.deleteHistory();
                break;

            case 4:
                cout << "Enter folder name: ";
                getline(cin, name);
                folders.hideFolder(name);
                break;

            case 5:
                folders.displayFolders();
                break;

            case 6:
                cout << "Enter folder name to unhide: ";
                getline(cin, name);
                folders.unhideFolder(name);
                break;

            case 7:
                cout << "Exiting program...\n";
                break;

            default:
                cout << "Invalid choice!\n";
        }

    } while (choice != 7);

    return 0;
}