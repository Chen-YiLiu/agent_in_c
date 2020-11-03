/*
The program loads parameters of a trained neural network from a JSON file.
Then, using the neural network, the program builds an Actor-Critic reinforcement learning agent.
The agent communicates with the OpenAI gym environment through a compatible Python server program over TCP/IP.

Author: Chen-Yi Liu
Date: May, 2017
*/

//#include "stdafx.h"
#include <boost/asio.hpp>
#include <iostream>
#include "feedforward.h"
#include "load_parameters.h"

using boost::asio::ip::tcp;

const int ACTION_DIM = 1, STATE_DIM = 3;


/////////////////////////////////////////
// A feed forward network
/////////////////////////////////////////
class ActorNetwork
{
public:
	const int input_dim = STATE_DIM;
	const int output_dim = ACTION_DIM;
	FeedForwardLayer input_layer{ input_dim, 0 };
	FeedForwardLayer layer1{ 50, input_dim };
	FeedForwardLayer layer2{ 10, 50 };
	FeedForwardLayer layer3{ 1, 10 };
	FeedForwardLayer output_layer{ output_dim, 1 };

	ActorNetwork()
	{
		layer1.connect_to_layer(&input_layer);
		layer2.connect_to_layer(&layer1);
		layer3.connect_to_layer(&layer2);
		output_layer.connect_to_layer(&layer3);
	}

	const std::vector<float> &run_data(std::vector<float> &input)
	{
		input_layer.set_values(input);
		layer1.propogate();
		layer2.propogate();
		layer3.propogate();
		output_layer.propogate();

		output.clear();
		for (int i = 0; i < output_dim; i++)
		{
			output.push_back(output_layer.neurons[i].value());
		}
		
		return output;
	}

private:
	std::vector<float> output;
};

const char *weight_labels[] = { "actor_layer_1_w", "actor_layer_2_w", "actor_layer_3_w" };
const char *bias_labels[] = { "actor_layer_1_b", "actor_layer_2_b", "actor_layer_3_b" };

int main()
{
	// Build an actor network using parameters trained with tensorflow and saved in a JSON file
	ActorNetwork actor;
	try
	{
		LoadParameters trained{ "extracted_weights.json" };

		actor.layer1.set_weights(trained.load_weights(weight_labels[0]));
		actor.layer1.set_biases(trained.load_biases(bias_labels[0]));
		actor.layer2.set_weights(trained.load_weights(weight_labels[1]));
		actor.layer2.set_biases(trained.load_biases(bias_labels[1]));
		actor.layer3.set_weights(trained.load_weights(weight_labels[2]));
		actor.layer3.set_biases(trained.load_biases(bias_labels[2]));
		actor.layer3.neurons[0].set_activation_function(Neuron::Activation::tanh);
		actor.output_layer.neurons[0].set_weight(0, 2.0);
		actor.output_layer.neurons[0].set_activation_function(Neuron::Activation::linear);
	}
	catch (boost::property_tree::json_parser_error &e)
	{
		std::cout << e.what() << std::endl;
		return -1;
	}

	// Start the connection to the gym server
	try
	{
		// use boost asio to do tcp communication with a local server that runs the openai gym
		boost::asio::io_service io_service;
		tcp::resolver resolver(io_service);
		tcp::resolver::query query("localhost", "1000");
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::socket socket(io_service);
		boost::asio::connect(socket, endpoint_iterator);

		// reinforcement learning variables
		std::vector<float> action(ACTION_DIM, 0.0), state(STATE_DIM);
		float reward = 0.0;

		//Send and receive data
		while (true)
		{
			boost::system::error_code error;

			//read from socket
			size_t len = boost::asio::read(socket, boost::asio::buffer(state), error);

			// check for socket reading error
			if (error == boost::asio::error::eof)
			{
				std::cout << "Connection closed on the server side." << std::endl; // Connection closed cleanly by peer.
				break;
			}
			else if (error)
				throw boost::system::system_error(error); // Some other error.

			// reward is the last element of the vector
			reward = state.back();

			// getting rid of the reward value leaves us with the state values
			state.pop_back();

			// use the current state vector to calculate an appropriate action using the actor network
			action = actor.run_data(state);

			//print out the values and check if they are correct
			/*for (const auto &value : action)
			{
				std::cout << value << std::endl;
			}
			*/

			//write to socket
			len = boost::asio::write(socket, boost::asio::buffer(action), error);

			// check for socket writing error
			if (error == boost::asio::error::eof)
			{
				std::cout << "Connection closed on the server side." << std::endl; // Connection closed cleanly by peer.
				break;
			}
			else if (error)
				throw boost::system::system_error(error); // Some other error.
		}

	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}


	return 0;
}