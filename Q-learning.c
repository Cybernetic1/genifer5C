// Q-learner
// Basically it is a feed-forward neural net that approximates the Q-value function
// used in reinforcement learning.  Call this neural network the Q-net.
// Q-net accepts K and K' as input.  K is the current state of the Reasoner,
// K' is the "next" state.  Q-net's output is the Q-value, ie, the utility at state
// K making the transition to K'.
// In other words, Q-net approximates the function K x K' -> Q.
// Given K, K', and Q, Q-net learns via tradition back-prop.
// The special thing about Q-net is that there is another algorithm that computes,
// when given K, the K' that achieves maximum Q value.  This is the optimization part.

// TO-DO:

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <SDL2/SDL.h>

#include "RNN.h"

#define ETA 0.01			// learning rate

extern double sigmoid(double v);

extern double randomWeight();

extern void createNeuronNetwork(NNET *net, int numberOfLayers, int *neuronsOfLayer);

extern void feedforward(NNET *net, double *K);

extern double calculateError(NNET *net, double *Y);

extern void backpropagation(NNET *net);

//**************************main function***********************//
// Algorithm:
// At some point in the main algorithm, we have a current K value, and a

int Qlearn()
	{
    NNET *Net = (NNET *)malloc(sizeof(NNET));
    int numberOfLayers = 3;
    //the first layer -- input layer
    //the last layer -- output layer
    // int neuronsOfLayer[5] = {2, 3, 4, 4, 4};
    int neuronsOfLayer[3] = {20, 20, 1};

    //create neural network for backpropagation
    createNeuronNetwork(Net, numberOfLayers, neuronsOfLayer);

    int maxlen = 0;
    int epoch = 1;

    // SDL_Renderer *gfx = newWindow();		// create graphics window

    //output data to a file
    FILE *fout;
    if ((fout = fopen("randomtest-1.txt", "w")) == NULL)
		{ fprintf(stderr, "file open failed.\n"); exit(1); }

	// Main algorithm:
	// Input is copied into K.
	// Desired output is K*.
	// Do forward propagation (recurrently) a few times.
	// Output is K'.  Error is K'-K*.
	// Use back-prop to reduce this error.
	// Repeat.
    do		// Loop over all epochs
		{
        // double squareErrorSum = 0;

		// Loop over all training data
		for (int i = 0; i < DATASIZE; ++i)
			{
			// Write input value to K
			for (int k = 0; k < dim_K; ++k)
				K[k] = trainingIN[i][k];

			// Let RNN act on K n times
			for (int j = 0; j < RECURRENCE; j++)
				{
				feedforward(Net, K);

				calculateError(Net, trainingOUT[i]);
				backpropagation(Net);

				// copy output to input
				for (int k = 0; k < dim_K; ++k)
					K[k] = Net->layers[LastLayer].neurons[k].output;
				}
			}

        // error[maxlen] = sqrt(squareErrorSum / DATASIZE);
        //test network
        // printf(		  "%d  \t %lf\t %lf\n", epoch, 0.0f);
        printf("%03d:", epoch);
        for (int i = 0; i < dim_K; ++i)
            printf(" %lf", K[i]);
        printf("\n");
        // fprintf(fout, "%d", epoch);
        maxlen++;
        epoch++;

        // drawNetwork(Net, gfx);
        // SDL_Delay(1000 /* milliseconds*/);

		}
    while(maxlen < MAX_EPOCHS);

    fclose(fout);
    free(Net);

    // plot_rectangles(gfx); //keep the window open

    return 0;
	}
