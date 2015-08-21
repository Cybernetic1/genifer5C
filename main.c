// TO-DO:
// 3. hook up with RL -- needs ANN for Q
// 4. test with small problem with rewards

// TODO (yky#1#): There is a convergence problem -- it always converges.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <SDL2/SDL.h>

// #define DATASIZE 800		// size of training data and test data
#define ETA 0.01			// learning rate
#define BIASOUTPUT 1		// output for bias. It's always 1.
#define MAX_EPOCHS 30		// maximum epochs
#define K_length 10			// size of cognitive state vector

//**********************struct for NEURON**********************************//
typedef struct NEURON
	{
	double input;
	double output;
	double *weights;
	double delta;
	double error;
	} NEURON;

//**********************struct for LAYER***********************************//
typedef struct LAYER
	{
	int numberOfNeurons;
	NEURON *neurons;
	} LAYER;

//*********************struct for NNET************************************//
typedef struct NNET
	{
	double *inputs;
	int numberOfLayers;
	LAYER *layers;
	} NNET; //neural network


//********sigmoid function and randomWeight generator********************//

double sigmoid(double v)
	{
	return 1 / (1 + exp(-v));
	}

double randomWeight()	//random weight generator between -0.5 ~ 0.5
	{
	return ((int)rand() % 100000) / (float) 100000 - 0.5;
	}

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

	/* calculate mean square error;
	double y[4] = {0, 0, 0, 0};	 //desired value array
	y[data.desired_value - 1] = 1;
	double sumOfSquareError = 0;
	int numberOfLayers = net->numberOfLayers;
	// This means each output neuron corresponds to a classification label --YKY
	for(i = 0; i < net->layers[numberOfLayers - 1].numberOfNeurons; i++)
		{
		//error = desired_value - output
		double error = y[i] - net->layers[numberOfLayers - 1].neurons[i].output;
		net->layers[numberOfLayers - 1].neurons[i].error = error;
		sumOfSquareError += error * error / 2;
		}
	double mse = sumOfSquareError / net->layers[numberOfLayers - 1].numberOfNeurons;
	return mse;	 //return the root of mean square error
	*/
	}


//**************************backpropagation***********************//

void backpropagation(NNET *net)
	{
	//calculate delta
	int i, j, k;
	int numberOfLayers = net->numberOfLayers;

	//calculate delta for output layer
	for (i = 0; i < net->layers[numberOfLayers - 1].numberOfNeurons; i++)
		{
		double output = net->layers[numberOfLayers - 1].neurons[i].output;
		double error = net->layers[numberOfLayers - 1].neurons[i].error;
		//for output layer, delta = y(1-y)error
		net->layers[numberOfLayers - 1].neurons[i].delta = output * (1 - output) * error;
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


/*******************read training data and testing data**********************

void getTrainingAndTestData(int argc, char **path, ENTRY *training, ENTRY *testing)
	{
	if(argc != 3)
		{
		printf("Usage: program training_data_file testing_data_file\n");
		exit(0);
		}

	FILE *fp1, *fp2;
	if((fp1 = fopen(path[1], "r")) == NULL)
		{
		printf("cannot open %s\n", path[1]);
		exit(1);
		}
	if((fp2 = fopen(path[2], "r")) == NULL)
		{
		printf("cannot open %s\n", path[2]);
		exit(1);
		}

	int i = 0;
	int num1, num2;
	while(i < 800)
		{
		fscanf(fp1, "%d %d %lf %lf", &num1, &num2, &training[i].x1, &training[i].x2);
		fscanf(fp2, "%d %d %lf %lf", &num1, &num2, &testing[i].x1, &testing[i].x2);
		if(i < 200)
			{
			training[i].desired_value = 1;
			testing[i].desired_value = 1;
			}
		else if(i < 400)
			{
			training[i].desired_value = 2;
			testing[i].desired_value = 2;
			}
		else if(i < 600)
			{
			training[i].desired_value = 3;
			testing[i].desired_value = 3;
			}
		else
			{
			training[i].desired_value = 4;
			testing[i].desired_value = 4;
			}
		i++;
		}
	fclose(fp1);
	fclose(fp2);
	}
*/

/********shuffle the order of presentation to neuron*********
void swap(ENTRY *data, int i, int j)
	{
	ENTRY temp;
	temp.x1 = data[i].x1;
	temp.x2 = data[i].x2;
	temp.desired_value = data[i].desired_value;
	data[i].x1 = data[j].x1;
	data[i].x2 = data[j].x2;
	data[i].desired_value = data[j].desired_value;
	data[j].x1 = temp.x1;
	data[j].x2 = temp.x2;
	data[j].desired_value = temp.desired_value;
	}

void shuffle(ENTRY *data, int size)
	{
	srand(time(NULL));
	int i;
	for(i = 0; i < size; i++)
		{
		int j = (int)rand() % size;
		swap(data, i, j);
		}
	}
*/

/*
void printWeights(NNET *net)
{
	int i, j, k;
	 for(i = 1; i < net->numberOfLayers; i++)
		{
			printf("Layer %d\n", i);
			for(j = 0; j < net->layers[i].numberOfNeurons; j++)
	{
		printf("\t Neuron %d : ", j);
		for(k = 0; k <= net->layers[i-1].numberOfNeurons; k++)
			{
				printf("%lf ", net->layers[i].neurons[j].weights[k]);
			}
		printf("\n");
	}
		}
}
*/


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

	double error1 = 0;
	double error2 = 0;

	int i;

	//calculate the average of the first 10 errors
	for (i = start1; i < start1 + 10; i++)
		{
		error1 += error[i];
		}
	double averageError1 = error1 / 10;

	//calculate the average of the second 10 errors
	for (i = start2; i < start2 + 10; i++)
		{
		error2 += error[i];
		}
	double averageError2 = error2 / 10;

	double relativeErr = (averageError1 - averageError2) / averageError1;
	return (relativeErr > 0) ? relativeErr : -relativeErr;
	}



//**************************main function***********************//

int main(int argc, char** argv)
	{
	NNET *Net = (NNET *)malloc(sizeof(NNET));
	int numberOfLayers = 4;
	#define lastLayer (numberOfLayers - 1)
	//the first layer -- input layer
	//the last layer -- output layer
	// int neuronsOfLayer[5] = {2, 3, 4, 4, 4};
	int neuronsOfLayer[4] = {10, 14, 13, 10};

	double K[K_length];

	//read training data and testing data from file
	// getTrainingAndTestData(argc, argv, training_data, testing_data);

	//create neural network for backpropagation
	createNeuronNetwork(Net, numberOfLayers, neuronsOfLayer);

	//error array to keep track of errors
	// double *error = (double *)malloc(MAX_EPOCHS * sizeof(double));
	int maxlen = 0;
	int epoch = 1;

	plot_rectangles();

	//output data to a file
	FILE *fout;
	if ((fout = fopen("randomtest_1.txt", "w")) == NULL)
		{
		fprintf(stderr, "file open failed.\n");
		exit(1);
		}

	//shuffle the order of presenting training data to neural network
	//shuffle(training_data, DATASIZE);

	// Do forward-propagation, then feed output to input
	do
		{
		// double squareErrorSum = 0;

		feedforward(Net, K);

		// copy output to input
		for (int i = 0; i < K_length; ++i)
			K[i] = Net->layers[lastLayer].neurons[i].output;

		// backpropagation(Net);
		// error[maxlen] = sqrt(squareErrorSum / DATASIZE);
		//test network
		// printf(		  "%d  \t %lf\t %lf\n", epoch, 0.0f);
		printf("%03d:", epoch);
		for (int i = 0; i < K_length; ++i)
			printf(" %lf", K[i]);
		printf("\n");
		// fprintf(fout, "%d", epoch);
		maxlen++;
		epoch++;
		}
	while(maxlen < MAX_EPOCHS);

	fclose(fout);
	free(Net);

	return 0;
	}

void plot_rectangles()
	{

	bool loadMedia()
		{
		//Loading success flag
		bool success = true;

		//Nothing to load
		return success;
		}

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
		printf("SDL_Init Error: %s \n", SDL_GetError());
		return 1;
		}

	SDL_Window *win = SDL_CreateWindow("Hello World!", 10, 10, 800, 900, SDL_WINDOW_SHOWN);
	if (win == NULL)
		{
		printf("SDL_CreateWindow Error: %s \n", SDL_GetError());
		SDL_Quit();
		return 1;
		}

	SDL_Renderer *gRenderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (gRenderer == NULL)
		{
		SDL_DestroyWindow(win);
		printf("SDL_CreateRenderer Error: %s \n", SDL_GetError());
		SDL_Quit();
		return 1;
		}

	bool quit;
	SDL_Event e;
	//While application is running
	while ( !quit )
		{
		//Handle events on queue
		while ( SDL_PollEvent( &e ) != 0 )
			{
			//User requests quit
			if ( e.type == SDL_QUIT )
				{
				quit = true;
				}//Render red filled quad
			#define SCREEN_WIDTH 800
			#define SCREEN_HEIGHT 900
			SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );
            SDL_RenderFillRect( gRenderer, &fillRect );
			}

		//Clear screen
		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear( gRenderer );

		//Render red filled quad
		SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );
		SDL_RenderFillRect( gRenderer, &fillRect );

		//Render green outlined quad
		SDL_Rect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
		SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
		SDL_RenderDrawRect( gRenderer, &outlineRect );

		//Draw blue horizontal line
		SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, 0xFF );
		SDL_RenderDrawLine( gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2 );

		//Draw vertical line of purple dots
		SDL_SetRenderDrawColor( gRenderer, 0x44, 0x00, 0x44, 0xFF );
		for( int i = 0; i < SCREEN_HEIGHT; i += 4 )
			{
			SDL_RenderDrawPoint( gRenderer, SCREEN_WIDTH / 2, i );
			}

		//Update screen
		SDL_RenderPresent( gRenderer );
		}
	}

int test_SDL()
	{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
		printf("SDL_Init Error: %s \n", SDL_GetError());
		return 1;
		}

	SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (win == NULL)
		{
		printf("SDL_CreateWindow Error: %s \n", SDL_GetError());
		SDL_Quit();
		return 1;
		}

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL)
		{
		SDL_DestroyWindow(win);
		printf("SDL_CreateRenderer Error: %s \n", SDL_GetError());
		SDL_Quit();
		return 1;
		}

	SDL_Surface *bmp = SDL_LoadBMP("hello.bmp");
	if (bmp == NULL)
		{
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		printf("SDL_LoadBMP Error: %s \n", SDL_GetError());
		SDL_Quit();
		return 1;
		}

	SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
	SDL_FreeSurface(bmp);
	if (tex == NULL)
		{
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		printf("SDL_CreateTextureFromSurface Error: %s \n", SDL_GetError());
		SDL_Quit();
		return 1;
		}

	//A sleepy rendering loop, wait for 3 seconds and render and present the screen each time
	for (int i = 0; i < 3; ++i)
		{
		//First clear the renderer
		SDL_RenderClear(ren);
		//Draw the texture
		SDL_RenderCopy(ren, tex, NULL, NULL);
		//Update the screen
		SDL_RenderPresent(ren);
		//Take a quick break after all that hard work
		SDL_Delay(1000);
		}

	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	}
