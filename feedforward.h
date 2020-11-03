#pragma once

/*
* A continuous state neuron model class and a feedforward network layer class

* Author: Chen-Yi Liu
* April, 2017
*/


#include <vector>
#include <math.h>

////////////////////////////////////////////
// a simple continuous state neuron model
////////////////////////////////////////////
class Neuron
{
public:
	enum class Activation { ReLU, tanh, sigmoid, linear };

	Neuron(unsigned int num_of_inputs, const float init_w, const float init_b)
		:num_inputs(num_of_inputs), inputs(num_of_inputs), state(0.0), weights(num_of_inputs, init_w), bias(init_b)
	{}

	//~Neuron();

	const float value() const
	{
		return output;
	}

	// connect another neuron to one of the input ports. Returns true if successful 
	bool connect(const unsigned int location, Neuron *source)
	{
		if (location < num_inputs && source != nullptr)
		{
			inputs[location] = source;
			return true;
		}
		else return false;
	}

	// gather the output valuse of the connected neurons and use those values to update this neuron's state
	void propogate()
	{
		float input_signal = 0.0;

		for (unsigned int i = 0; i < num_inputs; i++)
		{
			if (inputs[i] != nullptr)
				input_signal += inputs[i]->value() * weights[i];
		}

		// the neuron can choose how much of its previous state will carry forward and how much of the new signal will come in 
		state = retension * state + (1 - retension) * input_signal + bias;

		// pass the state of the neuron through an activation function
		switch (activation)
		{
		case Activation::ReLU:
			if (state > 0.0) output = state;
			else output = 0.0;
			break;
		case Activation::tanh:
			output	= tanh(state);
			break;
		case Activation::sigmoid:
			output = 1 / (1 + exp(-state));
			break;
		case Activation::linear:
		default:
			output = state;
		}
	}

	const float get_weight(const unsigned int location) const
	{
		if (location < num_inputs)
		{
			return weights[location];
		}
		else return 0.0; //if the user asks for the value of a non existent variable, the function returns 0 as the default value
	}

	const float get_bias() const
	{
		return bias;
	}

	bool set_weight(const unsigned int location, const float new_weight)
	{
		if (location < num_inputs)
		{
			weights[location] = new_weight;
			return true;
		}
		else return false;
	}

	void set_bias(const float new_bias)
	{
		bias = new_bias;
	}

	void set_output(const float new_value)
	{
		output = new_value;
	}

	void set_activation_function(Activation new_activation_function)
	{
		activation = new_activation_function;
	}

private:
	unsigned int num_inputs;
	float state;
	float output;
	std::vector<Neuron*> inputs;
	std::vector<float> weights;
	float bias;
	float retension = 0; // set retension to 0 as I am not using it at the moment
	Activation activation = Activation::ReLU;
};
//////////////////////////////////

//////////////////////////////////////////
// A fully connected layer model in a feed forward network
//////////////////////////////////////////
class FeedForwardLayer
{
public:
	FeedForwardLayer(unsigned int num_of_neurons, unsigned int num_of_inputs)
		: num_neurons(num_of_neurons), num_inputs(num_of_inputs), neurons(num_of_neurons, Neuron(num_of_inputs, 0.0, 0.0))
	{}
	//~FeedForwardLayer(){}

	void propogate()
	{
		for (unsigned int i = 0; i < num_neurons; i++)
			neurons[i].propogate();
	}

	// A wrapper function that connects each neuron in this layer to neurons from the previous layer.
	bool connect_to_layer(FeedForwardLayer *new_input_layer)
	{
		if (new_input_layer != nullptr && num_inputs == new_input_layer->num_of_neurons()) //sanity check
		{
			input_layer = new_input_layer;

			for (unsigned int i = 0; i < num_neurons; i++)
			{
				for (unsigned int j = 0; j < num_inputs; j++)
				{
					neurons[i].connect(j, &(input_layer->neurons[j]));
				}
			}

			return true;
		}
		else return false;
	}

	const unsigned int num_of_neurons() const
	{
		return num_neurons;
	}

	const unsigned int num_of_inputs() const
	{
		return num_inputs;
	}

	// Set the connection weights of the neurons in the layer.
	// The first dimension of new_weights is the number of neurons in the model.
	// The second dimension is the number of neurons from the previous layer.
	// The dimensions of new_weights are[# of input neurons, # of current neurons]
	bool set_weights(const std::vector<std::vector<float>> &new_weights)
	{
		if (num_inputs == new_weights.size())
		{		
			// make sure the weight vector has the correct dimension
			for (const auto &v : new_weights)
			{
				if (num_neurons != v.size()) return false;
			}

			for (unsigned int i = 0; i < num_neurons; i++)
			{
				for (unsigned int j = 0; j < num_inputs; j++)
				{
					// the i and j index are swapped because the matrix in tensorflow is a transpose od what we use here
					neurons[i].set_weight(j, new_weights[j][i]);
				}
			}

			return true;
		}
		else return false;
	}

	bool set_biases(const std::vector<float> &new_biases)
	{
		if (num_neurons == new_biases.size())
		{
			for (unsigned int i = 0; i < num_neurons; i++)
			{
				neurons[i].set_bias(new_biases[i]);
			}

			return true;
		}
		else return false;
	}

	bool set_values(const std::vector<float> &new_values)
	{
		if (num_neurons == new_values.size())
		{
			for (unsigned int i = 0; i < num_neurons; i++)
			{
				neurons[i].set_output(new_values[i]);
			}
			return true;
		}
		else return false;
	}

	std::vector<Neuron> neurons;

private:
	unsigned int num_neurons, num_inputs;
	FeedForwardLayer *input_layer;
};
////////////////////////////////////////
