/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include "ork/ork.hpp"

#if ORK_USE_GLM
#    include "glm/vec3.hpp"
#    include "glm/geometric.hpp"


namespace ork {

enum class orientation;

namespace GLM { // Kind of deceiving, but whatever


using glm_size_t = glm::dvec3::length_type;


struct dunit3 {
private:
    glm::dvec3 _vec;

public:
    ORK_INLINE explicit dunit3(const glm::dvec3& vector)
        : _vec(glm::normalize(vector))
    {}
    ORK_INLINE dunit3(const double x, const double y, const double z)
        : _vec(glm::normalize(glm::dvec3(x, y, z)))
    {}

public:
    ORK_INLINE const glm::dvec3& get() const
    {
        return _vec;
    }
    ORK_INLINE double& operator[](const glm_size_t index)
    {
        return _vec[index];
    }
    ORK_INLINE const double& operator[](const glm_size_t index) const
    {
        return _vec[index];
    }
    ORK_INLINE double x() const
    {
        return _vec.x;
    }
    ORK_INLINE double y() const
    {
        return _vec.y;
    }
    ORK_INLINE double z() const
    {
        return _vec.z;
    }
    ORK_INLINE dunit3 operator-() const
    {
        return dunit3(-_vec);
    }
    ORK_INLINE glm_size_t size() const
    {
        return 3;
    }
    ORK_INLINE glm_size_t length() const
    {
        return 3;
    }
};

ORK_INLINE glm::dvec3 operator*(const double lhs, const dunit3& rhs)
{
    return lhs * rhs.get();
}
ORK_ORK_EXT(o_stream&) operator<<(o_stream& stream, const dunit3& vec);


// Simply tired of defining these
ORK_ORK_EXTERN const dunit3 pos_x;
ORK_ORK_EXTERN const dunit3 neg_x;
ORK_ORK_EXTERN const dunit3 pos_y;
ORK_ORK_EXTERN const dunit3 neg_y;
ORK_ORK_EXTERN const dunit3 pos_z;
ORK_ORK_EXTERN const dunit3 neg_z;


    // glm version of loops, needed because glm types use length()
#    define LOOPVG(SERIES, INDEX) \
        for(ork::GLM::glm_size_t INDEX = 0, limit##INDEX = SERIES.length(); \
            INDEX != limit##INDEX; \
            ++INDEX)
#    define LOOPVIG(SERIES) LOOPVG(SERIES, i)
#    define LOOPVJG(SERIES) LOOPVG(SERIES, j)
#    define LOOPVKG(SERIES) LOOPVG(SERIES, k)
#    define LOOPVLG(SERIES) LOOPVG(SERIES, l)


/*
This function basically tests that two floats could have resulted from a single, mathematically equivalent operation
float f1=0.2;//Cannot be exactly represented! (One bit either way)
float f2=0.1;//ditto
Float f2+=0.1;//Maybe 2 bits different now(both bits)
assert(f1==f2);//have fun crashing
assert(might_be_equal(f1,f2));//We are safe, assuming IEEE 754 (Ok,I am not authoritatively positive)
There are false positives with small numbers!
*/
template<typename T>
struct epsilon_type;
template<>
struct epsilon_type<float> {
    using type = uint32_t;
};
template<>
struct epsilon_type<double> {
    using type = uint64_t;
};


/*
Precision is for modeling, for kernels that take such an argument (OCC)
Tolerance is for checking.
*/
template<typename T>
ORK_INLINE ORK_CONSTEXPR T precision()
{
#    if ORK_USE_ACIS
    return (static_cast<epsilon_type<double>::type>(0x1) << 4) *
           std::numeric_limits<T>::epsilon();
#    elif ORK_USE_OCC
    return (static_cast<epsilon_type<double>::type>(0x1) << 10) *
           std::numeric_limits<T>::epsilon();
#    else
    return (static_cast<epsilon_type<double>::type>(0x1) << 8) *
           std::numeric_limits<T>::epsilon();
#    endif
}


template<typename T>
struct default_epsilon_factor;
template<>
struct default_epsilon_factor<float> {
    static const epsilon_type<float>::type value =
        static_cast<epsilon_type<float>::type>(0x1) << 4; // A guesstimate
};


template<>
struct default_epsilon_factor<double> {
#    if ORK_USE_ACIS
    static const epsilon_type<double>::type value =
        static_cast<epsilon_type<double>::type>(0x1)
        << 4; // This is only verified over time as the minimum upper bound across ACIS when T is double
#    else // OCC
    static const epsilon_type<double>::type value =
        static_cast<epsilon_type<double>::type>(0x1)
        << 28; // This is only verified over time as the minimum upper bound across OCC when T is double
#    endif
};


template<typename T, typename epsilon_type<T>::type eps_factor = default_epsilon_factor<T>::value>
ORK_INLINE ORK_CONSTEXPR T tolerance()
{
    return eps_factor * std::numeric_limits<T>::epsilon();
}


namespace detail {

template<
    typename T,
    typename epsilon_type<T>::type eps_factor = default_epsilon_factor<T>::value,
    typename epsilon_type<T>::type rel_factor = static_cast<epsilon_type<T>::type>(1)>
ORK_INLINE bool equal_simple(const T& lhs, const T& rhs)
{
    static const T abs_eps = tolerance<T, eps_factor>(); // We need an absolute epsilon
    static const T rel_eps =
        rel_factor * abs_eps; // Factor of 2 to allow for the case of second LSB bump
    return std::abs(lhs - rhs) <= std::max(abs_eps, rel_eps * std::max(lhs, rhs));
}

template<class T>
ORK_INLINE bool equal_vector(const T& lhs, const T& rhs)
{
    LOOPVIG(lhs)
    {
        if(!equal_simple(lhs[i], rhs[i]))
            return false;
    }
    return true;
}

template<class T>
ORK_INLINE bool equal_matrix(const T& lhs, const T& rhs)
{
    LOOPVIG(lhs)
    {
        if(!equal_vector(lhs[i], rhs[i]))
            return false;
    }
    return true;
}


template<typename T>
ORK_INLINE bool less_simple(const T& lhs, const T& rhs)
{
    return lhs < rhs && !equal_simple<T>(lhs, rhs);
}

template<typename T>
ORK_INLINE bool greater_simple(const T& lhs, const T& rhs)
{
    return lhs > rhs && !equal_simple<T>(lhs, rhs);
}

template<typename T>
ORK_INLINE bool less_equal_simple(const T& lhs, const T& rhs)
{
    return lhs < rhs || equal_simple<T>(lhs, rhs);
}

template<typename T>
ORK_INLINE bool greater_equal_simple(const T& lhs, const T& rhs)
{
    return lhs > rhs || equal_simple<T>(lhs, rhs);
}


} // namespace detail


// Use matrix as the default because there are more of them
template<typename T>
ORK_INLINE bool equal(const T& lhs, const T& rhs)
{
    return detail::equal_matrix<T>(lhs, rhs);
}

// And we have not bothered to add other overloads
template<typename V, glm::precision P>
ORK_INLINE bool equal(const glm::tvec2<V, P>& lhs, const glm::tvec2<V, P>& rhs)
{
    return detail::equal_vector<glm::tvec2<V, P>>(lhs, rhs);
}
template<typename V, glm::precision P>
ORK_INLINE bool equal(const glm::tvec3<V, P>& lhs, const glm::tvec3<V, P>& rhs)
{
    return detail::equal_vector<glm::tvec3<V, P>>(lhs, rhs);
}
template<typename V, glm::precision P>
ORK_INLINE bool equal(const glm::tvec4<V, P>& lhs, const glm::tvec4<V, P>& rhs)
{
    return detail::equal_vector<glm::tvec4<V, P>>(lhs, rhs);
}
ORK_INLINE bool equal(const GLM::dunit3& lhs, const GLM::dunit3& rhs)
{
    return detail::equal_vector<GLM::dunit3>(lhs, rhs);
}


template<>
ORK_INLINE bool equal<float>(const float& lhs, const float& rhs)
{
    return detail::equal_simple<float>(lhs, rhs);
}
template<>
ORK_INLINE bool equal<double>(const double& lhs, const double& rhs)
{
    return detail::equal_simple<double>(lhs, rhs);
}


template<class V>
ORK_INLINE bool parallel(const V& v1, const V& v2)
{
    const double norms = glm::length(v1) * glm::length(v2);
    const double dot = glm::dot(v1, v2);
    return equal(norms, dot);
}
template<>
ORK_INLINE bool parallel<dunit3>(const dunit3& v1, const dunit3& v2)
{
    const double norms = glm::length(v1.get()) * glm::length(v2.get());
    const double dot = glm::dot(v1.get(), v2.get());
    return equal(norms, dot);
}


template<class V>
ORK_INLINE bool antiparallel(const V& v1, const V& v2)
{
    return parallel<V>(v1, -v2);
}


template<class V>
ORK_INLINE bool axial(const V& v1, const V& v2)
{
    return parallel<V>(v1, v2) || parallel<V>(v1, -v2);
}
template<class V>
ORK_INLINE bool collinear(const V& v1, const V& v2)
{ // Just an alias
    return parallel<V>(v1, v2) || parallel<V>(v1, -v2);
}


template<class V>
ORK_INLINE bool orthogonal(const V& v1, const V& v2)
{
    const double norms = glm::length(v1) * glm::length(v2);
    const double dot = glm::dot(v1, v2);
    const bool test = equal(norms, norms + dot);
    return test;
}
template<>
ORK_INLINE bool orthogonal<dunit3>(const dunit3& v1, const dunit3& v2)
{
    return orthogonal(v1.get(), v2.get());
}


/*
Some vector functions
*/


template<class V>
ORK_INLINE bool less(const V& v1, const V& v2)
{
    LOOPVIG(v1)
    {
        if(equal(v1[i], v2[i]) || v1[i] > v2[i])
            return false;
    }
    return true;
}
template<>
ORK_INLINE bool less<float>(const float& lhs, const float& rhs)
{
    return detail::less_simple<float>(lhs, rhs);
}
template<>
ORK_INLINE bool less<double>(const double& lhs, const double& rhs)
{
    return detail::less_simple<double>(lhs, rhs);
}


template<class V>
ORK_INLINE bool greater(const V& v1, const V& v2)
{
    LOOPVIG(v1)
    {
        if(equal(v1[i], v2[i]) || v1[i] < v2[i])
            return false;
    }
    return true;
}
template<>
ORK_INLINE bool greater<float>(const float& lhs, const float& rhs)
{
    return detail::greater_simple<float>(lhs, rhs);
}
template<>
ORK_INLINE bool greater<double>(const double& lhs, const double& rhs)
{
    return detail::greater_simple<double>(lhs, rhs);
}


template<class V>
ORK_INLINE bool less_equal(const V& v1, const V& v2)
{
    LOOPVIG(v1)
    {
        if(greater(v1[i], v2[i]))
            return false;
    }
    return true;
}
template<>
ORK_INLINE bool less_equal<float>(const float& lhs, const float& rhs)
{
    return detail::less_equal_simple<float>(lhs, rhs);
}
template<>
ORK_INLINE bool less_equal<double>(const double& lhs, const double& rhs)
{
    return detail::less_equal_simple<double>(lhs, rhs);
}


template<class V>
ORK_INLINE bool greater_equal(const V& v1, const V& v2)
{
    LOOPVIG(v1)
    {
        if(less(v1[i], v2[i]))
            return false;
    }
    return true;
}
template<>
ORK_INLINE bool greater_equal<float>(const float& lhs, const float& rhs)
{
    return detail::greater_equal_simple<float>(lhs, rhs);
}
template<>
ORK_INLINE bool greater_equal<double>(const double& lhs, const double& rhs)
{
    return detail::greater_equal_simple<double>(lhs, rhs);
}


// Rotates a vector about the origin, such that normal would become new_normal if subject to the same rotation
ORK_ORK_EXT(glm::dvec3)
rotate(const glm::dvec3& vec, const dunit3& normal, const dunit3& new_normal);


ORK_ORK_EXT(glm::dvec3)
proj_on_plane(const glm::dvec3& vec, const GLM::dunit3& normal);


/*
Orientation stuff
*/
ORK_ORK_EXT(const dunit3&) orientation2direction(const orientation axis);


} // namespace GLM

ORK_ORK_EXT(string) to_string(const glm::dvec3& vec);
ORK_ORK_EXT(string) to_string(const GLM::dunit3& vec);

} // namespace ork

namespace glm {
ORK_ORK_EXT(ork::o_stream&)
operator<<(ork::o_stream& stream, const glm::dvec2& vec);
ORK_ORK_EXT(ork::o_stream&)
operator<<(ork::o_stream& stream, const glm::dvec3& vec);
} // namespace glm

#endif // ORK_USE_GLM
