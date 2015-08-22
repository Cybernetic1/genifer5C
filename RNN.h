
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

