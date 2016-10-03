/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#include"boost/filesystem.hpp"
#include"ork/ork.hpp"


namespace ork {


/*
Below is implemented generic directory iteration.
The only requirement on the functor is that following is a valid statement: functor(const path&);
We use this syntactic interface to avoid including boost::function in this header.
*/


//Just some type switches for searching (we could use the named argument paradigm instead)
struct search_type {};
struct flat_search :public search_type {};
struct recursive_search :public search_type {};


//And the implementation of the type switches
namespace detail {

template<class T>struct search_typer;

template<>struct search_typer<flat_search> {
	typedef file::directory_iterator type;
};

template<>struct search_typer<recursive_search> {
	typedef file::recursive_directory_iterator type;
};

}


//More type switches, this time for sorting
//We do not provide hooks into the sorting, so for now it produces lexicographical order
struct sort_type {};
struct sorted :public sort_type {};
struct unsorted :public sort_type {};


//Again, implementation of the type switches
namespace detail {

template<class functor, class iter_t, class sort>struct directory_executer {};

//Partial specialization for sorted visitation
template<class functor, class iter_t>struct directory_executer<functor, iter_t, sorted> {
	static void run(const file::path&dir, functor&f) {
		//Create a list of the paths in the directory, and then sort it
		std::vector<file::path>paths;
		std::copy(iter_t(dir), iter_t(), std::back_inserter(paths));
		std::sort(paths.begin(), paths.end());

		//Now execute the functor on each
		LOOPVI(paths)f(paths[i]);
	}
};

//Partial specialization for unsorted visitation
template<class functor, class iter_t>struct directory_executer<functor, iter_t, unsorted> {
	static void run(const file::path&dir, functor&f) {
		const iter_t end;
		for(iter_t it(dir); it != end; ++it)f(it->path());
	}
};

}


//The static version
template<class functor, class search_type = flat_search, class sort_type = unsorted>
struct iterate_directory {
	void operator()(const file::path&dir, functor&f) {
		run(dir, f);
	}
	static void run(const file::path&dir, functor&f) {
		typedef typename detail::search_typer<search_type>::type iter_t;
		if(!test_directory(dir))FILE_OPEN_ERR(TXT("Error searching directory!"), dir);
		detail::directory_executer<functor, iter_t, sort_type>::run(dir, f);
	}
};


//The runtime version
template<class functor>
void visit_directory(const file::path&dir, const bool recursive, const bool sort_it, functor&f) {
	if(sort_it) {
		if(recursive) {
			iterate_directory<functor, recursive_search, sorted>::run(f);
		}
		else {
			iterate_directory<functor, flat_search, sorted>::run(f);
		}
	}
	else {
		if(recursive) {
			iterate_directory<functor, recursive_search, unsorted>::run(f);
		}
		else {
			iterate_directory<functor, flat_search, unsorted>::run(f);
		}
	}
}


//Returns the subdirectory that sorts to the top
bool top_subdirectory(const file::path&dir, file::path&p);


template<class T>
struct directory_range {
public:
	typedef typename detail::search_typer<T>::type iterator;
protected:
	file::path _p;
public:
	directory_range(const file::path&p) : _p(p) {
		if(!file::is_directory(p))THROW_MSG(TXT("Non-directory used to initialize directory_range: ") << p.GEN_STR());
	}

	iterator begin() { return iterator(_p); }
	iterator end() { return iterator(); }
};

}//namespace ork
