#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <limits>
#include <string>  // 添加必要的头文件

using namespace std;

class Student {
public:
    string name;
    string id;
    vector<double> scores;

    double getTotal() const {
        double sum = 0;
        for (auto s : scores) {
            sum += s;
        }
        return sum;
    }

    bool hasFailed() const {
        for (auto score : scores) {
            if (score < 60.0) {
                return true;
            }
        }
        return false;
    }
};

class GradeSystem {
private:
    vector<Student> students;
    unordered_set<string> studentIds;
    const string datafile = "grades.dat";

    double getValidScore(int subjectNum) {
        double score;
        while (true) {
            cout << "Score for subject " << subjectNum << " (0-100): ";
            cin >> score;

            if (cin.fail() || score < 0 || score > 100) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input! Please enter a number between 0 and 100." << endl;
            }
            else {
                break;
            }
        }
        return score;
    }

public:
    void addStudent() {
        Student s;

        cout << "Enter student name: ";
        cin.ignore();
        getline(cin, s.name);

        string id;
        bool idExists;
        do {
            idExists = false;
            cout << "Enter student ID: ";
            cin >> id;

            if (studentIds.find(id) != studentIds.end()) {
                cout << "ID already exists! Please enter a unique ID." << endl;
                idExists = true;
            }
        } while (idExists);

        s.id = id;
        studentIds.insert(id);

        int subj;
        cout << "Number of subjects: ";
        cin >> subj;

        for (int i = 0; i < subj; i++) {
            s.scores.push_back(getValidScore(i + 1));
        }

        students.push_back(s);
        cout << "Student added successfully!" << endl;
    }

    void deleteStudent() {
        if (students.empty()) {
            cout << "No students to delete!" << endl;
            return;
        }

        string id;
        cout << "Enter student ID to delete: ";
        cin >> id;

        auto it = remove_if(students.begin(), students.end(),
            [&id](const Student& s) { return s.id == id; });

        if (it != students.end()) {
            students.erase(it, students.end());
            studentIds.erase(id);
            cout << "Student deleted successfully!" << endl;
        }
        else {
            cout << "Student not found!" << endl;
        }
    }

    void modifyStudent() {
        string id;
        cout << "Enter student ID to modify: ";
        cin >> id;

        auto it = find_if(students.begin(), students.end(),
            [&id](const Student& s) { return s.id == id; });

        if (it == students.end()) {
            cout << "Student not found!" << endl;
            return;
        }

        cout << "Current scores for " << it->name << ":" << endl;
        for (size_t i = 0; i < it->scores.size(); ++i) {
            cout << "Subject " << i + 1 << ": " << it->scores[i] << endl;
        }

        cout << "Enter new scores:" << endl;
        for (size_t i = 0; i < it->scores.size(); ++i) {
            it->scores[i] = getValidScore(i + 1);  // 修复多余括号
        }

        cout << "Scores updated successfully!" << endl;
    }

    void showAll() const {
        if (students.empty()) {
            cout << "No student data!" << endl;
            return;
        }

        for (const auto& s : students) {
            cout << "\nName: " << s.name
                << "\nID: " << s.id
                << "\nTotal: " << s.getTotal() << endl;
        }
    }

    void showClassAverage() const {
        if (students.empty()) {
            cout << "No student data!" << endl;
            return;
        }

        double total = 0;
        for (const auto& s : students) {
            total += s.getTotal();
        }
        cout << "Class average: " << total / students.size() << endl;
    }

    void showRanking(int topN = 0) const {
        if (students.empty()) {
            cout << "No student data!" << endl;
            return;
        }

        vector<Student> sortedStudents = students;
        sort(sortedStudents.begin(), sortedStudents.end(),
            [](const Student& a, const Student& b) {
                return a.getTotal() > b.getTotal();
            });

        int displayCount = (topN > 0) ? min(topN, (int)sortedStudents.size()) : sortedStudents.size();

        cout << "Ranking List:" << endl;
        for (int i = 0; i < displayCount; ++i) {
            cout << i + 1 << ". " << sortedStudents[i].name
                << " (Total: " << sortedStudents[i].getTotal() << ")" << endl;
        }
    }

    void showFailedStudents() const {
        if (students.empty()) {
            cout << "No student data!" << endl;
            return;
        }

        bool found = false;
        for (const auto& s : students) {
            if (s.hasFailed()) {
                cout << "\nName: " << s.name
                    << "\nID: " << s.id
                    << "\nTotal: " << s.getTotal() << endl;
                found = true;
            }
        }

        if (!found) {
            cout << "No failed students!" << endl;
        }
    }

    void saveToFile() {
        ofstream fout(datafile);
        if (!fout) {
            cerr << "Error opening file for writing!" << endl;
            return;
        }

        for (const auto& s : students) {
            fout << s.name << "," << s.id;
            for (auto score : s.scores) {
                fout << "," << score;
            }
            fout << endl;
        }
        cout << "Data saved successfully!" << endl;
    }

    void loadFromFile() {
        ifstream fin(datafile);
        if (!fin) {
            cerr << "No existing data file found." << endl;
            return;
        }

        students.clear();
        studentIds.clear();

        string line;
        while (getline(fin, line)) {
            Student s;
            size_t pos = 0;
            size_t next_pos;

            next_pos = line.find(',', pos);
            s.name = line.substr(pos, next_pos - pos);
            pos = next_pos + 1;

            next_pos = line.find(',', pos);
            s.id = line.substr(pos, next_pos - pos);
            pos = next_pos + 1;

            while (pos < line.length()) {
                next_pos = line.find(',', pos);
                if (next_pos == string::npos) {
                    next_pos = line.length();
                }
                double score = stod(line.substr(pos, next_pos - pos));
                s.scores.push_back(score);
                pos = next_pos + 1;
            }

            students.push_back(s);
            studentIds.insert(s.id);
        }

        cout << "Data loaded successfully! Loaded " << students.size() << " students." << endl;
    }
};

int main() {
    GradeSystem sys;
    sys.loadFromFile();

    int choice;
    do {
        cout << "\n1. Add Student\n2. Show All\n3. Modify Student\n4. Delete Student"
            << "\n5. Class Average\n6. Ranking\n7. Failed Students\n8. Save Data\n9. Exit"
            << "\nEnter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            sys.addStudent();
            break;
        case 2:
            sys.showAll();
            break;
        case 3:
            sys.modifyStudent();
            break;
        case 4:
            sys.deleteStudent();
            break;
        case 5:
            sys.showClassAverage();
            break;
        case 6:
        {
            int topN;
            cout << "Enter number of top students to show (0 for all): ";
            cin >> topN;
            sys.showRanking(topN);
        }
        break;
        case 7:
            sys.showFailedStudents();
            break;
        case 8:
            sys.saveToFile();
            break;
        case 9:
            sys.saveToFile();
            break;
        default:
            cout << "Invalid choice!" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (choice != 9);

    return 0;
}