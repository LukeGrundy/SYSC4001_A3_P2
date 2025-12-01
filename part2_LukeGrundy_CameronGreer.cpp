/**
 * 
 * @file part2_LukeGrundy_CameronGreer.cpp
 * @author Cameron Greer
 * @author Luke Grundy
 * 
 * @date  December 30th, 2025
 */


#include "part2_LukeGrundy_CameronGreer.hpp"

void TA_process(int id, shared_data *shm) {
    //Iterate through rubric
    //  Decide randomly if rubric needs correcting
    //  If so, correct it (0.5 to 1.0 seconds)
    //  Update shared memory to reflect correction
    for (int i = 0; i < NUM_QUESTIONS; i++) {
        bool needs_correction = (rand() % 2) == 0; //50% chance
        if (needs_correction) {
            cout << "TA " << id << " is correcting rubric question " << i << endl;
            random_delay(0.5, 1.0); //Simulate time taken to correct
            shm->total_questions_graded++;
            cout << "TA " << id << " finished correcting rubric question " << i << endl;
        }
    }
    
    //Begin marking exam questions
    // TA choses next unmarked question from shared memory
    //  Mark it (1.0 to 2.0 seconds)
    //  Update shared memory to reflect marking}
    while (shm->total_questions_graded < NUM_QUESTIONS) {
        int question_to_mark = -1;

        //Critical section start
        if (shm->total_questions_graded < NUM_QUESTIONS) {
            question_to_mark = shm->total_questions_graded;
            shm->total_questions_graded++;
        }
        //Critical section end

        if (question_to_mark != -1) {
            cout << "TA " << id << " is marking question " << question_to_mark << endl;
            random_delay(1.0, 2.0); //Simulate time taken to mark
            cout << "TA " << id << " finished marking question " << question_to_mark << endl;
        }
    }


int main(int argc, char** argv) {//code originally from part 1

    //Get the input file from the user
    if(argc != 3) {
        cout << "ERROR!\nExpected 2 arguments, received " << argc - 1 << endl;
        cout << "To run the program, do: ./part2 <your_rubric.txt> <number_of_TAs>" << endl;
        return -1;
    }

    int number_of_TAs = std::atoi(argv[2]); //Convert the 2nd argument to an integer representing the number of TAs

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
    while(getline(input_file, line)) {
        auto input_tokens = split_delim(line, ", ");
        auto new_process = add_process(input_tokens);
        list_process.push_back(new_process);
    }
    input_file.close();

    //With the list of processes, run the simulation
    auto [exec] = run_simulation(list_process);

    write_output(exec, "execution.txt");

    /* --- Generate TA processes --- */
    for (int i = 0; i < number_of_TAs; i++) {
        if (fork() == 0) {
            TA_process(i, shm);
        }
    }

    return 0;
}