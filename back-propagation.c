// TO-DO:
// * hook up with RL -- needs ANN for Q
// * the calculation of "error" in back-prop is unclear

// TO-DO: (Cantonese task)
// * need vector representation for individual words

// TODO (yky#1#): There is a convergence problem -- it almost always converges and really quickly

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <SDL2/SDL.h>

#include "RNN.h"

// #define DATASIZE 800		// size of training data and test data
#define ETA 0.01			// learning rate
#define BIASOUTPUT 1		// output for bias. It's always 1.
#define MAX_EPOCHS 20		// maximum epochs
#define RECURRENCE 10		// RNN acts on K n times

//********sigmoid function and randomWeight generator********************//

double sigmoid(double v)
    return 1 / (1 + exp(-v));

double randomWeight()	//random weight generator between -0.5 ~ 0.5
    return ((int)rand() % 100000) / (float) 100000 - 0.5;

//****************************create neuron network*********************//

void createNeuronNetwork(NNET *net, int numberOfLayers, int *neuronsOfLayer)
	{
    //in order to create a neural network,
    //we need know how many layers and how many neurons in each layer

    int i, j, k;
    srand(time(NULL));
    net->numberOfLayers = numberOfLayers;

    assert(numberOfLayers >= 3);

    net->layers = (LAYER *)malloc(numberOfLayers * sizeof(LAYER));
    //construct input layer, no weights
    net->layers[0].numberOfNeurons = neuronsOfLayer[0];
    net->layers[0].neurons = (NEURON *) malloc(neuronsOfLayer[0] * sizeof(NEURON));

    //construct hidden layers
    for(i = 1; i < numberOfLayers; i++)		//construct layers
		{
        net->layers[i].neurons = (NEURON *) malloc(neuronsOfLayer[i] * sizeof(NEURON));
        net->layers[i].numberOfNeurons = neuronsOfLayer[i];
        for(j = 0; j < neuronsOfLayer[i]; j++)	// construct each neuron in the layer
			{
            net->layers[i].neurons[j].weights = (double *)malloc((neuronsOfLayer[i - 1] + 1) * sizeof(double));
            for(k = 0; k <= neuronsOfLayer[i - 1]; k++)
				{
                //construct weights of neuron from previous layer neurons
                net->layers[i].neurons[j].weights[k] = randomWeight(); //when k = 0, it's bias weight
                //net->layers[i].neurons[j].weights[k] = 0;
				}
			}
		}
	}

//****************************feed forward***************************//

void feedforward(NNET *net, double *K)
	{
    //set the output of input layer
    //two inputs x1 and x2
    for (int i = 0; i < 4; ++i)
        net->layers[0].neurons[i].output = K[i];

    for (int i = 1; i < net->numberOfLayers; i++)	//calculate output from hidden layers to output layer
		{
        for (int j = 0; j < net->layers[i].numberOfNeurons; j++)
			{
            double v = 0;	//induced local field for neurons
            //calculate v, which is the sum of the product of input and weights
            for (int k = 0; k <= net->layers[i - 1].numberOfNeurons; k++)
				{
                if (k == 0)
                    v += net->layers[i].neurons[j].weights[k] * BIASOUTPUT;
                else
                    v += net->layers[i].neurons[j].weights[k] * net->layers[i - 1].neurons[k - 1].output;
				}

            //double output = sigmoid(v);
            //net->layers[i].neurons[j].output = output;
            net->layers[i].neurons[j].output = sigmoid(v);
            //printf("%lf\n", output);
			}
		}
	}

#define LastLayer (numberOfLayers - 1)

double calculateError(NNET *net, double *Y)
	{
    // calculate mean square error;
    // desired value = K* = trainingOUT
    double sumOfSquareError = 0;

    int numberOfLayers = net->numberOfLayers;
    // This means each output neuron corresponds to a classification label --YKY
    for(int i = 0; i < net->layers[LastLayer].numberOfNeurons; i++)
    	{
    	//error = desired_value - output
    	double error = Y[i] - net->layers[LastLayer].neurons[i].output;
    	net->layers[LastLayer].neurons[i].error = error;
    	sumOfSquareError += error * error / 2;
    	}
    double mse = sumOfSquareError / net->layers[LastLayer].numberOfNeurons;
    return mse;	 //return the root of mean square error
	}


//**************************backpropagation***********************//

#define LastLayer (numberOfLayers - 1)

void backpropagation(NNET *net)
	{
    //calculate delta
    int i, j, k;
    int numberOfLayers = net->numberOfLayers;

    //calculate delta for output layer
    for (i = 0; i < net->layers[LastLayer].numberOfNeurons; i++)
		{
        double output = net->layers[LastLayer].neurons[i].output;
        double error = net->layers[LastLayer].neurons[i].error;
        //for output layer, delta = y(1-y)error
        net->layers[LastLayer].neurons[i].delta = output * (1 - output) * error;
		}

    //calculate delta for hidden layers
    for (i = numberOfLayers - 2; i > 0; i--)
		{
        for (j = 0; j < net->layers[i].numberOfNeurons; j++)
			{
            double output = net->layers[i].neurons[j].output;
            double sum = 0;
            for (k = 0 ; k < net->layers[i + 1].numberOfNeurons; k++)
				{
                sum += net->layers[i + 1].neurons[k].weights[j + 1] * net->layers[i + 1].neurons[k].delta;
				}
            net->layers[i].neurons[j].delta = output * (1 - output) * sum;
			}
		}

    //update weights
    for (i = 1; i < numberOfLayers; i++)
		{
        for (j = 0; j < net->layers[i].numberOfNeurons; j++)
			{
            for (k = 0; k <= net->layers[i - 1].numberOfNeurons; k++)
				{
                double inputForThisNeuron;
                if (k == 0)
                    inputForThisNeuron = 1;	//bias input
                else
                    inputForThisNeuron = net->layers[i - 1].neurons[k - 1].output;

                net->layers[i].neurons[j].weights[k] += ETA * net->layers[i].neurons[j].delta * inputForThisNeuron;
				}
			}
		}
	}

//*************************calculate error average*************//
// relative error = |average of second 10 errors : average of first 10 errors - 1|
// It is 0 if the errors stay constant, non-zero if the errors are changing rapidly
// these errors are from the training set --YKY
double relativeError(double *error, int len)
	{
    len = len - 1;
    if (len < 20)
        return 1;
    //keep track of the last 20 Root of Mean Square Errors
    int start1 = len - 20;
    int start2 = len - 10;

    double error1, error2 = 0;

    //calculate the average of the first 10 errors
    for (int i = start1; i < start1 + 10; i++)
        error1 += error[i];
    double averageError1 = error1 / 10;

    //calculate the average of the second 10 errors
    for (int i = start2; i < start2 + 10; i++)
        error2 += error[i];
    double averageError2 = error2 / 10;

    double relativeErr = (averageError1 - averageError2) / averageError1;
    return (relativeErr > 0) ? relativeErr : -relativeErr;
	}

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

//**************************main function***********************//
// Main algorithm:
// Input is copied into K.
// Desired output is K*.
// Do forward propagation (recurrently) a few times.
// Output is K'.  Error is K'-K*.
// Use back-prop to reduce this error.
// Repeat.

int train()
	{
    NNET *Net = (NNET *)malloc(sizeof(NNET));
    int numberOfLayers = 4;
    //the first layer -- input layer
    //the last layer -- output layer
    // int neuronsOfLayer[5] = {2, 3, 4, 4, 4};
    int neuronsOfLayer[4] = {10, 14, 13, 10};

    //read training data and testing data from file
    read_trainers();

    //create neural network for backpropagation
    createNeuronNetwork(Net, numberOfLayers, neuronsOfLayer);

    //error array to keep track of errors
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

        drawNetwork(Net, gfx);
        SDL_Delay(1000 /* milliseconds*/);

		}
    while(maxlen < MAX_EPOCHS);

    fclose(fout);
    free(Net);

    plot_rectangles(gfx); //keep the window open

    return 0;
	}