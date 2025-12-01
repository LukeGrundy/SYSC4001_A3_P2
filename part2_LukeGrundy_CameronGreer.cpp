/**
 * 
 * @file part2_LukeGrundy_CameronGreer.cpp
 * @author Cameron Greer
 * @author Luke Grundy
 * 
 * @date  December 30th, 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <iostream>

#include "part2_LukeGrundy_CameronGreer.hpp"

int main(int argc, char** argv) {//code originally from part 1

    //Get the input file from the user
    if(argc != 3) {
        std::cout << "ERROR!\nExpected 2 arguments, received " << argc - 1 << std::endl;
        std::cout << "To run the program, do: ./part2 <your_rubric.txt> <number_of_TAs>" << std::endl;
        return -1;
    }

    int number_of_TAs = std::stoi(argv[2]);

    //Open the input file
    auto file_name = argv[1];
    std::ifstream input_file;
    input_file.open(file_name);

    //Ensure that the file actually opens
    if (!input_file.is_open()) {
        std::cerr << "Error: Unable to open file: " << file_name << std::endl;
        return -1;
    }

    //Parse the entire input file and populate a rubric vector.
    //To do so, the add_process() helper function is used (see include file).
    std::string line;
    std::vector<rubric> list_questions;
    while(std::getline(input_file, line)) {
        auto input_tokens = split_delim(line, ", ");
        auto new_process = add_process(input_tokens);
        list_process.push_back(new_process);
    }
    input_file.close();

    //With the list of processes, run the simulation
    auto [exec] = run_simulation(list_process);

    write_output(exec, "execution.txt");

    return 0;
}