/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#ifndef ORK_ORK_HPP
#    error This header can only be included from ork/ork.hpp!
#endif
#ifdef ORK_FILE_SYSTEM_HPP
#    error This header can only be included from ork/ork.hpp!
#endif

#ifndef ORK_FILE_SYSTEM_HPP
#    define ORK_FILE_SYSTEM_HPP


#    if ORK_STL_HAS_FILE

namespace std {
namespace experimental {
namespace filesystem {
inline namespace v1 {
class path;
} // namespace v1
} // namespace filesystem
} // namespace experimental
} // namespace std

namespace ork {
namespace ext_file = std::experimental::filesystem::v1;
} // namespace ork

#    else

// Some boost configuration here
#        define BOOST_SYSTEM_NO_DEPRECATED 1
#        define BOOST_FILESYSTEM_NO_DEPRECATED 1

namespace boost {
namespace filesystem {
class path;
bool exists(const path&);
} // namespace filesystem
} // namespace boost

namespace ork {
namespace ext_file = boost::filesystem;
} // namespace ork

#    endif


namespace ork {
namespace file {

// Forward declaration to keep boost out of headers
using path = ext_file::path;

ORK_ORK_EXTERN std::array<uint8_t, 3> utf8_bom;
ORK_ORK_EXTERN const char* const utf8_bom_str;


ORK_ORK_EXT(path) canonical_normalized_trimmed(const path&);


ORK_ORK_EXT(bool) test_directory(const path&);
ORK_ORK_EXT(bool) test_file(const path&);

// Create a directory, and all parent directories if these do not already exist.
// Return true if the directory was created or if it already existed, otherwise
// false to indicate an error. This function can handle absolute or relative
// path names. The file version creates the parent directory.
ORK_ORK_EXT(bool) ensure_directory(const path&);
ORK_ORK_EXT(bool) ensure_file(const path&);


#    define ORK_FILE_OPEN_ERR(MSG, FILE_NAME) \
        ORK_STMT(ORK_THROW(MSG << ORK("\n -- Path: \"") << FILE_NAME << ORK("\"")))

#    define ORK_FILE_READ_(FILE_NAME, STREAM_T) \
        ork::STREAM_T fin(FILE_NAME); \
        if(!fin) { \
            ORK_FILE_OPEN_ERR(ORK("Could not open input file!"), FILE_NAME) \
        }
#    define ORK_FILE_WRITE_(FILE_NAME, STREAM_T) \
        ork::STREAM_T fout(FILE_NAME); \
        if(!fout) { \
            ORK_FILE_OPEN_ERR(ORK("Could not open output file!"), FILE_NAME) \
        }
#    define ORK_FILE_PARSE_(FILE_NAME, STREAM_T) \
        ORK_FILE_READ_(FILE_NAME, STREAM_T) \
        fin.unsetf(std::ios::skipws);

#    define ORK_FILE_READ(FILE_NAME) ORK_FILE_READ_(FILE_NAME, if_stream)
#    define ORK_FILE_WRITE(FILE_NAME) ORK_FILE_WRITE_(FILE_NAME, of_stream)
#    define ORK_FILE_PARSE(FILE_NAME) ORK_FILE_PARSE_(FILE_NAME, if_stream)

#    define ORK_FILE_READ_B(FILE_NAME) ORK_FILE_READ_(FILE_NAME, bif_stream)
#    define ORK_FILE_WRITE_B(FILE_NAME) ORK_FILE_WRITE_(FILE_NAME, bof_stream)
#    define ORK_FILE_PARSE_B(FILE_NAME) ORK_FILE_PARSE_(FILE_NAME, bif_stream)

#    define ORK_FILE_READ_W(FILE_NAME) ORK_FILE_READ_(FILE_NAME, wif_stream)
#    define ORK_FILE_WRITE_W(FILE_NAME) ORK_FILE_WRITE_(FILE_NAME, wof_stream)
#    define ORK_FILE_PARSE_W(FILE_NAME) ORK_FILE_PARSE_(FILE_NAME, wif_stream)

} // namespace file
} // namespace ork

#endif // ORK_FILE_SYSTEM_HPP
