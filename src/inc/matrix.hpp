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
    { this->for_each([&x](auto& i) { i = x; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator=(const std::initializer_list<S> x)
    { size_t e = 0; std::all_of(x.begin(), x.end(), [&e, this](const auto& i) { if(e < this->num_elements()) { this->data()[e++] = i; } return false; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator+=(const S& x)
    { this->for_each([&x](auto& i) { i += x; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator-=(const S& x)
    { this->for_each([&x](auto& i) { i -= x; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator*=(const S& x)
    { this->for_each([&x](auto& i) { i *= x; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator/=(const S& x)
    { this->for_each([&x](auto& i) { i /= x; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator+=(const matrix<S, N>& x)
    { size_t e = 0; this->for_each([&e, &x](auto& i) { i += x.data()[e++]; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator-=(const matrix<S, N>& x)
    { size_t e = 0; this->for_each([&e, &x](auto& i) { i -= x.data()[e++]; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator*=(const matrix<S, N>& x)
    { size_t e = 0; this->for_each([&e, &x](auto& i) { i *= x.data()[e++]; }); return *this; }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline matrix& operator/=(const matrix<S, N>& x)
    { size_t e = 0; this->for_each([&e, &x](auto& i) { i /= x.data()[e++]; }); return *this; }

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

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline bool operator==(const S& x) const
    { size_t e = 0; this->for_each([&x, &e](const auto& i) { if(i == static_cast<T>(x) && ++e) return true; return false; }); return e == this->num_elements(); }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline bool operator!=(const S& x) const
    { return !this->operator ==(x); }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline bool operator<(const S& x) const
    { size_t e = 0; this->all_of([&x, &e](const auto& i) { if(i < static_cast<T>(x) && ++e) return true; return false; }); return e == this->num_elements(); }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline bool operator>(const S& x) const
    { size_t e = 0; this->all_of([&x, &e](const auto& i) { if(i > static_cast<T>(x) && ++e) return true; return false; }); return e == this->num_elements(); }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline bool operator<=(const S& x) const
    { return !this->operator >(x); }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline bool operator>=(const S& x) const
    { return !this->operator <(x); }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline bool operator==(const matrix<S, N>& x) const
    { size_t e = 0; this->all_of([&x, &e](const auto& i) { if(i == static_cast<T>(x.data()[e]) && ++e) return true; return false; }); return e == this->num_elements();}

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline bool operator!=(const matrix<S, N>& x) const
    { return !this->operator ==(x); }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline bool operator<(const matrix<S, N>& x) const
    { size_t e = 0; this->all_of([&x, &e](const auto& i) { if(i < static_cast<T>(x.data()[e]) && ++e) return true; return false; }); return e == this->num_elements();}

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline bool operator>(const matrix<S, N>& x) const
    { size_t e = 0; this->all_of([&x, &e](const auto& i) { if(i > static_cast<T>(x.data()[e]) && ++e) return true; return false; }); return e == this->num_elements();}

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline bool operator<=(const matrix<S, N>& x) const
    { return !this->operator >(x); }

    template<typename S, typename = typename std::enable_if<std::is_convertible<S, T>::value>::type>
    inline bool operator>=(const matrix<S, N>& x) const
    { return !this->operator <(x); }

    inline matrix<double, N> normalize() const
    { return this->as<double>() / std::accumulate(this->begin(), this->end(), 0.0); }

    template<typename S, typename = typename std::enable_if<std::is_convertible<T, S>::value>::type>
    inline matrix<S, N> as() const
    { size_t e = 0; matrix<S, N> out(this->_sizes); this->for_each([&out, &e](const auto& i) { out.data()[e++] = static_cast<S>(i); }); return out; }

    friend std::ostream& operator << (std::ostream& os, const matrix<T, N>& m)
    { m.for_each([&os](const auto& i) { os << i << " "; }); return os; }

    inline typename base::element& operator()(const std::array<size_t, N>& l)
    { return base::operator ()(l); }

    inline typename base::element operator()(const std::array<size_t, N>& l) const
    { return base::operator ()(l); }

    template<typename callback>
    inline void for_each(callback c) { for(size_t i = 0; i < this->num_elements(); i++) c(this->data()[i]); }

    template<typename callback>
    inline void for_each(callback c) const { for(size_t i = 0; i < this->num_elements(); i++) c(this->data()[i]); }

    template<typename callback>
    inline void all_of(callback c) { for(size_t i = 0; i < this->num_elements(); i++) if(!c(this->data()[i])) break; }

    template<typename callback>
    inline void all_of(callback c) const { for(size_t i = 0; i < this->num_elements(); i++) if(!c(this->data()[i])) break; }

    inline T min() const
    { T x = *this->data(); this->for_each([&x](const auto& i) { if(i < x) x = i; }); return x; }

    inline T max() const
    { T x = *this->data(); this->for_each([&x](const auto& i) { if(i > x) x = i; }); return x; }

    inline matrix<T, N> abs() const
    { matrix<T, N> out = *this; out.for_each([](auto& i) { i = ((i < 0) ? i * -1 : i); }); return out; }

    inline T norm2() const
    { T x = T(); this->for_each([&x](const auto& i) { x += i * i; }); return x; }

    template<size_t S>
    matrix<T, N - S> slice(const std::array<size_t, S>& indices) const {
        size_t start = 0;
        std::array<size_t, N - S> dim;
        for(size_t i = S, k = 0; i < N; i++, k++)
        { dim[k] = this->shape()[i]; }
        matrix<T, N - S> out(dim);
        for(size_t i = 0; i < S; i++) {
            BOOST_ASSERT_MSG(indices[i] < this->shape()[i], "index overflow!");
            start += indices[i] * this->strides()[i];
        }
        out.for_each([&](auto& i) { i = this->data()[start++]; });
        return out;
    }

    template<size_t S>
    matrix<T*, N - S> slice_ref(const std::array<size_t, S>& indices) {
        size_t start = 0;
        std::array<size_t, N - S> dim;
        for(size_t i = S, k = 0; i < N; i++, k++)
        { dim[k] = this->shape()[i]; }
        matrix<T*, N - S> out(dim);
        for(size_t i = 0; i < S; i++) {
            BOOST_ASSERT_MSG(indices[i] < this->shape()[i], "index overflow!");
            start += indices[i] * this->strides()[i];
        }
        out.for_each([&](auto& i) { i = &(this->data()[start++]); });
        return out;
    }
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

template<size_t T>
using slice = array<size_t, T>;

using boost::indices;
typedef boost::multi_array_types::index_range range;

#endif // MATRIX_HPP
