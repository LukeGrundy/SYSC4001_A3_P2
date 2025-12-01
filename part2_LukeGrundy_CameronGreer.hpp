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

using namespace std;

struct rubric{
    std::string questions[4];
};

struct exam{
    int student_id;
    bool questions_marked[NUM_QUESTIONS];
};

struct shared_data {
    rubric questions[NUM_QUESTIONS];
    exam exams[21];
    int total_questions_graded;
    int total_TAs_working;
    int running = 1;
};

//----------------------------------------------------------
//                  HELPER FUNCTIONS
//----------------------------------------------------------

void random_delay(double min_sec, double max_sec) {
    double t = min_sec + ((double) rand() / RAND_MAX) * (max_sec - min_sec);
    usleep(t * 1000000);
}

// Following function was taken from part 1 taken from stackoverflow; helper function for splitting strings
std::vector<std::string> split_delim(std::string input, std::string delim) {
    std::vector<std::string> tokens;
    std::size_t pos = 0;
    std::string token;
    while ((pos = input.find(delim)) != std::string::npos) {
        token = input.substr(0, pos);
        tokens.push_back(token);
        input.erase(0, pos + delim.length());
    }
    tokens.push_back(input);

    return tokens;
}

void load_rubric(vector<string> input_tokens, shared_data *shm) {
    ifstream file("rubric.txt");
    if (!file.is_open()) {
        cerr << "Error: Unable to open rubric file." << endl;
        return;
    }

    for (size_t i = 0; i < NUM_QUESTIONS; i++) {

        shm->questions[i].questions[i] = input_tokens[i];
    }
}

#endif