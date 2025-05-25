#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Contact {
    string name;
    string number;
    string group;
    Contact* left;
    Contact* right;
    int height;

    Contact(const string& n, const string& num, const string& grp)
        : name(n), number(num), group(grp), left(nullptr), right(nullptr), height(1) {}
};


class TelephoneDirectory {
private:
    Contact* root;
    string fileName;

    int getHeight(Contact* node) {
        if (node == nullptr) return 0;
        return node->height;
    }

    int calculateHeight(Contact* node) {
        if (node == nullptr) return 0;
        int leftHeight = getHeight(node->left);
        int rightHeight = getHeight(node->right);
        return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
    }

    int balanceFactor(Contact* node) {
        if (node == nullptr) return 0;
        return getHeight(node->left) - getHeight(node->right);
    }

    Contact* rotateLeft(Contact* node) {
        Contact* newRoot = node->right;
        Contact* temp = newRoot->left;

        newRoot->left = node;
        node->right = temp;

        node->height = calculateHeight(node);
        newRoot->height = calculateHeight(newRoot);

        return newRoot;
    }

    Contact* rotateRight(Contact* node) {
        Contact* newRoot = node->left;
        Contact* temp = newRoot->right;

        newRoot->right = node;
        node->left = temp;

        node->height = calculateHeight(node);
        newRoot->height = calculateHeight(newRoot);

        return newRoot;
    }

    Contact* insertContact(Contact* node, const string& name, const string& number, const string& group) {
        if (node == nullptr) {
            return new Contact(name, number,group);
        }

        if (name < node->name) {
            node->left = insertContact(node->left, name, number,group);
        }
        else if (name > node->name) {
            node->right = insertContact(node->right, name, number,group);
        }
        else {
            // Contact with the same name already exists
            return node;
        }

        node->height = calculateHeight(node);
        int balance = balanceFactor(node);

        // Left Left Case
        if (balance > 1 && name < node->left->name) {
            return rotateRight(node);
        }
        // Right Right Case
        if (balance < -1 && name > node->right->name) {
            return rotateLeft(node);
        }
        // Left Right Case
        if (balance > 1 && name > node->left->name) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        // Right Left Case
        if (balance < -1 && name < node->right->name) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    Contact* searchContactUtil(Contact* node, const string& name) {
        if (node == nullptr || node->name == name) {
            return node;
        }

        if (name < node->name) {
            return searchContactUtil(node->left, name);
        }

        return searchContactUtil(node->right, name);
    }

   void inorderTraversal(Contact* node) {
    if (node != nullptr) {
        inorderTraversal(node->left);
        cout << node->name << " - " << node->number << " (Group: " << node->group << ")" << endl;
        inorderTraversal(node->right);
    }
}


    void loadContactsFromFile() {
        ifstream inputFile(fileName);
        if (inputFile.is_open()) {
            string name, number,group;
            while (inputFile >> name >> number>> group) {
                root = insertContact(root, name, number, group);
            }
            inputFile.close();
        }
        else {
            cout << "File not found or unable to open. Creating a new file for contacts." << endl;
        }
    }
    Contact* updateContactUtil(Contact* node, const string& name, const string& newNumber) {
        if (node == nullptr) {
            // Contact not found
            return nullptr;
        }

        if (name < node->name) {
            node->left = updateContactUtil(node->left, name, newNumber);
        }
        else if (name > node->name) {
            node->right = updateContactUtil(node->right, name, newNumber);
        }
        else {
            // Contact found, update the number
            node->number = newNumber;
        }

        // Update height and balance the tree
        node->height = calculateHeight(node);
        int balance = balanceFactor(node);

        // Perform rotations if necessary
        if (balance > 1 && name < node->left->name) {
            return rotateRight(node);
        }
        if (balance < -1 && name > node->right->name) {
            return rotateLeft(node);
        }
        if (balance > 1 && name > node->left->name) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balance < -1 && name < node->right->name) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }


    void updateFile(const string& name, const string& newNumber) {
        ifstream inputFile(fileName);
        ofstream tempFile("temp.txt");

        if (inputFile.is_open() && tempFile.is_open()) {
            string contactName, contactNumber, contactGroup;

            while (inputFile >> contactName >> contactNumber >> contactGroup) {
                if (contactName == name) {
                    // Update the number for the specific contact
                    tempFile << contactName << " " << newNumber << " " << contactGroup << endl;
                }
                else {
                    tempFile << contactName << " " << contactNumber << " " << contactGroup << endl;
                }
            }

            inputFile.close();
            tempFile.close();

            // Remove the original file
            remove(fileName.c_str());

            // Rename the temporary file to the original file name
            if (rename("temp.txt", fileName.c_str()) != 0) {
                cout << "Error renaming file." << endl;
            }
        }
        else {
            cout << "File not found or unable to open." << endl;
        }
    }


    void inorderTraversalToFile(Contact* node, ofstream& outFile) {
        if (node != nullptr) {
            inorderTraversalToFile(node->left, outFile);
            outFile << node->name << " " << node->number << " " << node->group << endl;
            inorderTraversalToFile(node->right, outFile);
        }
    }
    void searchContactsByGroupUtil(Contact* node, const string& group) {
        if (node != nullptr) {
            searchContactsByGroupUtil(node->left, group);

            if (node->group == group) {
                cout << node->name << " - " << node->number << " (Group: " << node->group << ")" << endl;
            }

            searchContactsByGroupUtil(node->right, group);
        }
    }

    Contact* minValueNode(Contact* node) {
        Contact* current = node;
        while (current && current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    Contact* deleteContactUtil(Contact* node, const string& name) {
        if (node == nullptr) {
            return node;
        }

        if (name < node->name) {
            node->left = deleteContactUtil(node->left, name);
        }
        else if (name > node->name) {
            node->right = deleteContactUtil(node->right, name);
        }
        else {
            if (node->left == nullptr || node->right == nullptr) {
                Contact* temp = node->left ? node->left : node->right;

                if (temp == nullptr) {
                    temp = node;
                    node = nullptr;
                }
                else {
                    *node = *temp;
                }

                delete temp;
            }
            else {
                Contact* temp = minValueNode(node->right);

                node->name = temp->name;
                node->number = temp->number;

                node->right = deleteContactUtil(node->right, temp->name);
            }
        }

        if (node == nullptr) {
            return node;
        }

        node->height = calculateHeight(node);
        int balance = balanceFactor(node);

        // Left Left Case
        if (balance > 1 && balanceFactor(node->left) >= 0) {
            return rotateRight(node);
        }
        // Right Right Case
        if (balance < -1 && balanceFactor(node->right) <= 0) {
            return rotateLeft(node);
        }
        // Left Right Case
        if (balance > 1 && balanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        // Right Left Case
        if (balance < -1 && balanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }
    void deleteContactFromFile(const string& name) {
        ifstream inputFile(fileName);
        ofstream tempFile("temp.txt"); // Temporary file to write updated contacts

        if (inputFile.is_open() && tempFile.is_open()) {
            string contactName, contactNumber;

            while (inputFile >> contactName >> contactNumber) {
                // Check if the contact read from the file matches the name to be deleted
                if (contactName != name) {
                    // Write the contact to the temporary file if it's not the contact to be deleted
                    tempFile << contactName << " " << contactNumber << endl;
                }
            }

            inputFile.close();
            tempFile.close();

            // Remove the original file
            remove(fileName.c_str());

            // Rename the temporary file to the original file name and check for success
            if (rename("temp.txt", fileName.c_str()) != 0) {
                cout << "Error renaming file." << endl;
            }
        }
        else {
            cout << "File not found or unable to open." << endl;
        }
    }
    void partialNameSearchUtil(Contact* node, const string& partialName) {
        if (node == nullptr) {
            return;
        }

        // Check if the partialName is a substring of the current contact's name
        size_t found = node->name.find(partialName);
        if (found != string::npos) {
            // Display the matching contact
            cout << "Partial match found: " << node->name << " - " << node->number << endl;
        }

        // Continue searching in both left and right subtrees
        partialNameSearchUtil(node->left, partialName);
        partialNameSearchUtil(node->right, partialName);
    }
    Contact* searchContactByNumberUtil(Contact* node, const string& number) {
        if (node == nullptr || node->number == number) {
            return node;
        }

        if (number < node->number) {
            return searchContactByNumberUtil(node->left, number);
        }

        return searchContactByNumberUtil(node->right, number);
    }

public:
    TelephoneDirectory() : root(nullptr), fileName("contacts.txt") {
        loadContactsFromFile();
    }

    void addContact(const string& name, const string& number, const string& group) {
        root = insertContact(root, name, number,group);
        cout << "Contact added: " << name << " - " << number << " (Group: " << group << ")" << endl;
        ofstream outFile(fileName, ios::app);
        if (outFile.is_open()) {
            outFile << name << " " << number << " " << group << endl;
            outFile.close();
        }
        else {
            cout << "Unable to open file for writing." << endl;
        }
        
    }

    void searchContact(const string& name) {
        Contact* found = searchContactUtil(root, name);
        if (found != nullptr) {
            cout << "Contact found: " << found->name << " - " << found->number << endl;
        }
        else {
            cout << "Contact not found" << endl;
        }
    }

    void displayDirectory() {
        if (root == nullptr) {
            cout << "Directory is empty" << endl;
            return;
        }

        cout << "Telephone Directory:" << endl;
        inorderTraversal(root);
    }
    void updateContact(const string& name, const string& newNumber) {
        root = updateContactUtil(root, name, newNumber);
        if (root != nullptr) {
            cout << "Contact updated: " << name << " - " << newNumber << endl;
            updateFile(name,newNumber);  // Update the file with the modified contacts
        }
        else {
            cout << "Contact not found" << endl;
        }
        
    }
    void deleteContact(const string& name) {
        root = deleteContactUtil(root, name);
        deleteContactFromFile(name);
    }
    void searchContactsByGroup(const string& group) {
        cout << "Contacts in Group '" << group << "':" << endl;
        searchContactsByGroupUtil(root, group);
    }
    void partialNameSearch(const string& partialName) {
        cout << "Partial matches found:" << endl;
        partialNameSearchUtil(root, partialName);
    }

    void deleteContactTree(Contact* node) {
        if (node != nullptr) {
            deleteContactTree(node->left);
            deleteContactTree(node->right);
            delete node;
        }
    }
    void clearDirectory() {
        deleteContactTree(root); // Delete all contacts
        ofstream outFile(fileName, ios::trunc); // Clear the file content
        if (outFile.is_open()) {
            outFile.close();
            cout << "Directory cleared successfully." << endl;
        }
        else {
            cout << "Unable to open file for writing." << endl;
        }
        root = nullptr; // Set root to nullptr after clearing
    }
    void searchContactByNumber(const string& number) {
        Contact* found = searchContactByNumberUtil(root, number);
        if (found != nullptr) {
            cout << "Contact found: " << found->name << " - " << found->number << endl;
        }
        else {
            cout << "Contact not found" << endl;
        }
    }
};



int main() {
    TelephoneDirectory directory;

    // Menu-driven telephone directory
    int choice;
    string name, number,newNumber,group;

    do {
        cout << "\n1. Add Contact\n2. Search Contact\n3. Display Directory\n4. Update Contact\n5. Delete Contact\n6. Search Contacts by Group\n7. Partial Name Search\n8. Clear Directory\n9. Search Contact by Number \n10. Exit\nEnter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Enter name: ";
            cin >> name;
            cout << "Enter number: ";
            cin >> number;
            cout << "Enter group: ";
            cin >> group;
            directory.addContact(name, number, group);
            break;
        case 2:
            cout << "Enter name to search: ";
            cin >> name;
            directory.searchContact(name);
            break;
        case 3:
            directory.displayDirectory();
            break;
        case 4:
            cout << "Enter name to update: ";
            cin >> name;
            cout << "Enter new number: ";
            cin >> newNumber;
            directory.updateContact(name, newNumber);
            break;
        case 5:
            cout << "Enter name to delete: ";
            cin >> name;
            directory.deleteContact(name);
            break;
        case 6:
            cout << "Enter group to search: ";
            cin >> group;
            directory.searchContactsByGroup(group);
            break;
        case 7:
            cout << "Enter partial name to search: ";
            cin >> name;
            directory.partialNameSearch(name);
            break;
        case 8:
            directory.clearDirectory();
            break;
        case 9:
            cout << "Enter number to search: ";
            cin >> number;
            directory.searchContactByNumber(number);
            break;
        case 10:
            cout << "Exiting... Thank you!" << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
        }
    } while (choice != 10);

    return 0;
}