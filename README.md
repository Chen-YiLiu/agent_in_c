# agent_in_c
A reinforcement learning agent written in C++. (Inference only)


After training a reinforcement learning agent using the Deep Deterministic Policy Gradient algorithm in Python and Tensorflow, the trained neural network parameters can be exported to a JSON file.

This C++ program reads the parameters from the JSON file and builds a copy of the trained network. Therefore, the agent can be run as an independent program without the need of Tensorflow.

The program communicates with a server side program written in Python to send actions and to receive state values from a learning environment, eg. OpenAI gym.

## File Structure
- FeedForwardModel.cpp: the main program that runs as the agent. depends on feedforward.h and load_parameters.h
- gym_server.py: the server program that acts as an interface between the agent and the simulator, in this case, OpenAI gym. depends on Server_socket.py
- extracted_weights.json: an example JSON file that contains trained network parameters of an agent capable of solving the Pendulum-v0 environment in OpenAI gym


#### Author: Chen-Yi Liu
#### Date: May, 2017
