#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <mysql/mysql.h> // MySQL connector header

using namespace std;

// Abstract Base Class for Persons (Abstraction)
class Person {
protected:
    string name;
    int age;
public:
    Person(const string& n, int a) : name(n), age(a) {}
    virtual ~Person() {}

    string getName() const { return name; }
    int getAge() const { return age; }

    virtual void display() const = 0; // Pure virtual function (Abstract Method)
};

// Derived Class for Patients
class Patient : public Person {
private:
    string medicalRecord;
    int patientID;
public:
    Patient(const string& n, int a, const string& record, int id)
        : Person(n, a), medicalRecord(record), patientID(id) {}

    void setMedicalRecord(const string& record) { medicalRecord = record; }
    string getMedicalRecord() const { return medicalRecord; }
    int getPatientID() const { return patientID; }

    void display() const override {
        cout << "Patient: " << name << ", Age: " << age << ", Record: " << medicalRecord << endl;
    }
};

// Derived Class for Doctors
class Doctor : public Person {
private:
    string specialty;
    int doctorID;
public:
    Doctor(const string& n, int a, const string& spec, int id)
        : Person(n, a), specialty(spec), doctorID(id) {}

    void setSpecialty(const string& spec) { specialty = spec; }
    string getSpecialty() const { return specialty; }
    int getDoctorID() const { return doctorID; }

    void display() const override {
        cout << "Doctor: " << name << ", Age: " << age << ", Specialty: " << specialty << endl;
    }
};

// Class for MySQL Database Connection
class Database {
private:
    MYSQL* conn;
public:
    Database() {
        conn = mysql_init(0);
        conn = mysql_real_connect(conn, "localhost", "root", "password", "hospital_db", 3306, NULL, 0);
        if (!conn) {
            throw runtime_error("Failed to connect to the database.");
        }
    }

    ~Database() {
        if (conn) {
            mysql_close(conn);
        }
    }

    void executeQuery(const string& query) {
        if (mysql_query(conn, query.c_str())) {
            throw runtime_error("MySQL Query Error: " + string(mysql_error(conn)));
        }
    }

    MYSQL_RES* fetchResults(const string& query) {
        if (mysql_query(conn, query.c_str())) {
            throw runtime_error("MySQL Query Error: " + string(mysql_error(conn)));
        }
        return mysql_store_result(conn);
    }
};

// Class for Hospital (Composition)
class Hospital {
private:
    vector<Patient> patients;
    vector<Doctor> doctors;
    Database db;

public:
    void addPatient(const Patient& patient) {
        patients.push_back(patient);
        string query = "INSERT INTO patients (name, age, medical_record) VALUES ('" + patient.getName() + "', " + to_string(patient.getAge()) + ", '" + patient.getMedicalRecord() + "')";
        db.executeQuery(query);
    }

    void addDoctor(const Doctor& doctor) {
        doctors.push_back(doctor);
        string query = "INSERT INTO doctors (name, age, specialty) VALUES ('" + doctor.getName() + "', " + to_string(doctor.getAge()) + ", '" + doctor.getSpecialty() + "')";
        db.executeQuery(query);
    }

    void displayPatients() const {
        cout << "Patients List:" << endl;
        for (const auto& patient : patients) {
            patient.display();
        }
    }

    void displayDoctors() const {
        cout << "Doctors List:" << endl;
        for (const auto& doctor : doctors) {
            doctor.display();
        }
    }

    void displayAllPersons() const {
        cout << "All Persons in Hospital:" << endl;
        for (const auto& patient : patients) {
            patient.display();
        }
        for (const auto& doctor : doctors) {
            doctor.display();
        }
    }
};

int main() {
    Hospital hospital;

    // Menu options
    int choice;
    do {
        cout << "\nHospital Management System Menu:\n";
        cout << "1. Add Patient\n";
        cout << "2. Add Doctor\n";
        cout << "3. Display Patients\n";
        cout << "4. Display Doctors\n";
        cout << "5. Display All Persons\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Ignore newline character left in the buffer

        switch (choice) {
            case 1: {
                string name, medicalRecord;
                int age, id;
                cout << "Enter patient name: ";
                getline(cin, name);
                cout << "Enter patient age: ";
                cin >> age;
                cin.ignore(); // Ignore newline character
                cout << "Enter medical record: ";
                getline(cin, medicalRecord);
                cout << "Enter patient ID: ";
                cin >> id;
                cin.ignore(); // Ignore newline character

                Patient patient(name, age, medicalRecord, id);
                hospital.addPatient(patient);
                break;
            }
            case 2: {
                string name, specialty;
                int age, id;
                cout << "Enter doctor name: ";
                getline(cin, name);
                cout << "Enter doctor age: ";
                cin >> age;
                cin.ignore(); // Ignore newline character
                cout << "Enter specialty: ";
                getline(cin, specialty);
                cout << "Enter doctor ID: ";
                cin >> id;
                cin.ignore(); // Ignore newline character

                Doctor doctor(name, age, specialty, id);
                hospital.addDoctor(doctor);
                break;
            }
            case 3:
                hospital.displayPatients();
                break;
            case 4:
                hospital.displayDoctors();
                break;
            case 5:
                hospital.displayAllPersons();
                break;
            case 6:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    } while (choice != 6);

    return 0;
}

