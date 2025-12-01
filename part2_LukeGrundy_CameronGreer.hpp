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

#define NUM_QUESTIONS 5
#define NUM_EXAMS 21

using namespace std;

struct rubric{
    std::string questions[NUM_QUESTIONS - 1];
};

struct exam{
    int student_id;
    bool questions_marked[NUM_QUESTIONS - 1];
};

struct shared_data {
    // RUBRIC AND RUBRIC METRICS
    rubric rb;                       // The rubric for the exam
    int total_rubric_corrections;    // Number of rubric revisions made

    // EXAM AND EXAM METRICS
    exam current_exam;               // The current exam being marked
    int total_questions_graded;      // Number of questions graded so far
    int total_exams_marked;          // Number of exams fully marked

    // TA METRICS
    int marked_exams[NUM_EXAMS];     // Student IDs of marked exams
    int total_TAs_working;           // Number of TAs currently working
    int running = 1;                 // Flag to indicate if TAs should keep working
};


//----------------------------------------------------------
//                  HELPER FUNCTIONS
//----------------------------------------------------------

void random_delay(double min_sec, double max_sec) {
    double t = min_sec + ((double) rand() / RAND_MAX) * (max_sec - min_sec);
    usleep(t * 1000000);
}

// Following function was taken from part 1 taken from stackoverflow; helper function for splitting strings
vector<string> split_delim(string input, string delim) {
    vector<std::string> tokens;
    size_t pos = 0;
    string token;
    while ((pos = input.find(delim)) != string::npos) {
        token = input.substr(0, pos);
        tokens.push_back(token);
        input.erase(0, pos + delim.length());
    }
    tokens.push_back(input);

    return tokens;
}

//Writes a string to a file (also from part 1)
void write_output(std::string execution, const char* filename) {
    std::ofstream output_file(filename);

    if (output_file.is_open()) {
        output_file << execution;
        output_file.close();  // Close the file when done
        std::cout << "File content overwritten successfully." << std::endl;
    } else {
        std::cerr << "Error opening file!" << std::endl;
    }

    std::cout << "Output generated in " << filename << ".txt" << std::endl;
}

void correct_rubric_question(rubric &rubric, int question_index) {
    // Simulate rubric correction by adding 1 to the curent ascii value of the question string
    rubric.questions[question_index] += 1;
}

#endif