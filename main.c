
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "RNN.h"

extern void Q_learn(double *, double *, double, double);
extern double *Q_act(double *);
extern void forward_prop(NNET *, int, double *);

//************************** training data ***********************//
// Each entry of training data consists of a K input value and a desired K
// output value.

#define DATASIZE 100	// number of training / testing examples

double trainingIN[DATASIZE][dim_K];
double trainingOUT[DATASIZE][dim_K];

double testingIN[DATASIZE][dim_K];
double testingOUT[DATASIZE][dim_K];

void read_trainers()
	{
	//open training file
	FILE *fp1, *fp2, *fp3, *fp4;

    if ((fp1 = fopen("training-set-in.txt", "r")) == NULL)
		{ fprintf(stderr, "Cannot open training-set-in.\n"); exit(1); }
    if ((fp2 = fopen("training-set-out.txt", "r")) == NULL)
		{ fprintf(stderr, "Cannot open training-set-out.\n"); exit(1); }
    if ((fp3 = fopen("testing-set-in.txt", "r")) == NULL)
		{ fprintf(stderr, "Cannot open testing-set-in.\n"); exit(1); }
    if ((fp4 = fopen("testing-set-out.txt", "r")) == NULL)
		{ fprintf(stderr, "Cannot open testing-set-out.\n"); exit(1); }

	for (int i = 0; i < DATASIZE; ++i)
		for (int j = 0; j < dim_K; ++j)
			{
			fscanf(fp1, "%lf", &trainingIN[i][j]);
			fscanf(fp2, "%lf", &trainingOUT[i][j]);
			fscanf(fp3, "%lf", &testingIN[i][j]);
			fscanf(fp4, "%lf", &testingOUT[i][j]);
			}

	fclose(fp1); fclose(fp2); fclose(fp3); fclose(fp4);
	}

//**************************main algorithm***********************//
// Main loop:
// 		----- RNN part -----
//		Input is copied into K.
//		Desired output is K*.
//		Do forward propagation (recurrently) a few times.
//		Output is K'.  Error is K'-K*.
//		Use back-prop to reduce this error.
//		----- RL part -----
//		Use Q value to choose an optimal action, taking K' to K''.
//		Invoke Q-learning, using the reward to update Q
// Repeat

NNET *Net;

#define LastLayer (numLayers - 1)

int train()
	{
    Net = (NNET *)malloc(sizeof(NNET));
    int numLayers = 4;
    //the first layer -- input layer
    //the last layer -- output layer
    // int neuronsOfLayer[5] = {2, 3, 4, 4, 4};
    int neuronsOfLayer[4] = {10, 14, 13, 10};
	double *K2;

    //read training data and testing data from file
    read_trainers();

    //create neural network for backpropagation
    create_NN(Net, numLayers, neuronsOfLayer);

    //error array to keep track of errors
    #define MAX_EPOCHS 30
    double error[MAX_EPOCHS];
    int maxlen = 0;
    int epoch = 1;

    SDL_Renderer *gfx = newWindow();		// create graphics window

    //output data to a file
    FILE *fout;
    if ((fout = fopen("randomtest-1.txt", "w")) == NULL)
		{ fprintf(stderr, "file open failed.\n"); exit(1); }

    do		// Loop over all epochs
		{
        // double squareErrorSum = 0;

		// Loop over all training data
		for (int i = 0; i < DATASIZE; ++i)
			{
			// Write input value to K
			for (int k = 0; k < dim_K; ++k)
				K[k] = trainingIN[i][k];

			// Let RNN act on K n times (TO-DO: is this really meaningful?)
			#define RECURRENCE 10
			for (int j = 0; j < RECURRENCE; j++)
				{
				forward_prop(Net, dim_K, K);

				calc_error(Net, trainingOUT[i]);
				back_prop(Net);

				// copy output to input
				for (int k = 0; k < dim_K; ++k)
					K[k] = Net->layers[LastLayer].neurons[k].output;
				}
			}

		// ----- RL part -----

		// Use Q value to choose an optimal action, taking K to K2.
		K2 = Q_act(K);
		// Q_act(K);

		// Invoke Q-learning, using the reward to update Q
		double R = 0.0;	// dummy, TO-DO
		double oldQ = 0.0; // dummy, TO-DO
		Q_learn(K, K2, R, oldQ);

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

        drawNetwork(Net, gfx);
        SDL_Delay(1000 /* milliseconds*/);

		}
    while(maxlen < MAX_EPOCHS);

    fclose(fout);
    free(Net);
    extern NNET *Qnet;
    free(Qnet);

    plot_rectangles(gfx); //keep the window open

    return 0;
	}

// Randomly generate an RNN, watch it operate on K and see how K moves
void test_K_wandering()
	{
    Net = (NNET *) malloc(sizeof(NNET));
    int numLayers = 4;
    int neuronsOfLayer[4] = {10, 14, 13, 10};	// first = input layer, last = output layer

	for (int j = 0; j < 30; j++)
		{
		forward_prop(Net, dim_K, K);

		// copy output to input
		for (int k = 0; k < dim_K; ++k)
			{
			K[k] = Net->layers[LastLayer].neurons[k].output;
			printf("%lf\t", K[k]);
			}
		printf("\n");
		}
	}

//**************************main function***********************//

int main(int argc, char** argv)
	{
	printf("*** Welcome to Genifer 5.3 ***\n\n");

	test_K_wandering();

	return 0;
	}
