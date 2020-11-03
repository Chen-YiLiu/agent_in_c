#pragma once

/*
// a class that handles the loading of connection weights and neuron biases from a JSON file
// Author: Chen-Yi Liu
// May, 2017
*/

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <vector>
#include <string>

class LoadParameters
{
public:
	LoadParameters(const std::string filename)
	{
		boost::property_tree::read_json(filename, tree);
	}

	//load a list of bias values under the name "label"
	std::vector<float> &load_biases(const std::string label)
	{
		biases.clear(); //empties the container before loading new values

		for (boost::property_tree::ptree::value_type &v : tree.get_child(label))
		{
			biases.push_back(v.second.get_value<float>());
		}

		return biases;
	}

	// load a matrix of synaptic weight values from "label" in the JSON file
	 std::vector<std::vector<float>> &load_weights(const std::string label)
	{
		weights.clear(); //empties the container before loading new values

		for (boost::property_tree::ptree::value_type &node : tree.get_child(label))
		{
			std::vector<float> values;
			values.reserve(node.second.size()); //allocate enough memory to hold all the values

			for (boost::property_tree::ptree::value_type &v : node.second.get_child(""))
			{
				values.push_back(v.second.get_value<float>());
			}
			weights.push_back(std::move(values));
		}

		return weights;
	}

private:
	boost::property_tree::ptree tree;
	std::vector<float> biases;
	std::vector<std::vector<float>> weights;
};