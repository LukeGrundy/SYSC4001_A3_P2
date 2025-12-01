/**
 *
 * @file part2_LukeGrundy_CameronGreer.cpp
 * @author Cameron Greer
 * @author Luke Grundy
 *
 * @date  December 30th, 2025
 */

#include "part2_LukeGrundy_CameronGreer.hpp"

void TA_process(int id, shared_data *shm, int sem_id)
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
                if (sem_wait(sem_id, SEM_RUBRIC) != 0)
                {
                    std::cout << "Error: sem_wait rubric" << std::endl;
                    continue;
                }
                cout << "TA " << id << " is correcting rubric question " << i + 1 << endl;
                random_delay(0.5, 1.0); // Simulate time taken to correct
                correct_rubric_question(shm->rb, i);
                shm->total_rubric_corrections++;
                cout << "TA " << id << " finished correcting rubric question " << i + 1 << endl;
                if (sem_signal(sem_id, SEM_RUBRIC) != 0)
                {
                    std::cout << "Error: sem_signal rubric" << std::endl;
                }
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
            if (sem_wait(sem_id, SEM_MARK_ASSIGN) != 0)
            {
                std::cout << "Error: sem_wait mark assign" << std::endl;
                continue;
            }
            if (shm->total_questions_graded < NUM_QUESTIONS)
            {
                cout << "TA " << id << " is selecting next question to mark." << endl;
                // Assign next question to mark to TA by finding the next unmarked question
                for (int q = 0; q < NUM_QUESTIONS; q++)
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
            if (sem_signal(sem_id, SEM_MARK_ASSIGN) != 0)
            {
                std::cout << "Error: sem_signal mark assign" << std::endl;
            }
            // Critical section end

            if (question_to_mark != -1)
            {
                cout << "TA " << id << " is marking question " << question_to_mark + 1<< endl;
                random_delay(1.0, 2.0); // Simulate time taken to mark
                cout << "TA " << id << " finished marking question " << question_to_mark + 1 << endl;
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
                break;
            }
        }
    }
}

int main(int argc, char **argv)
{ // code originally from part 1

    // Get the input files from the user
    if (argc != 4)
    {
        cout << "ERROR!\nExpected 3 arguments, received " << argc - 1 << endl;
        cout << "To run the program, do: ./part2 <your_rubric.txt> <./your_exam_directory> <number_of_TAs>" << endl;
        return -1;
    }

    int number_of_TAs = atoi(argv[3]); // Convert the 2nd argument to an integer representing the number of TAs

    ///// Create Shared Memory //////
    int shm_id = shmget(IPC_PRIVATE, sizeof(shared_data), IPC_CREAT | 0666); // Shared memory ID
    if (shm_id < 0)
    {
        std::cout << "Error: shmget" << std::endl;
        return -1;
    }
    shared_data *shm = (shared_data *)shmat(shm_id, nullptr, 0); // Shared memory pointer

    shm->total_questions_graded = 0;
    shm->total_TAs_working = 0;
    shm->running = 1;
    shm->total_exams_marked = 0;
    shm->total_rubric_corrections = 0;

    for (int i = 0; i < NUM_QUESTIONS; i++)
    {
        shm->current_exam.questions_marked[i] = false;
    }
    ////////////////////////////////

    ///// Load Rubric FIle /////
    ifstream input_file(argv[1]);
    if (!input_file.is_open())
    {
        std::cout << "Error: unable to open rubric file" << std::endl;
        return -1;
    }

    string line;
    int question_index = 0;

    while (getline(input_file, line))
    {
        if (question_index >= NUM_QUESTIONS)
        {
            break;
        }
        auto tokens = split_delim(line, ", ");
        if (tokens.size() < 2)
        {
            std::cout << "Invalid rubric formatting" << std::endl;
            continue;
        }
        // strcpy(shm->rb.questions[question_index], tokens[1].c_str(), 255);
        question_index++;
    }
    input_file.close();
    ////////////////////////////

    ///// Load Exam FIle //////
    DIR *d = opendir(argv[2]);
    if (!d)
    {
        std::cout << "Error: unable to open exam directory" << std::endl;
        return -1;
    }
    struct dirent *dir;
    bool exam_loaded = false;
    while ((dir = readdir(d)) != nullptr)
    {
        if (dir->d_name[0] == '.')
        {
            continue;
        }
        string file_path = string(argv[2]) + "/" + dir->d_name;
        ifstream exam_file(file_path);

        if (!exam_file.is_open())
        {
            continue;
        }

        getline(exam_file, line);
        shm->current_exam.student_id = stoi(line);

        for (int i = 0; i < NUM_QUESTIONS; i++)
        {
            shm->current_exam.questions_marked[i] = false;
        }

        exam_loaded = true;
        exam_file.close();
        break;
    }
    closedir(d);

    if (!exam_loaded)
    {
        std::cout << "Error: no exam files found" << std::endl;
        return -1;
    }
    ///////////////////////////

    ///// Create Semaphores /////
    int sem_id = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);
    if (sem_id < 0)
    {
        std::cout << "Error: semget" << std::endl;
        cleanup(sem_id, -1, shm);
        return 1;
    }

    if (sem_set(sem_id, SEM_RUBRIC, 1) < 0 || sem_set(sem_id, SEM_EXAM_LOAD, 1) < 0 ||
        sem_set(sem_id, SEM_MARK_ASSIGN, 1) < 0)
    {
        std::cout << "Error: sem_set" << std::endl;
        cleanup(sem_id, -1, shm);
        return 1;
    }
    /////////////////////////////

    ///// Fork TA's /////
    for (int i = 1; i < number_of_TAs + 1; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            TA_process(i, shm, sem_id);
            exit(0);
        }
        else if (pid < 0)
        {
            std::cout << "Error: fork" << std::endl;
        }
        ////////////////////

        // make sure processes wait for their children to be terminated
        for (int i = 0; i < number_of_TAs; i++)
        {
            wait(NULL);
        }

        cleanup(shm_id, sem_id, shm);
        return 0;
    }
}