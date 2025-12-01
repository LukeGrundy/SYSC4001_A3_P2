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

struct rubric{
    std::string questions[4];
};

struct exam{
    int student_id;
    bool questions_marked[4];
};

struct shared_data {
    rubric questions[4];
    exam exams[21];
    int total_questions_graded;
    int total_TAs_working;
};

//----------------------------------------------------------
//                  HELPER FUNCTIONS
//----------------------------------------------------------

void random_delay(double min_sec, double max_sec) {
    double t = min_sec + ((double) rand() / RAND_MAX) * (max_sec - min_sec);
    usleep(t * 1000000);
}


#endif