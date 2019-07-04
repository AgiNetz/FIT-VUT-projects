/**
 * Filename: pgrep.c
 * Author: Tomas Danis
 */
#include <stdio.h>
#include<unistd.h>
#include<thread>
#include<mutex>
#include<vector>
#include<regex>
#include <iostream>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>

typedef struct {
std::regex *reg;
long int score;
} SRegex; //Scored Regex

char *line;
long int score = 0; //Score of a given iteration
std::mutex *scoreSem; //"score" variable exclusive access mutex
pthread_barrier_t barrier; //Synchronisation barrier for the beginning and the end of an iteration

char *read_line(int *res) {
	std::string line;
	char *str;
	if (std::getline(std::cin, line)) {
		str=(char *) malloc(sizeof(char)*(line.length()+1));
		if(str == NULL) //Osetreni mallocu
		{
            fprintf(stderr,"Could not allocate memory for input string!");
            *res=0;
            return NULL;
		}
		strcpy(str,line.c_str());
		*res=1;
		return str;
	} else {
		*res=0;
		return NULL;
	}

}


void f(SRegex re)
{
    pthread_barrier_wait(&barrier); //Waiting for valid input in line variable
    while(line != NULL)
    {
        if(std::regex_match(line, *(re.reg)))
        {
            //exclusive access
            (*scoreSem).lock();
            score += re.score;
            (*scoreSem).unlock();
        }
        pthread_barrier_wait(&barrier); //Waiting for all threads to finish and evaluate the input before starting another iteration
        pthread_barrier_wait(&barrier); //Waiting for valid input in line in subsequent iterations
    }
}

int main(int argc, char *argv[])
{
    long int minScore = strtol(argv[1], NULL, 10);
	std::vector <std::thread *> threads; //thread array
	std::vector <SRegex> regexes; //scored regex array(one for each thread)
	unsigned tNum = 0; //Number of threads

	/*Regex initialization*/
	for(int i = 2; i < argc-1; i+=2)
	{
        SRegex tmp;
        tmp.reg = new std::regex(argv[i]);
        tmp.score = strtol(argv[i+1], NULL, 10);
        regexes.push_back(tmp);
        tNum++;
	}

	/*Synchronisation utils initializaiton*/
    scoreSem = new std::mutex();
	pthread_barrier_init(&barrier, NULL, tNum+1);

	/* Thread Creation*/
	threads.resize(tNum); //Setting the size of thread array
	for(unsigned i=0;i<tNum;i++)
	{
		std::thread *new_thread = new std::thread (f,regexes[i]); //Starts a thread
		threads[i]=new_thread;
	}

	/*Pgrep calculation*/
	int res;
	line=read_line(&res);
    pthread_barrier_wait(&barrier); //Making sure line has valid content before threads access it
	while (res)
	{
        pthread_barrier_wait(&barrier); //Waiting for all threads to finish
        if(score >= minScore)
            printf("%s\n",line);
        score = 0; //resetting score for next iteration
		free(line); //freeing processed input
		line=read_line(&res);
        pthread_barrier_wait(&barrier); //Making sure threads do not read line until it has been updated
	}


    /*Memory deallocation and wrap-up*/

	/* Waiting for all threads to finish before finish ourselves */
	for(unsigned i=0;i<tNum;i++)
	{
		(*(threads[i])).join();
		delete threads[i];
		delete regexes[i].reg;
	}

    /*Synchronisation utils clean-up*/
	delete scoreSem;
	pthread_barrier_destroy(&barrier);

    return 0;
}
