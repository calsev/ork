//No include guard!

#if ORK_MSC
#pragma warning(push)
#pragma warning(disable:4365) //return' signed/unsigned mismatch
#pragma warning(disable:4371) //layout changed
#pragma warning(disable:4571) //catch(...) semantics changed for structured exceptions
#pragma warning(disable:4619) //disable non-existant warning
#pragma warning(disable:4625) //implicit copy constructor
#pragma warning(disable:4626) //implicit copy assignment
#pragma warning(disable:4668) //undefined macro converted to 0
#pragma warning(disable:4774) //format string argument is not a string literal
#pragma warning(disable:5026) //implicit move constructor
#pragma warning(disable:5027) //implicit move assignment
#pragma warning(disable:5031) //unmatched pragma push in header
#endif

#include ORK_BOOST_INC_FILE
#undef ORK_BOOST_INC_FILE

#if ORK_MSC
#pragma warning(pop)
#endif
