/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#include"ork/ork.hpp"


namespace ork {


/*
Class that generates some random distributions
Thread safe
*/
class random {
protected:
	struct impl;
protected:
	std::shared_ptr<impl>_pimpl;
public:
	random();
	~random();
public:
	double uniform_prob();
	double triangular_prob();//Symmetric about 0.5
	bool uniform_true(const double prob);

	int uniform(const int max);//[0, ..., max]
	size_t uniform(const size_t max);//[0, ..., max]

	int uniform_index(const int min, const int max);//[min, ..., max]
	size_t uniform_index(const size_t min, const size_t max);//[min, ..., max]

	size_t discrete_index(const std::vector<double>&likelihoods);//Weighted selection
};

random&g_random();//Static because construction is expensive


}//namespace ork
