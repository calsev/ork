/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#include<atomic>
#include<string>
#include<mutex>
#include"ork/orientation.hpp"


namespace ork {

//This class is thread safe and intended for static usage.
//We might make a global environment with in the future, but for now we use a static to set the root directory
class tagger {
private:
	static file::path _debug_root;//Global guarded by mutex
	static std::mutex _mutex;
	std::atomic<unsigned>_count;//Local static
	const bool _number_folder;//Local static constant
	const string _tag;//Local static constant
public:
	ORK_ORK_API static string sub_tag(const string&tag, size_t&index);
public:
	ORK_INLINE explicit tagger(const string&tag) :_count(0), _number_folder(true), _tag(tag) {}
	ORK_INLINE tagger(const string&tag, bool numbered_folders) :_count(0), _number_folder(numbered_folders), _tag(tag) {}
	ORK_NON_COPYABLE(tagger)
public:
	ORK_ORK_API static void set_debug_root(const string&directory);
	ORK_ORK_API static void set_debug_root(const string&as_is_path, const string&to_be_path);
	ORK_ORK_API string operator()();//Fetch instance tag
	ORK_ORK_API unsigned count();//The number of times this has been invoked
};


//TODO: This is not reusable code, push down to project
//We use this class to generate setup paths so that downstream can use the exact same logic
class setup_hierarchy {
private:
	string _setup_directory;
	bool _cached;
	unsigned _score;
	std::vector<orientation>_setups;
public:
	ORK_ORK_API setup_hierarchy(const string&setup_root, const string&as_is_path, const string&to_be_path);
public:
	ORK_INLINE const string&get_path() {
		return _setup_directory;
	}
	ORK_ORK_API file::path get_subdirectory();
	ORK_ORK_API file::path get_subdirectory(const string&exact_setups);
public:
	ORK_ORK_API const std::vector<orientation>&get_setups();
	ORK_ORK_API const std::vector<orientation>&get_setups(const string&exact_setups);
private:
	const std::vector<orientation>&do_get_setups(const string&setup_dir);
};

}//namespace ork
