// No include guard!

#if ORK_MSC
#    pragma warning(push)
#    pragma warning(disable : 4061) // enum val not explicitly handled by a case label
#    pragma warning(disable : 4100) // unreferenced formal parameter
#    pragma warning(disable : 4127) // conditional expression is constant
#    pragma warning(disable : 4365) // return' signed/unsigned mismatch
#    pragma warning(disable : 4371) // layout changed
#    pragma warning(disable : 4555) // expression has no effect
#    pragma warning(disable : 4459) // declaration hides global declaration
#    pragma warning(disable : 4503) // decorated name length exceeded, name was truncated
#    pragma warning(disable : 4571) // catch(...) semantics changed for structured exceptions
#    pragma warning(disable : 4619) // disable non-existant warning
#    pragma warning(disable : 4623) // default constructor was implicitly defined as deleted
#    pragma warning(disable : 4625) // implicit copy constructor
#    pragma warning(disable : 4626) // implicit copy assignment
#    pragma warning(disable : 4628) // digraphs not supported with - Ze
#    pragma warning(disable : 4668) // undefined macro converted to 0
#    pragma warning(disable : 4714) // function marked as __forceinline not inlined
#    pragma warning(disable : 4774) // format string argument is not a string literal
#    pragma warning(disable : 4996) // deprecated
#    pragma warning(disable : 5026) // implicit move constructor
#    pragma warning(disable : 5027) // implicit move assignment
#    pragma warning(disable : 5031) // unmatched pragma push in header
#endif

#include ORK_BOOST_INC_FILE
#undef ORK_BOOST_INC_FILE

#if ORK_MSC
#    pragma warning(pop)
#endif
