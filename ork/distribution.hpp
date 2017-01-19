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
private:
	struct impl;
private:
	std::shared_ptr<impl>_pimpl;
public:
	ORK_ORK_API random();
	ORK_ORK_API ~random();
public:
	ORK_ORK_API double uniform_prob();
	ORK_ORK_API double triangular_prob();//Symmetric about 0.5
	ORK_ORK_API bool uniform_true(const double prob);

	ORK_ORK_API int uniform(const int max);//[0, ..., max]
	ORK_ORK_API size_t uniform(const size_t max);//[0, ..., max]

	ORK_ORK_API int uniform_index(const int min, const int max);//[min, ..., max]
	ORK_ORK_API size_t uniform_index(const size_t min, const size_t max);//[min, ..., max]

	ORK_ORK_API size_t discrete_index(const std::vector<double>&likelihoods);//Weighted selection
};

ORK_ORK_EXT(random&) g_random();//Static because construction is expensive


template<typename T>
class triangle_distribution {
public:
	using val_type = T;
protected:
	val_type _min;
	val_type _mid;
	val_type _max;
public:
	triangle_distribution(const val_type min, const val_type mid, const val_type max) :_min(min), _mid(mid), _max(max) {
		if(min >= mid) {
			ORK_THROW(ORK("Min must be less than mid"));
		}
		if(mid >= max) {
			ORK_THROW(ORK("Mid must be less than max"));
		}
	}
public:
	val_type operator()(const val_type val)const {
		if(val < _min) {
			return static_cast<val_type>(0);
		}
		if(val <= _mid) {
			return (val - _min) / (_mid - _min);
		}
		if(val <= _max) {
			return (_max - val) / (_max - _mid);
		}
		//val > max
		return static_cast<val_type>(0);
	}
};


}//namespace ork
