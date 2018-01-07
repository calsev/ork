/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include"ork/ork.hpp"
#define ORK_STL_INC_FILE <mutex>
#include"ork/core/stl_include.inl"
#define ORK_STL_INC_FILE <sstream>
#include"ork/core/stl_include.inl"
#define ORK_STL_INC_FILE <vector>
#include"ork/core/stl_include.inl"

#include"ork/tagger.hpp"
#include"ork/file_utils.hpp"

#if ORK_MSC
#pragma warning(push)
#pragma warning(disable:4668) //Windows undefined macro
#if ORK_MSC > 1900
#   pragma warning(disable:5039) //pointer or reference to throwing function passed to extern C function
#endif
#define NOMINMAX
#include"windows.h"
#include"DbgHelp.h"
#pragma warning(pop)
#endif


namespace ork {


/*
From database.hpp
*/

bstring uuid2string(const char*const value) {
	b_string_stream stream;
	stream << std::hex;
	LOOPJ(16) {
		stream << (value[j] & (char(15) << 4));
		stream << (value[j] & char(15));
	}
	return stream.str();
}
std::vector<char> string2uuid(const bstring&val) {
	std::vector<char> retval;
	b_string_stream stream;
	stream << std::hex << val;
	LOOPJ(16) {
		char upper, lower;
		stream >> upper >> lower;
		retval.push_back((upper << 4) | lower);
	}
	return retval;
}


/*
From exception.hpp
*/

#if ORK_MSC

backtrace generate_backtrace(const unsigned skip_frames) {
	static const unsigned capture_frames = 128;
	static const unsigned max_name_len = 1024;

	HANDLE process = GetCurrentProcess();
	SymInitialize(process, nullptr, TRUE);

	void *addrs[capture_frames];
	const unsigned short num_addr = CaptureStackBackTrace(skip_frames + 1, capture_frames, addrs, nullptr);

	// Oddly allocated, I know. You must allocate additional memory after the
	// end of the SYMBOL_INFO structure to store the name. This memory must be
	// contiguous, so we can't use basic "new" statements, must resort to malloc
	SYMBOL_INFO *symbol = static_cast<SYMBOL_INFO *>(malloc(sizeof(SYMBOL_INFO) + sizeof(char) * max_name_len));
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol->MaxNameLen = max_name_len - 1;

	// This one stores file/line information
	IMAGEHLP_LINE64 line;
	line.SizeOfStruct = sizeof(line);

	backtrace result;

	LOOPI(num_addr) {
		DWORD64 addr = reinterpret_cast<DWORD64>(addrs[i]);

		DWORD displacement; // I don't care about this output parameter, but it must be supplied
		SymFromAddr(process, addr, 0, symbol);
		SymGetLineFromAddr64(process, addr, &displacement, &line);

		stack_frame frame;
		frame.function = symbol->Name;
		frame.filename = line.FileName;
		frame.line = line.LineNumber;
		result.frames.push_back(frame);
	}

	free(symbol);
	return result;
}


bstring backtrace::str() const {
	b_string_stream stream;

	stream << BORK("backtrace:\n");

	unsigned index = 0;
	for(const stack_frame&sf : frames) {
		stream << index++ << BORK(": ") << sf.function << BORK(" (") << sf.filename << BORK(":") << sf.line << BORK(")\n");
	}

	return stream.str();
}

#else//ORK_MSC

backtrace generate_backtrace(const unsigned skip_frames) {
	return backtrace();
}


bstring backtrace::str() const {
	return bstring();
}

#endif//ORK_MSC

std::ostream &operator << (std::ostream &stream, const backtrace &trace) {
	return stream << trace.str();
}


/*
From file_system.hpp
*/

namespace file {
std::array<uint8_t, 3>utf8_bom = { 0xEF, 0xBB, 0xBF };
std::array<uint8_t, 4>utf8_bom_ = { 0xEF, 0xBB, 0xBF, 0x00 };//For null termination
const char*const utf8_bom_str = reinterpret_cast<char*>(utf8_bom_.data());//Reinterpret cast used to handle pos/neg chars


path canonical_normalized_trimmed(const path&file) {
	const path complete = ext_file::system_complete(file);
	const path canonical = ext_file::canonical(complete);
	string name(canonical.ORK_GEN_STR());
	while(!name.empty() && name.back() == '/') {
		name.pop_back();
	}
	return path(name);
}


bool test_directory(const path&file_or_directory) {
	return ext_file::exists(file_or_directory) && ext_file::is_directory(file_or_directory);
}
bool test_file(const path&file) {
	return ext_file::exists(file) && ext_file::is_regular_file(file);
}
bool ensure_directory(const path&file_or_directory_) {
	//Remove remove_filename and has problems with mixed slashes
	path file_or_directory = canonical_normalized_trimmed(file_or_directory_);
	if(file_or_directory.empty()) {
		return true;
	}
	if(test_directory(file_or_directory)) {
		return true;
	}
	if(file_or_directory.has_extension()) {//Hard to tell if a path would be a directory or file
		file_or_directory.remove_filename();
	}
	file_or_directory = canonical_normalized_trimmed(file_or_directory);
	if(file_or_directory.empty()) {
		return true;//The path was a file in the current directory
	}
	if(test_directory(file_or_directory)) {
		return true;
	}
	return ext_file::create_directories(file_or_directory);
}
bool ensure_file(const path&file) {
	if(!ensure_directory(file))return false;
	if(file.empty() || ext_file::exists(file))return true;
	if(!ext_file::is_regular_file(file))return false;
	return ext_file::create_directories(file);
}

}//namespace file

/*
From file_utils.hpp
*/

bool discard_bom(bi_stream&fin) {
	for(const uint8_t bom : file::utf8_bom) {
		const uint8_t curr = static_cast<uint8_t>(fin.get());
		if(curr != bom) {
			fin.seekg(0);
			return false;
		}
	}
	return true;
}


bool top_subdirectory(const file::path&dir, file::path&p) {
	if(!file::test_directory(dir))ORK_FILE_OPEN_ERR(ORK("Error searching directory!"), dir);

	//Create a list of the paths in the directory, and then sort it
	std::vector<file::path>paths;
	std::copy(ext_file::directory_iterator(dir), ext_file::directory_iterator(), std::back_inserter(paths));
	std::sort(paths.begin(), paths.end());

	for(const file::path&curr_path : paths) {
		if(ext_file::is_directory(curr_path)) {
			p = curr_path;
			return true;
		}
	}

	return false;
}


/*
From text.hpp
*/

#if ORK_MSC
#   pragma warning(push)
#   pragma warning(disable:4996) //<codecvt> deprecated but not replaced; expecting a replacement in C++ 20??
#endif

struct string_converter_type::impl {
public:
	typedef std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>string_converter;
	typedef std::mutex mutex_type;
	typedef std::lock_guard<mutex_type>lock_type;
public:
	impl() {}
	ORK_NON_COPYABLE(impl)
public:
	string_converter converter;
	std::mutex mutex;
};


string_converter_type::string_converter_type() : _pimpl{ new impl{} } {}
string_converter_type::~string_converter_type() {}

bstring string_converter_type::wide2byte(const wchar_t s) {
	impl::lock_type lock(_pimpl->mutex); return _pimpl->converter.to_bytes(s);
}
bstring string_converter_type::wide2byte(const wchar_t*s) {
	impl::lock_type lock(_pimpl->mutex); return _pimpl->converter.to_bytes(s);
}
bstring string_converter_type::wide2byte(const wstring&s) {
	impl::lock_type lock(_pimpl->mutex); return _pimpl->converter.to_bytes(s);
}
bstring string_converter_type::wide2byte(const wchar_t*first, const wchar_t*last) {
	impl::lock_type lock(_pimpl->mutex); return _pimpl->converter.to_bytes(first, last);
}

wstring string_converter_type::byte2wide(const char s) {
	impl::lock_type lock(_pimpl->mutex); return _pimpl->converter.from_bytes(s);
}
wstring string_converter_type::byte2wide(const char*s) {
	impl::lock_type lock(_pimpl->mutex); return _pimpl->converter.from_bytes(s);
}
wstring string_converter_type::byte2wide(const bstring&s) {
	impl::lock_type lock(_pimpl->mutex); return _pimpl->converter.from_bytes(s);
}
wstring string_converter_type::byte2wide(const char*first, const char*last) {
	impl::lock_type lock(_pimpl->mutex); return _pimpl->converter.from_bytes(first, last);
}

#if ORK_MSC
#   pragma warning(pop)
#endif


string_converter_type&g_string_converter() {//From string_types.hpp
	static string_converter_type s_convert;
	return s_convert;
}

}//namespace ork
