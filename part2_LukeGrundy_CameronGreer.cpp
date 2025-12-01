/**
 *
 * @file part2_LukeGrundy_CameronGreer.cpp
 * @author Cameron Greer
 * @author Luke Grundy
 *
 * @date  December 30th, 2025
 */

#include "part2_LukeGrundy_CameronGreer.hpp"

void TA_process(int id, shared_data *shm)
{
    while (shm->running)
    {
        // Iterate through rubric
        //   Decide randomly if rubric needs correcting
        //   If so, correct it (0.5 to 1.0 seconds)
        //   Update shared memory to reflect correction
        for (int i = 0; i < NUM_QUESTIONS; i++)
        {
            bool needs_correction = (rand() % 2) == 0; // 50% chance
            if (needs_correction)
            {
                cout << "TA " << id << " is correcting rubric question " << i << endl;
                random_delay(0.5, 1.0); // Simulate time taken to correct
                correct_rubric_question(shm->rb, i);
                shm->total_rubric_corrections++;
                cout << "TA " << id << " finished correcting rubric question " << i << endl;
            }
        }

        // Begin marking exam questions
        //  TA choses next unmarked question from shared memory
        //   Mark it (1.0 to 2.0 seconds)
        //   Update shared memory to reflect marking}
        while (shm->total_questions_graded < NUM_QUESTIONS)
        {
            int question_to_mark = -1; // Index of question TA is marking, -1 indicates no question assigned yet

            // Critical section start
            if (shm->total_questions_graded < NUM_QUESTIONS)
            {
                cout << "TA " << id << " is selecting next question to mark." << endl;
                // Assign next question to mark to TA by finding the next unmarked question
                for (int q = 0; q < NUM_QUESTIONS - 1; q++)
                {
                    if (!shm->current_exam.questions_marked[q])
                    {
                        question_to_mark = q;
                        shm->current_exam.questions_marked[q] = true; // Mark as assigned
                        cout << "TA " << id << " assigned question " << question_to_mark + 1 << " to mark." << endl;
                        break;
                    }
                }
                question_to_mark = shm->total_questions_graded;
                shm->total_questions_graded++;
            }
            // Critical section end

            if (question_to_mark != -1)
            {
                cout << "TA " << id << " is marking question " << question_to_mark << endl;
                random_delay(1.0, 2.0); // Simulate time taken to mark
                cout << "TA " << id << " finished marking question " << question_to_mark << endl;
            }

            // If all questions are marked, update exams marked count and add student ID to marked exams
            // Reset total_questions_graded for next exam
            // Load next exam into shared memory
            if (shm->total_questions_graded >= NUM_QUESTIONS)
            {
                shm->total_exams_marked++;
                shm->marked_exams[shm->total_exams_marked - 1] = shm->current_exam.student_id;
                cout << "TA " << id << " has fully marked exam for student " << shm->current_exam.student_id << endl;
                load_next_exam(shm, id); // Load next exam
            }
        }
    }
}

int sem_wait(int smeid, int semnum){
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = -1;
    op.sem_flg = 0;
    return semop(semid, &op, 1);
}

int sem_signal(int semid, int semnum){
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = 1;
    op.sem_flg = 0;
    return semop(semid, &op, 1);
}

int sem_trywait(int semid, int semnum){
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = -1;
    op.sem_flg = IPC_NOWAIT;
    if (semop(semid, &op, 1) == 0){
	return 0;
    }
    return -1;
}

int sem_set(int semid, int semnum, int val){
    union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
    } arg;
    arg.val = val;
    return semctl(semid, semnum, SETVAL, arg);
}

int main(int argc, char **argv)
{ // code originally from part 1

    // Get the input files from the user
    if (argc != 4)
    {
        cout << "ERROR!\nExpected 2 arguments, received " << argc - 1 << endl;
        cout << "To run the program, do: ./part2 <your_rubric.txt> <./your_exam_directory> <number_of_TAs>" << endl;
        return -1;
    }

    int number_of_TAs = atoi(argv[3]); // Convert the 2nd argument to an integer representing the number of TAs

    int shm_id = shmget(IPC_PRIVATE, sizeof(shared_data), IPC_CREAT | 0666); // Shared memory ID
    shared_data *shm = (shared_data *)shmat(shm_id, nullptr, 0);             // Shared memory pointer

    shm->total_questions_graded = 0;
    shm->total_TAs_working = 0;

    // Open the input file
    auto file_name = argv[1];
    ifstream input_file;
    input_file.open(file_name);

    // Ensure that the file actually opens
    if (!input_file.is_open())
    {
        cerr << "Error: Unable to open file: " << file_name << endl;
        return -1;
    }

    // Parse the input file to populate a rubric vector.
    // To do so, the add_process() helper function is used (see include file).
    string line;
    vector<rubric> list_questions;
    while (getline(input_file, line))
    {
        int i = 0;
        auto input_tokens = split_delim(line, ", ");
        shm->rb.questions[i] = input_tokens[1];
        i++;
    }
    input_file.close();

    // Open the exams directory
    DIR *d = opendir(argv[2]);
    if (!d){
        cerr << "Error: Unable to open exams directory: " << argv[2] << endl;
        return -1;
    }

    // With the list of processes, run the simulation
    // auto [exec] = run_simulation(list_process);

    // write_output(exec, "execution.txt");

    /* --- Generate TA processes --- */
    for (int i = 0; i < number_of_TAs; i++)
    {
        if (fork() == 0)
        {
            TA_process(i, shm);
        }
    }

    return 0;
}
