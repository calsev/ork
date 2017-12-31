/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include<mutex>
#include<thread>
#include<vector>

#include"ork/ork.hpp"
#if ORK_MSC
#pragma warning(push)
#pragma warning(disable:4365) //Boost argument conversion
#pragma warning(disable:4668) //Boost undefined macro
#pragma warning(disable:5031) //Boost unmatched pragma push
#endif

#include"ork/distribution.hpp"

#include<boost/random//discrete_distribution.hpp>
#include<boost/random/taus88.hpp>
#include<boost/random/triangle_distribution.hpp>
#include<boost/random/uniform_01.hpp>
#include<boost/random/uniform_int_distribution.hpp>


namespace ork {


struct random::impl {
protected:
	boost::random::taus88 _gen;
	std::mutex _mutex;
public:
	impl() : _gen{}, _mutex{} {}
	ORK_NON_COPYABLE(impl)
public:
	double uniform_prob() {
		boost::random::uniform_01<>dist{};
		std::lock_guard<std::mutex> lock{_mutex};
		return dist(_gen);
	}
	double triangular_prob() {
		//Just convolve two uniform vars (uniform_prob()+uniform_prob())/2
		///or use the built-in distribution in boost!
		boost::random::triangle_distribution<double>dist;
		std::lock_guard<std::mutex> lock{_mutex};
		return dist(_gen);
	}
	bool uniform_true(const double prob) {
		return uniform_prob()<prob || prob == 1.;//0 and 1 are tricky, we need to handle true 0% and 100& prob
	}

	template<typename T>T uniform(const T max) {
		boost::random::uniform_int_distribution<T>dist{max};
		std::lock_guard<std::mutex> lock{_mutex};
		return dist(_gen);
	}

	template<typename T>T uniform_index(const T min, const T max) {
		boost::random::uniform_int_distribution<T>dist{min, max};
		std::lock_guard<std::mutex> lock{_mutex};
		return dist(_gen);
	}

	template<typename T>size_t discrete_index(const std::vector<T>&likelihoods) {
		//Probability of choosing J = Pj/(Sum(Pi))
		//Discrete probability distribution is built into boost::random, and it supports weights not summing to 1
		boost::random::discrete_distribution<size_t, T>dist(likelihoods);
		std::lock_guard<std::mutex> lock{_mutex};
		return dist(_gen);
	}
};


random::random() :_pimpl{new impl{}} {}
random::~random() {}


double random::uniform_prob() {
	return _pimpl->uniform_prob();
}
double random::triangular_prob() {
	return _pimpl->triangular_prob();
}
bool random::uniform_true(const double prob) {
	return _pimpl->uniform_true(prob);
}


int random::uniform(const int max) {
	return _pimpl->uniform(max);
}
size_t random::uniform(const size_t max) {
	return _pimpl->uniform(max);
}


int random::uniform_index(const int min, const int max) {
	return _pimpl->uniform_index(min, max);
}

size_t random::uniform_index(const size_t min, const size_t max) {
	return _pimpl->uniform_index(min, max);
}


size_t random::discrete_index(const std::vector<double>&likelihoods) {
	return _pimpl->discrete_index(likelihoods);
}


/*
Globals
*/
random&g_random() {
	static random inst;//Static because construction is expensive
	return inst;
}


}//namespace ork

#if ORK_MSC
#pragma warning(pop)
#endif
