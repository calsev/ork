/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include<string>
#include<sstream>
#include"boost/lexical_cast.hpp"

#include"ork/file_utils.hpp"
#include"ork/tagger.hpp"


namespace ork {


file::path tagger::_debug_root(ORK("./debug"));
std::mutex tagger::_mutex;

string tagger::sub_tag(const string&tag, size_t&index) {
	string_stream stream;
	stream << tag << index++ << ORK("_");
	return stream.str();
}

string tagger::operator()() {
	string_stream stream;
	file::path p(_debug_root);
	if(_number_folder) {
		stream << _tag << ORK("_") << _count++ << ORK("/");
		p /= stream.str();
		return p.ORK_GEN_STR();
	}
	else {
		stream << _tag << ORK("/");
		p /= stream.str();
		return p.ORK_GEN_STR() + boost::lexical_cast<string>(_count++) + ORK("_");
	}
}

unsigned tagger::count() {
	return _count.load();
}

void tagger::set_debug_root(const string&directory) {
	std::lock_guard<std::mutex> lock(_mutex);
	_debug_root = directory;
}

void tagger::set_debug_root(const string&as_is_path, const string&to_be_path) {
	file::path directory(ORK("./debug/"));
	directory /= file::path(as_is_path).stem();
	directory /= file::path(to_be_path).stem();
	std::lock_guard<std::mutex> lock(_mutex);
	_debug_root = directory.ORK_GEN_STR() + ORK("/");;
}


struct setup_hierarchy::impl {
public:
	string setup_directory;
	bool cached;
	unsigned score;
	std::vector<orientation>setups;
public:
	impl()
		: setup_directory()
		, cached(false)
		, score(0)
		, setups()
	{}
};


setup_hierarchy::setup_hierarchy(const string&setup_root, const string&as_is_path, const string&to_be_path)
	: _pimpl{ new impl{ } }
{
	if(!file::ensure_directory(setup_root))ORK_THROW(ORK("Setup root could not be created: ") << setup_root);
	//if(!test_file(as_is_path))ORK_THROW(ORK("As-is file could not be found: ") << as_is_path);
	//if(!test_file(to_be_path))ORK_THROW(ORK("To-Be file could not be found: ") << to_be_path);

	const string as_is_name(file::path(as_is_path).stem().ORK_GEN_STR());
	const string to_be_name(file::path(to_be_path).stem().ORK_GEN_STR());

	file::path setup_path(setup_root);
	setup_path /= as_is_name + ORK("/");
	setup_path /= to_be_name + ORK("/");
	_pimpl->setup_directory = setup_path.ORK_GEN_STR();
}


file::path setup_hierarchy::get_subdirectory() {
	file::path setup_dir;
	if(!top_subdirectory(get_path(), setup_dir))ORK_THROW(ORK("Setup directory not found for root: ") << get_path());
	_pimpl->cached = false;//Dirty, in case the client fetches more than one setup
	return setup_dir /= ORK("/");
}


struct setup_match {
	string setups;
	file::path setup_dir;
	setup_match(const string&exact_setups) :setups(exact_setups), setup_dir() {}
	void operator()(const file::path&p) {
		if(!setup_dir.empty())return;//Already found (this could be a superstring, so find might work)
		if(!ext_file::is_directory(p))return;//Just in case
		const string dir_name(p.stem().ORK_GEN_STR());
		if(dir_name.find(setups) != string::npos)setup_dir = p;
	}
};

const string&setup_hierarchy::get_path() {
	return _pimpl->setup_directory;
}

file::path setup_hierarchy::get_subdirectory(const string&exact_setups) {
	setup_match match(exact_setups);
	iterate_directory<setup_match, flat_search, sorted>::run(get_path(), match);
	if(match.setup_dir.empty())ORK_THROW(ORK("Setup permutation not found: ") << exact_setups);
	_pimpl->cached = false;//Dirty, in case the client fetches more than one setup
	return match.setup_dir;
}


const std::vector<orientation>& setup_hierarchy::get_setups() {
	return do_get_setups(file::canonical_normalized_trimmed(get_subdirectory()).filename().ORK_GEN_STR());
}

const std::vector<orientation>& setup_hierarchy::get_setups(const string&exact_setups) {
	return do_get_setups(file::canonical_normalized_trimmed(get_subdirectory(exact_setups)).filename().ORK_GEN_STR());
}


const std::vector<orientation>&setup_hierarchy::do_get_setups(const string&setup_dir) {
	if(!_pimpl->cached) {
		//Directory format: score [orientation]+
		//TODO: abstract writing, reading setups into a utility (right now process_plan class does the writing, but uses ACIS)
		i_string_stream strm(setup_dir);
		strm >> _pimpl->score;

		string setup;
		strm >> setup;
		while(strm) {
			_pimpl->setups.push_back(string2orientation(setup));
			strm >> setup;
		}
		if(_pimpl->setups.empty())ORK_THROW(ORK("Setup directory not formatted correctly: ") << get_path() << ORK("/") << setup_dir);

		_pimpl->cached = true;
	}
	if(_pimpl->setups.empty())ORK_THROW(ORK("Setups not found for directory: ") << setup_dir);
	return _pimpl->setups;
}


}//namespace ork
