📘 Timetable Management System (C++ | Data Structures & Algorithms)

A DSA-based automated timetable generator with graph-based conflict detection

🚀 Project Overview

The Timetable Management System is a complete C++-based academic scheduling solution designed using core Data Structures & Algorithms.
It automates timetable generation, detects scheduling conflicts using graphs, and stores all outputs through file handling.

The project features:

Automatic timetable generation (6 × 8 weekly grid)

Graph-based conflict detection (faculty/room/batch clashes)

Faculty, subject & room management

File-based storage (timetable + conflict report)

Fast availability checking using hash tables

Clean modular architecture

This project is fully implemented for academic DSA coursework and serves as a real demonstration of graphs, hash maps, sets, vectors, and file-management in C++.

🛠️ Tech Stack

Language: C++

Paradigm: Object-Oriented + DSA

Data Structures Used:

2D array (timetable grid)

vector (dynamic lists)

map / unordered_map (faculty/subject lookup)

set / unordered_set (availability tracking)

Graph - adjacency list (conflict detection)

struct (slot representation)

File Handling: Standard C++ I/O

📂 Project Structure
/Timetable-Management-System
│
├── src/
│   ├── main.cpp
│   ├── faculty_module.cpp
│   ├── subject_module.cpp
│   ├── timetable.cpp
│   ├── graph_conflict.cpp
│   └── file_manager.cpp
│
├── include/
│   ├── faculty.h
│   ├── subject.h
│   ├── timetable.h
│   ├── graph_conflict.h
│   └── file_manager.h
│
├── data/
│   ├── faculty.txt
│   ├── subjects.txt
│   ├── timetable_output.txt
│   └── conflict_report.txt
│
└── README.md

⚡ Key Features
✔ 1. Automated Timetable Generation

Fills a fixed 6×8 weekly grid using constraints:

faculty availability

room availability

batch requirements

✔ 2. Graph-Based Conflict Detection

Each scheduled class is treated as a node.
Edges are formed when two nodes share:

faculty

room

batch

Any edge = conflict.

✔ 3. Fast Availability Checking

Using:

unordered_set

map

to ensure O(1) lookups for:

faculty free/not free

rooms free/not free

batch free/not free

✔ 4. Clean Modular Architecture

Separated into:

Faculty module

Subject module

Timetable grid

Conflict detection engine

File manager

✔ 5. File Outputs

Automatically generates:

timetable_output.txt

conflict_report.txt

📊 Architecture Diagram (Text-Based)
+---------------------------+
|  Input Layer (users, data)|
+------------+--------------+
             |
             v
+---------------------------+
|  Data Storage Layer       |
|  (vectors, maps, sets)    |
+------------+--------------+
             |
             v
+---------------------------+
|  Timetable Grid (2D array)|
+------------+--------------+
             |
             v
+---------------------------+
|  Conflict Detector (Graph)|
|  Adjacency List + Rules   |
+------------+--------------+
             |
             v
+---------------------------+
|  File Manager (I/O)       |
+---------------------------+

🧪 Testing Summary

✔ Functional Testing — Passed

✔ Conflict Detection — Passed

✔ Stress Testing (large faculty/subject lists) — Passed

✔ File generation validation — Passed

📈 Future Enhancements

GUI Interface (Qt/HTML)

Cloud deployment

Automatic room optimization

Multi-user login system

📥 How to Run
1. Clone the repository
git clone https://github.com/atharvtiwari26/Timetable-Management-System.git
cd Timetable-Management-System

2. Compile
g++ src/*.cpp -o timetable

3. Run
./timetable

👨‍💻 Team Members
Name	Role
Atharv Tiwari	Lead Developer, Architecture, Scheduling
Imamuddin	Conflict Detection, File Outputs
Mayank Joshi	Authentication, Data Modules
📝 License

This project is created for academic purposes (DSA Coursework).
Feel free to use or modify with credit to the contributors.
