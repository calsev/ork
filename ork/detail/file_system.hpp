/*
This file is part of the ORK_STR library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#ifndef ORK_ORK_HPP
#error This header can only be included from ork/common.hpp!
#endif


#include<sstream>
#include<fstream>
//Some boost configuration here
#define BOOST_SYSTEM_NO_DEPRECATED 1
#define BOOST_FILESYSTEM_NO_DEPRECATED 1
#include"ork/detail/tools.hpp"
#include"ork/detail/exception.hpp"


namespace ork {


//Forward declaration to keep boost out of headers
namespace file = boost::filesystem;
class file::path;


bool test_directory(const file::path&file_or_directory);
bool test_file(const file::path&file);

//Create a directory, and all parent directories if these do not already exist.
//Return true if the directory was created or if it already existed, otherwise false to indicate an error.
//This function can handle absolute or relative path names.
//The file version creates the parent directory.
bool ensure_directory(file::path file_or_directory);
bool ensure_file(const file::path&file);


#define ORK_FILE_OPEN_ERR(MSG,FILE_NAME)STMT(\
	ORK_THROW(MSG << ORK("\n -- Path: \"") << FILE_NAME << ORK("\""))\
)

#define ORK_FILE_READ(FILE_NAME)\
	ork::if_stream fin(FILE_NAME);\
	if(!fin)ORK_FILE_OPEN_ERR(ORK("Could not open input file!"), FILE_NAME)
#define ORK_FILE_WRITE(FILE_NAME)\
	ork::of_stream fout(FILE_NAME);\
	if(!fout)ORK_FILE_OPEN_ERR(ORK("Could not open output file!"), FILE_NAME)

}//namespace ork
