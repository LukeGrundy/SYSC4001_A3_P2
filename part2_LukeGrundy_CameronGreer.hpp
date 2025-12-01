/**
 *
 * @file part2_LukeGrundy_CameronGreer.hpp
 * @author Cameron Greer
 * @author Luke Grundy
 *
 * @date  December 30th, 2025
 */

#ifndef PART2_LukeGrundy_CameronGreer_HPP_
#define PART2_LukeGrundy_CameronGreer_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sys/sem.h>
#include <sys/ipc.h>

#define NUM_QUESTIONS 4
#define NUM_EXAMS 21

#define SEM_RUBRIC 0      // For protecting access to the rubric
#define SEM_EXAM_LOAD 1   // For protecting the loading of exams
#define SEM_MARK_ASSIGN 2 // For protecting assigning marks

using namespace std;

struct rubric
{
    std::string questions[NUM_QUESTIONS];
};

struct exam
{
    int student_id;
    bool questions_marked[NUM_QUESTIONS];
};

struct shared_data
{
    // RUBRIC AND RUBRIC METRICS
    rubric rb;                    // The rubric for the exam
    int total_rubric_corrections; // Number of rubric revisions made

    // EXAM AND EXAM METRICS
    exam current_exam;          // The current exam being marked
    int total_questions_graded; // Number of questions graded so far
    int total_exams_marked;     // Number of exams fully marked

    // TA METRICS
    int marked_exams[NUM_EXAMS]; // Student IDs of marked exams
    int total_TAs_working;       // Number of TAs currently working
    int running = 1;             // Flag to indicate if TAs should keep working
};

//----------------------------------------------------------
//                  HELPER FUNCTIONS
//----------------------------------------------------------

void random_delay(double min_sec, double max_sec)
{
    double t = min_sec + ((double)rand() / RAND_MAX) * (max_sec - min_sec);
    usleep(t * 1000000);
}

// Following function was taken from part 1 taken from stackoverflow; helper function for splitting strings
vector<string> split_delim(string input, string delim)
{
    vector<std::string> tokens;
    size_t pos = 0;
    string token;
    while ((pos = input.find(delim)) != string::npos)
    {
        token = input.substr(0, pos);
        tokens.push_back(token);
        input.erase(0, pos + delim.length());
    }
    tokens.push_back(input);

    return tokens;
}

// Writes a string to a file (also from part 1)
void write_output(std::string execution, const char *filename)
{
    std::ofstream output_file(filename);

    if (output_file.is_open())
    {
        output_file << execution;
        output_file.close(); // Close the file when done
        std::cout << "File content overwritten successfully." << std::endl;
    }
    else
    {
        std::cerr << "Error opening file!" << std::endl;
    }

    std::cout << "Output generated in " << filename << ".txt" << std::endl;
}

void correct_rubric_question(rubric &rubric, int question_index)
{
    // Simulate rubric correction by adding 1 to the curent ascii value of the question string
    rubric.questions[question_index] += 1;
}

void load_next_exam(shared_data *shm, int TA_id)
{
    DIR *d = opendir("./exams");
    struct dirent *dir;
    dir = readdir(d); // Read first entry
    // Iterate to the exam that needs to be loaded
    if (dir->d_name[0] == '.')
    {
        for (int i = 0; i < shm->total_exams_marked; i++)
        {
            dir = readdir(d); // Skip already marked exams
        }
        // Load the exam file
        string file_path = string("./exams/") + string(dir->d_name);
        ifstream exam_file(file_path);
        if (exam_file.is_open())
        {
            string line;
            getline(exam_file, line);
            shm->current_exam.student_id = stoi(line); // Get student ID
            for (int i = 0; i < NUM_QUESTIONS; i++)
            {
                shm->current_exam.questions_marked[i] = false; // Initialize all questions as unmarked
            }
            exam_file.close();
            cout << "TA " << TA_id << " loaded exam for student " << shm->current_exam.student_id << endl;
            closedir(d);
            return; // Exit after loading one exam
        }
    }
}

int sem_wait(int semid, int semnum)
{
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = -1;
    op.sem_flg = 0;
    return semop(semid, &op, 1);
}

int sem_signal(int semid, int semnum)
{
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = 1;
    op.sem_flg = 0;
    return semop(semid, &op, 1);
}

int sem_trywait(int semid, int semnum)
{
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = -1;
    op.sem_flg = IPC_NOWAIT;
    if (semop(semid, &op, 1) == 0)
    {
        return 0;
    }
    return -1;
}

int sem_set(int semid, int semnum, int val)
{
    return semctl(semid, semnum, SETVAL, val);
}

void cleanup(int shmid, int semid, shared_data *shm_ptr)
{
    if (shm_ptr != NULL)
    {
        shmdt(shm_ptr);
    }
    if (shmid >= 0)
    {
        shmctl(shmid, IPC_RMID, NULL);
    }
    if (semid >= 0)
    {
        semctl(semid, 0, IPC_RMID);
    }
}

#endif
