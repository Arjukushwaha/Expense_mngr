#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <unordered_map>

using namespace std;

string simpleHash(const string &str) {
    unsigned long hash = 5381;
    int c;
    for (char ch : str) {
        c = ch;
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return to_string(hash);
}


class User {
    string username;
    string password;
public:
    User(string uname, string pwd) : username(uname), password(simpleHash(pwd)) {}

    string getUsername() {
        return username;
    }

    string getPassword() {
        return password;
    }
};


class Expense {
public:
    double amount;
    string category;
    string description;
    string date;

    Expense(double amt, string cat, string desc, string dt)
        : amount(amt), category(cat), description(desc), date(dt) {}

    void displayExpense() {
        cout << "Date: " << date << " | Category: " << category 
             << " | Amount: " << amount << " | Description: " << description << endl;
    }

    string getCategory() {
        return category;
    }

    string getDate() {
        return date;
    }

    double getAmount() {
        return amount;
    }
};

class ExpenseTracker {
    vector<Expense> expenses;
    double budget;
    User currentUser;
public:
    ExpenseTracker(User user) : currentUser(user), budget(0) {}

    void setBudget(double bud) {
        budget = bud;
    }

    void addExpense(double amt, string cat, string desc, string dt) {
        expenses.push_back(Expense(amt, cat, desc, dt));
        checkBudget();
    }

    void viewExpenses() {
        cout << "\nExpense List:\n";
        for (Expense &e : expenses) {
            e.displayExpense();
        }
    }

    void checkBudget() {
        double total = 0;
        for (Expense &e : expenses) {
            total += e.amount;
        }
        if (total > budget) {
            cout << "\n** Alert! You have exceeded your budget of " << budget << " **\n";
        } else {
            cout << "\nCurrent spending: " << total << " / " << budget << "\n";
        }
    }

  
    void filterByCategory(string category) {
        cout << "\nExpenses in Category: " << category << "\n";
        for (Expense &e : expenses) {
            if (e.getCategory() == category) {
                e.displayExpense();
            }
        }
    }

    void filterByDateRange(string startDate, string endDate) {
        cout << "\nExpenses from " << startDate << " to " << endDate << "\n";
        for (Expense &e : expenses) {
            if (e.getDate() >= startDate && e.getDate() <= endDate) {
                e.displayExpense();
            }
        }
    }

    void displayCategoryChart() {
        unordered_map<string, double> categoryTotals;

        for (Expense &e : expenses) {
            categoryTotals[e.getCategory()] += e.getAmount();
        }

        cout << "\nSpending per Category (ASCII Bar Chart):\n";
        for (auto &cat : categoryTotals) {
            cout << cat.first << ": ";
            int barLength = static_cast<int>(cat.second / 10);
            for (int i = 0; i < barLength; ++i) {
                cout << "|";
            }
            cout << " " << cat.second << "\n";
        }
    }


    string getCurrentDate() {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        return to_string(ltm->tm_mday) + "-" + to_string(1 + ltm->tm_mon) + "-" + to_string(1900 + ltm->tm_year);
    }
};

User loginSystem() {
    string username, password;
    int choice;

    cout << "1. Register\n2. Login\nChoose: ";
    cin >> choice;

    if (choice == 1) {
        cout << "Register\n";
        cout << "Enter Username: ";
        cin >> username;
        cout << "Enter Password: ";
        cin >> password;

        ofstream userFile("users.txt", ios::app);
        userFile << username << " " << simpleHash(password) << endl;
        userFile.close();
        cout << "Registration successful!\n";
    } else if (choice == 2) {
        cout << "Login\n";
        cout << "Enter Username: ";
        cin >> username;
        cout << "Enter Password: ";
        cin >> password;

        ifstream userFile("users.txt");
        string uname, pwd;
        bool found = false;
        while (userFile >> uname >> pwd) {
            if (uname == username && pwd == simpleHash(password)) {
                found = true;
                break;
            }
        }
        userFile.close();
        if (!found) {
            cout << "Invalid login details. Try again.\n";
            return loginSystem();
        }
    }

    return User(username, password);
}


void menu(ExpenseTracker &tracker) {
    int choice;
    do {
        cout << "\nExpense Tracker Menu:\n";
        cout << "1. Add Expense\n2. View Expenses\n3. Set Budget\n4. Filter by Category\n5. Filter by Date Range\n6. Show Category Chart\n7. Exit\nChoose: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                double amount;
                string category, description, date;

                cout << "Enter amount: ";
                cin >> amount;
                cout << "Enter category (e.g., Food, Transport): ";
                cin >> category;
                cout << "Enter description: ";
                cin.ignore();
                getline(cin, description);

                date = tracker.getCurrentDate();
                tracker.addExpense(amount, category, description, date);
                break;
            }
            case 2:
                tracker.viewExpenses();
                break;
            case 3: {
                double budget;
                cout << "Set monthly budget: ";
                cin >> budget;
                tracker.setBudget(budget);
                break;
            }
           
            case 4: {
                string category;
                cout << "Enter category to filter: ";
                cin >> category;
                tracker.filterByCategory(category);
                break;
            }
            case 5: {
                string startDate, endDate;
                cout << "Enter start date (dd-mm-yyyy): ";
                cin >> startDate;
                cout << "Enter end date (dd-mm-yyyy): ";
                cin >> endDate;
                tracker.filterByDateRange(startDate, endDate);
                break;
            }
            case 6:
                tracker.displayCategoryChart();
                break;
            case 7:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 7);
}

// Main function
int main() {
    User user = loginSystem();
    ExpenseTracker tracker(user);

    cout << "Welcome, " << user.getUsername() << "!\n";
    menu(tracker);

    return 0;
}