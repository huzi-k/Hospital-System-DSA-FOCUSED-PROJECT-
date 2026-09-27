
#include <iostream> 
#include <string>
#include <queue>
#include <vector> 
#include <fstream>
#include <list>
using namespace std;

// patient node for patient BST
// one patient BST for each doctor

struct patient
{
    string name;
    int id;
    int roomnum;
    string procedure;
    patient* left;
    patient* right;

    patient(string n, int i, string p)
    {
        name = n;
        id = i;
        procedure = p;
        left = NULL;
        right = NULL;
    }
};

// stores emergency patient information
// used in emergency patient priority queue

struct emergencypatient
{
    string name;
    int id;
    int severity;
    string issue;
};

// comparator for emergency patient priority queue

struct patientcompare
{
    bool operator()(emergencypatient a, emergencypatient b)
    {
        return a.severity < b.severity;
    }
};

// priority queue for emergency patients (most critical patient at top)

priority_queue<emergencypatient, vector<emergencypatient>, patientcompare> emergencyqueue;

// doctor node for doctor BST

struct doctor
{
    string name;
    int id;
    bool available;
    doctor* left;
    doctor* right;

    patient* patientroot; // doctor points to patient bst

    doctor(string n, int i, bool b)
    {
        name = n;
        id = i;
        available = b;
        left = NULL;
        right = NULL;
        patientroot = NULL;
    }
};

// hospital departments
// each department has a BST of doctors

struct department
{
    string name;
    doctor* doctorroot; // department points to doctors 
};
department departments[5] = { {"Cardiology", nullptr},{"Neurology", nullptr}, {"Emergency", nullptr}, {"Gynecology", nullptr}, {"Oncology", nullptr} };


//room structure 

struct room
{
    doctor* roomdoctor;
    int capacity;
    room() {
        capacity = 10;
    }
};

// hashtable for room allocation
// room assigned based on patient ID

struct hashroom {
    list<patient*>* arr;
    int cap;
    hashroom() {
        cap = 100;
        arr = new list<patient*>[cap];
    }
    int hashfunc(int i) {
        return i % cap;
    }

    // assign patient to room using hashing
    // linear probing if capacity reached 

    void patienttoroom(patient* p)
    {
        int hash = hashfunc(p->id);
        int i = 0;
        while (arr[hash].size() >= 10) {
            if (i == 100) {
                cout << "ALL ROOMS ARE FULL";
                return;
            }
            hash = (hash + 1) % cap;
            i++;
        }
        p->roomnum = hash;
        //cout << "\nPatient Assigned To Room Number: " << hash << endl;
        arr[hash].push_back(p);
    }
};

// adds emergency patient to priority queue

void emerpatient(priority_queue<emergencypatient, vector<emergencypatient>, patientcompare>& q, string name, int id, int severity, string issue)
{
    emergencypatient p;
    p.name = name;
    p.id = id;
    p.severity = severity;
    p.issue = issue;

    q.push(p);
    cout << "\nEmergency Patient Added Successfully !\n";
}

// displays emergency patients in priority order 

void prioritypatients(priority_queue<emergencypatient, vector<emergencypatient>, patientcompare> q)
{
    if (q.empty())
    {
        cout << "There are no patients waiting. \n";
        return;
    }
    while (!q.empty())
    {
        emergencypatient p = q.top();
        cout << "Treat Next: \n";
        cout << p.name << endl;
        cout << p.issue << endl;
        cout << "Severity: " << p.severity << endl;
        cout << "--------------------\n";
        q.pop();
    }

}

void displayroominfo(int roomnumber, hashroom* h)
{
    if (h->arr[roomnumber].empty()) {
        cout << "\nRoom Doesnt Exist\n";
        return;
    }

    cout << "Room: " << roomnumber << endl;

    for (auto it = h->arr[roomnumber].begin();it != h->arr[roomnumber].end();it++) {

        cout << "Patient: " << (*it)->name << endl;
        cout << "	ID: " << (*it)->id << endl;
    }
}

// stores inventory object information
// used for the restocking queue management

struct inventoryitem
{
    string name;
    string type;
    int quantity;
    int minquantity;
    int priority;
};

int findpriority(int quantity, int minquantity) // determines priority
{
    if (quantity <= minquantity * 0.25)
        return 4;
    if (quantity <= minquantity * 0.5)
        return 3;
    if (quantity <= minquantity * 0.75)
        return 2;
    else
        return 1;
}

// comparator for inventory priority queue 

struct compare
{
    bool operator()(inventoryitem a, inventoryitem b)
    {
        return a.priority < b.priority;
    }
};

// priority queue for inventory restocking

priority_queue<inventoryitem, vector<inventoryitem>, compare> inventory;

void additem(priority_queue<inventoryitem, vector<inventoryitem>, compare>& inventory, string name, string type, int quantity, int minquantity)
{
    inventoryitem item;
    item.name = name;
    item.type = type;
    item.quantity = quantity;
    item.minquantity = minquantity;
    item.priority = findpriority(quantity, minquantity);

    inventory.push(item);
}

void displayinventory(priority_queue<inventoryitem, vector<inventoryitem>, compare> inventory)
{
    if (inventory.empty())
    {
        cout << "Inventory is empty.\n";
        return;
    }
    cout << "======== INVENTORY RESTOCK STATUS =========\n";
    while (!inventory.empty())
    {
        inventoryitem item = inventory.top();
        cout << "Name: " << item.name << endl;
        cout << "Type: " << item.type << endl;
        cout << "Quantity: " << item.quantity << endl;
        cout << "Minimum Quantity: " << item.minquantity << endl;
        cout << "Priority: " << item.priority << endl;
        switch (item.priority)
        {
        case 1:
        {
            cout << "LOW" << endl;
            break;
        }
        case 2:
        {
            cout << "MODERATE" << endl;
            break;
        }
        case 3:
        {
            cout << "HIGH" << endl;
            break;
        }
        case 4:
        {
            cout << "URGENT" << endl;
            break;
        }
        }
        cout << "-------------------------\n";
        inventory.pop();
    }
}
// insert doctor into doctor BST
doctor* insertdoctor(doctor* root, string name, int id, bool av)
{
    if (!root)
    {
        doctor* newdoc = new doctor(name, id, av);
        return newdoc;
    }
    if (id < root->id)
    {
        root->left = insertdoctor(root->left, name, id, av);
    }
    else if (id > root->id)
    {
        root->right = insertdoctor(root->right, name, id, av);
    }
    return root;
}

//insert patient into patient BST

patient* insertpatient(patient* root, string name, int id, string procedure, hashroom* h)
{
    if (!root)
    {
        patient* newpat = new patient(name, id, procedure);
        h->patienttoroom(newpat);
        return newpat;
    }
    if (id < root->id)
    {
        root->left = insertpatient(root->left, name, id, procedure, h);
    }
    else if (id > root->id)
    {
        root->right = insertpatient(root->right, name, id, procedure, h);
    }
    return root;
}

doctor* finddoctor(doctor* root, int id)
{
    if (!root)
        return NULL;
    if (id == root->id)
        return root;
    if (id < root->id)
        return finddoctor(root->left, id);
    else
        return finddoctor(root->right, id);
}

// adds a patient under a specific doctor 
// patient inserted in doctor BST

void newpatient(int depindex, int docid, int patid, string patname, string procedure, hashroom* h)
{
    department& d = departments[depindex];
    doctor* doc = finddoctor(d.doctorroot, docid);
    if (doc == NULL) {
        cout << "Assigned Doctor Doesnt Exist\n";
        return;
    }
    doc->patientroot = insertpatient(doc->patientroot, patname, patid, procedure, h);


}

// inorder traversal of patient BST

void displaypatients(patient* root)
{
    if (!root)
        return;
    displaypatients(root->left);
    cout << "\n-------------------\n" << endl;
    cout << "\nPatient: " << root->name << endl;
    cout << "ID: " << root->id << endl;;
    cout << "Procedure: " << root->procedure << endl;
    cout << "Room Number: " << root->roomnum << endl;
    cout << "\n-------------------\n" << endl;
    displaypatients(root->right);
}

void displaydoctors(doctor* root)
{
    if (!root)
        return;
    displaydoctors(root->left);
    cout << "\n-------------------\n" << endl;
    cout << "Doctor: " << root->name << endl;
    cout << "ID: " << root->id << endl;
    cout << "Availability Status: ";
    if (root->available)
        cout << "Yes\n";
    else if (!root->available)
        cout << "No\n";
    cout << "\n-------------------\n" << endl;
    displaydoctors(root->right);

}

patient* findpatient(patient* root, int id)
{
    if (!root)
        return NULL;
    if (id == root->id)
        return root;
    if (id < root->id)
        return findpatient(root->left, id);
    else
        return findpatient(root->right, id);
}

// loads doctors and patients from file
void readData(string file, hashroom* h) {
    ifstream infile(file);
    string docname, patname, proc;
    int pcount, docid, patid, departnum;
    bool avail;

    while (infile >> departnum) {
        infile >> docid;
        infile.ignore();
        getline(infile, docname);
        infile >> avail;
        infile >> pcount;
        departments[departnum].doctorroot = insertdoctor(departments[departnum].doctorroot, docname, docid, avail);

        for (int i = 0;i < pcount;i++) {
            infile >> patid;
            infile.ignore();
            getline(infile, patname);
            infile >> proc;
            newpatient(departnum, docid, patid, patname, proc, h);
        }
    }
    cout << "Data Loaded From File...\n";
}
// loads inventory items from file
void readItems(string file) {
    ifstream infile(file);
    string n, t;
    int q, minq;
    while (infile >> n >> t >> q >> minq) {
        additem(inventory, n, t, q, minq);
    }
    cout << "Inventory Loaded From File...\n";
}

int main()
{
    hashroom* rooms = new hashroom;

    int choice;
    readData("hosp.txt", rooms);
    readItems("invent.txt");
    do
    {
        cout << "\n===== HOSPITAL MANAGEMENT SYSTEM =====\n";
        cout << "1. Add Doctor\n";
        cout << "2. Add Patient\n";
        cout << "3. Display Doctors in Department\n";
        cout << "4. Display Patients of Doctor\n";
        cout << "5. Search Patient\n";
        cout << "6. Display Room Information\n";
        cout << "7. Add Inventory Items\n";
        cout << "8. Display Inventory\n";
        cout << "9. Add Emergency Patient\n";
        cout << "10. Display Priority Patients\n";
        cout << "0. Exit\n";
        cout << endl << "Enter Choice: ";
        cin >> choice;
        cout << endl;

        switch (choice)
        {
        case 1:
        {
            int dep, id;
            string name;
            bool available;

            cout << "Departments:\n";
            cout << "0. Cardiology\n1. Neurology\n2. Emergency\n";
            cout << "3. Gynecology\n4. Oncology\n";
            cout << "Department: ";
            cin >> dep;
            if (dep < 0 || dep>4) {
                cout << "\nWrong Department\n";
                continue;
            }

            cout << "Doctor ID: ";
            cin >> id;

            cin.ignore();

            cout << "Doctor Name: ";
            getline(cin, name);

            cout << "Available (1=Yes, 0=No): ";
            cin >> available;

            doctor* doc = finddoctor(departments[dep].doctorroot, id); //doctor duplicate check
            if (doc) {
                cout << "\nDoctor ID already Exists\n";
                continue;
            }

            departments[dep].doctorroot = insertdoctor(departments[dep].doctorroot, name, id, available);

            cout << "\nDoctor Added Successfully!\n";
            break;
        }

        case 2:
        {
            int dep, docid, patid;
            string patname, procedure;

            cout << "Department Index (0-4): ";
            cin >> dep;
            if (dep < 0 || dep>4) {
                cout << "\nWrong Department\n";
                continue;
            }

            cout << "Doctor ID: ";
            cin >> docid;
            doctor* doc = finddoctor(departments[dep].doctorroot, docid);
            if (!doc) {
                cout << "\nDoctor Doesnt Exist\n";
                continue;
            }


            cout << "Patient ID: ";
            cin >> patid;

            cin.ignore();

            cout << "Patient Name: ";
            getline(cin, patname);

            cout << "Procedure: ";
            getline(cin, procedure);
            // patient duplicate check
            if (findpatient(doc->patientroot, patid)) {
                cout << "\nPatient Already Exists\n";
                continue;
            }
            newpatient(dep, docid, patid, patname, procedure, rooms);
            cout << "\nPatient Added Successfully!\n";

            break;
        }

        case 3:
        {
            int dep;

            cout << "Department Index (0-4): ";
            cin >> dep;
            if (dep < 0 || dep>4) {
                cout << "\nWrong Department\n";
                continue;
            }

            displaydoctors(departments[dep].doctorroot);
            break;
        }

        case 4:
        {
            int dep, docid;

            cout << "Department Index (0-4): ";
            cin >> dep;
            if (dep < 0 || dep>4) {
                cout << "\nWrong Department\n";
                continue;
            }

            cout << "Doctor ID: ";
            cin >> docid;

            doctor* doc = finddoctor(departments[dep].doctorroot, docid);

            if (doc) {
                displaypatients(doc->patientroot);
            }
            else {
                cout << "\nDoctor Not Found!\n";
            }

            break;
        }

        case 5:
        {
            int dep, docid, patid;

            cout << "Department Index (0-4): ";
            cin >> dep;
            if (dep < 0 || dep>4) {
                cout << "\nWrong Department\n";
                continue;
            }

            cout << "Doctor ID: ";
            cin >> docid;

            doctor* doc = finddoctor(departments[dep].doctorroot, docid);

            if (!doc)
            {
                cout << "\nDoctor Not Found!\n";
                break;
            }

            cout << "Patient ID: ";
            cin >> patid;

            patient* p = findpatient(doc->patientroot, patid);

            if (p)
            {
                cout << "\nPatient Found\n";
                cout << "Name: " << p->name << endl;
                cout << "ID: " << p->id << endl;
                cout << "Procedure: " << p->procedure << endl;
            }
            else
            {
                cout << "Patient Not Found\n";
            }

            break;
        }

        case 6:
        {
            int roomno;

            cout << "Room Number (0-99): ";
            cin >> roomno;
            if (roomno < 0 || roomno>99) {
                cout << "\nInvalid Room Number\n";
                continue;
            }

            displayroominfo(roomno, rooms);
            break;
        }
        case 7:
        {
            string name, type;
            int quantity, minquantity;

            cin.ignore();

            cout << "Item Name: ";
            getline(cin, name);

            cout << "Type (Medicine / Tool): ";
            getline(cin, type);

            cout << "Current Quantity: ";
            cin >> quantity;

            cout << "Minimum Quantity Allowed: ";
            cin >> minquantity;

            additem(inventory, name, type, quantity, minquantity);
            cout << "\nItem Added.\n";
            break;
        }
        case 8:
        {
            displayinventory(inventory);
            break;
        }
        case 9:
        {
            string name, issue;
            int id, severity;

            cin.ignore();

            cout << "Name: ";
            getline(cin, name);

            cout << "ID: ";
            cin >> id;

            cin.ignore();

            cout << "Issue: ";
            getline(cin, issue);

            cout << "Severity (from 0 to 5): ";
            cin >> severity;
            if (severity < 0 || severity>5) {
                cout << "\nWrong Severity Added\n";
                continue;
            }

            emerpatient(emergencyqueue, name, id, severity, issue);
            break;
        }
        case 10:
        {
            prioritypatients(emergencyqueue);
            break;
        }
        case 0:
        {
            cout << "\nExiting System...\n";
            break;
        }
        default:
        {
            cout << "\nInvalid Choice!\n";
        }
        }

    } while (choice != 0);

    return 0;
}
