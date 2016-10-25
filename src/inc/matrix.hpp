#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <ostream>
#include <algorithm>
#include <type_traits>
#include <boost/range.hpp>
#include <initializer_list>
#include <boost/multi_array.hpp>

template<class T, size_t N>
class matrix : public boost::multi_array<T, N> {
    std::array<size_t, N> _sizes;
    typedef boost::multi_array<T, N> base;

public:
    typedef boost::multi_array_types::index index_t;
    typedef boost::multi_array_types::index_range range_t;
    typedef boost::multi_array_types::size_type size_type_t;

public:
    matrix()
    { }

    matrix(const std::array<size_t, N>& sizes)
        : _sizes(sizes)
    { this->resize(sizes); }

    std::array<size_t, N> size() const
    { return this->_sizes; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator=(const S& x)
    { std::for_each(this->data(), this->data() + this->num_elements(), [&x](auto& i) { i = x; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator=(const std::initializer_list<S> x)
    { size_t e = 0; std::all_of(x.begin(), x.end(), [&e, this](const auto& i) { if(e < this->num_elements()) { this->data()[e++] = i; return true; } return false; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator+=(const S& x)
    { std::for_each(this->data(), this->data() + this->num_elements(), [&x](auto& i) { i += x; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator-=(const S& x)
    { std::for_each(this->data(), this->data() + this->num_elements(), [&x](auto& i) { i -= x; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator*=(const S& x)
    { std::for_each(this->data(), this->data() + this->num_elements(), [&x](auto& i) { i *= x; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator/=(const S& x)
    { std::for_each(this->data(), this->data() + this->num_elements(), [&x](auto& i) { i /= x; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator+=(const matrix<S, N>& x)
    { size_t e = 0; std::for_each(this->data(), this->data() + this->num_elements(), [&e, &x](auto& i) { i += x.data()[e++]; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator-=(const matrix<S, N>& x)
    { size_t e = 0; std::for_each(this->data(), this->data() + this->num_elements(), [&e, &x](auto& i) { i -= x.data()[e++]; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator*=(const matrix<S, N>& x)
    { size_t e = 0; std::for_each(this->data(), this->data() + this->num_elements(), [&e, &x](auto& i) { i *= x.data()[e++]; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator/=(const matrix<S, N>& x)
    { size_t e = 0; std::for_each(this->data(), this->data() + this->num_elements(), [&e, &x](auto& i) { i /= x.data()[e++]; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix operator+(const matrix<S, N>& x) const
    { auto o = *this; o += x; return o; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix operator-(const matrix<S, N>& x) const
    { auto o = *this; o -= x; return o; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix operator*(const matrix<S, N>& x) const
    { auto o = *this; o *= x; return o; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix operator/(const matrix<S, N>& x) const
    { auto o = *this; o /= x; return o; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix operator+(const S& x) const
    { auto o = *this; o += x; return o; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix operator-(const S& x) const
    { auto o = *this; o -= x; return o; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix operator*(const S& x) const
    { auto o = *this; o *= x; return o; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix operator/(const S& x) const
    { auto o = *this; o /= x; return o; }

    inline T min() const
    { T x = *this->data(); std::for_each(this->data(), this->data() + this->num_elements(), [&x](const auto& i) { if(i < x) x = i; }); return x; }

    inline T max() const
    { T x = *this->data(); std::for_each(this->data(), this->data() + this->num_elements(), [&x](const auto& i) { if(i > x) x = i; }); return x; }

    inline matrix<T, N>& abs() const
    { std::for_each(this->data(), this->data() + this->num_elements(), [](const auto& i) { i = ((i < 0) ? i = i * -1 : i); }); return *this; }

    inline T norm2() const
    { T x = T(); std::for_each(this->data(), this->data() + this->num_elements(), [&x](const auto& i) { x += i * i; }); return x; }

    inline matrix<double, N> normalize() const
    { return this->as<double>() / std::accumulate(this->begin(), this->end(), 0.0); }

    template<typename S, typename = typename std::enable_if<std::is_convertible<T, S>::value>::type>
    inline matrix<S, N> as() const
    { size_t e = 0; matrix<S, N> out(this->_sizes); std::for_each(this->data(), this->data() + this->num_elements(), [&out, &e](const auto& i) { out.data()[e++] = static_cast<S>(i); }); return out; }

    friend std::ostream& operator << (std::ostream& os, const matrix<T, N>& m)
    { std::for_each(m.data(), m.data() + m.num_elements(), [&os](const auto& i) { os << i << " "; }); return os; }

    inline typename base::element& operator()(const std::array<size_t, N>& l)
    { return base::operator ()(l); }

    inline typename base::element operator()(const std::array<size_t, N>& l) const
    { return base::operator ()(l); }
};

template<class T, size_t N>
using matrixND_t = matrix<T, N>;

template<class T>
using matrix1D_t = matrixND_t<T, 1>;

template<class T>
using matrix2D_t = matrixND_t<T, 2>;

template<class T>
using matrix3D_t = matrixND_t<T, 3>;

template<class T>
using matrix4D_t = matrixND_t<T, 4>;

using boost::indices;
typedef boost::multi_array_types::index_range range;

#endif // MATRIX_HPP
