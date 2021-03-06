/*
 * This file is part of Susa.

 * Susa is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * Susa is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.

 * You should have received a copy of the Lesser GNU General Public License
 * along with Susa.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file signal.h
 * @brief Signal processing (declaration and definiton).
 * @author Behrooz Kamary
 * @version 1.0.0
 *
 * @defgroup Signal Signal Processing
 *
 * @todo <i>filter</i> and <i>conv</i> do not support all input matrices' sizes. it looks complicated to solve.
 */

#ifndef SIGNAL_H
#define SIGNAL_H

namespace susa {

/**
 * @brief Upsample
 *
 * @param mat_arg Input matrix
 * @param uint_u Upsampling rate
 * @return Upsampled matrix
 *
 * @ingroup Signal
 */
template <class T> matrix <T> upsample(const matrix <T> &mat_arg, size_t uint_u);

/**
 * @brief Downsample
 *
 * @param mat_arg Input matrix
 * @param uint_d Downsampling rate
 * @return Downsampled matrix
 *
 * @ingroup Signal
 */
template <class T> matrix <T> downsample(const matrix <T> &mat_arg, size_t uint_d);

/**
 * @brief Filter
 *
 * y(n) = b(1)*x(n) + b(2)*x(n-1) + ... + b(nb+1)*x(n-nb) - a(2)*y(n-1) - ... - a(na+1)*y(n-na)
 *
 * @param mat_arg_b Moving Average (MA) coefficients
 * @param mat_arg_a Autoregressive (AR) coefficients.
 * @param mat_arg_x Input data to be filtered.
 * @param size_length Additional tail of zeros that can be appended to the input.
 * @return filtered matrix
 *
 * @ingroup Signal
 */
template <class T, class TT> matrix <T> filter(const matrix <TT>& mat_arg_b, const matrix <TT>& mat_arg_a, const matrix <T>& mat_arg_x, size_t size_length = 0);

/**
 * @brief Convolution
 *
 * @param mat_arg_a
 * @param mat_arg_b
 * @return convolution of the two input vectors
 *
 * @ingroup Signal
 */
template <class T> matrix <T> conv(const matrix <T>& mat_arg_a, const matrix <T>& mat_arg_b);

/**
 * @brief Constructs a convolution matrix from the impulse response <i>mat_arg</i>
 *
 * @param mat_arg input vector (impulse response)
 * @param  size_len length of the impulse response i.e. memory length
 * @return convolution matrix
 *
 * @ingroup Signal
 */
template <class T> matrix <T> convmtx(const matrix <T>& mat_arg, size_t size_len);

/**
 * @brief Constructs a toeplitz matrix from input column and row vectors
 *
 * @param mat_col input column vector
 * @param mat_col input row vector
 * @return toeplitz matrix
 *
 * @ingroup Signal
 */
template <class T> matrix <T> toeplitz(const matrix <T>& mat_col, const matrix <T>& mat_row);

/**
 * @brief Constructs a toeplitz matrix from an input column vector
 *
 * @param mat_col input column vector
 * @return The toeplitz matrix
 *
 * @ingroup Signal
 */
template <class T> matrix <T> toeplitz(const matrix <T>& mat_col);

// Implementation

// UPSAMPLE

template <class T> matrix <T> upsample(const matrix <T> &mat_arg, size_t uint_u)
{
    matrix <T> mat_tmp(uint_u * mat_arg.no_rows(), mat_arg.no_cols());

    for (size_t size_col = 0; size_col < mat_arg.no_cols(); size_col++)
    {
        for (size_t uint_row = 0; uint_row < mat_arg.no_rows(); uint_row++)
        {
            mat_tmp(uint_row * uint_u,size_col) = mat_arg(uint_row, size_col);
        }
    }
    return mat_tmp;
}

// DOWNSAMPLE

template <class T> matrix <T> downsample(const matrix <T> &mat_arg, size_t uint_d)
{
    matrix <T> mat_tmp(mat_arg.no_rows()/uint_d,mat_arg.no_cols());

    for (size_t size_col = 0; size_col < mat_arg.no_cols(); size_col++)
    {
        for (size_t uint_row = 0; uint_row < mat_arg.no_rows()/uint_d; uint_row++)
        {
            mat_tmp(uint_row, size_col) = mat_arg(uint_row * uint_d, size_col);
        }
    }
    return mat_tmp;
}

// FILTER

template <class T, class TT> matrix <T> filter( const matrix <TT>& mat_arg_b, const matrix <TT>& mat_arg_a, const matrix <T>& mat_arg_x, size_t size_length)
{

    size_t      size_ret_len = 0;
    matrix <T>  mat_y;

    size_t size_b       = mat_arg_b.size();
    size_t size_a       = mat_arg_a.size();
    size_t size_x_rows  = mat_arg_x.no_rows();
    size_t size_x_cols  = mat_arg_x.no_cols();
    size_t size_x       = mat_arg_x.size();

    T x;
    T y;

    if (size_x_cols > 1 && size_x_rows > 1)
    {
        size_ret_len = size_x_rows + size_length;
        mat_y = matrix <T> (size_ret_len, size_x_cols, 0);
        size_t size_y_rows = mat_y.no_rows();

        for (size_t size_col = 0; size_col < size_x_cols; size_col++)
        {
            for (size_t i = 0 ; i < size_ret_len; i++)
            {
                for (size_t k = 0; k < size_b; k++)
                {
                    x = (i < (size_x_rows + k) && ((i + 1) > k)) ? mat_arg_x(i - k, size_col) : 0;
                    mat_y(i, size_col) += x * mat_arg_b(k);
                }
                for (size_t k = 1; k < size_a; k++)
                {
                    y = (i < (size_y_rows + k) && ((i + 1) > k)) ? mat_y(i - k, size_col) : 0;
                    mat_y(i, size_col) -= y * mat_arg_a(k);
                }
            }
        }
    }
    else if (size_x_cols == 1 && size_x_rows > 1)
    {
        // for column vector
        size_ret_len = size_x + size_length;
        mat_y = matrix <T> (size_ret_len, 1, 0);
        size_t size_y      = mat_y.size();

        for (size_t i = 0; i< size_ret_len; i++)
        {
            for (size_t k = 0; k < size_b; k++)
            {
                x = (i < (size_x + k) && ((i + 1) > k)) ? mat_arg_x(i - k) : 0;
                mat_y(i) += x * mat_arg_b(k);
            }
            for (size_t k = 1; k < size_a; k++)
            {
                y = (i < (size_y + k) && ((i + 1) > k)) ? mat_y(i - k) : 0;
                mat_y(i) -= y * mat_arg_a(k);
            }
        }
    }
    else if (size_x_cols > 1 && size_x_rows == 1)
    {
        // for row vector
        size_ret_len = mat_arg_x.size() + size_length;
        mat_y = matrix <T> (1, size_ret_len, 0);
        size_t size_y      = mat_y.size();

        for (size_t i = 0 ; i < size_ret_len; i++)
        {
            for (size_t k = 0; k < size_b; k++)
            {
                x = (i < (size_x + k) && ((i + 1) > k)) ? mat_arg_x(i - k) : 0;
                mat_y(i) += x * mat_arg_b(k);
            }
            for (size_t k = 1; k < size_a; k++)
            {
                y = (i < (size_y + k) && ((i + 1) > k)) ? mat_y(i - k) : 0;
                mat_y(i) -= y * mat_arg_a(k);
            }
        }
    }
    return mat_y;
}


template <class T> matrix <T> conv(const matrix <T>& mat_arg_a, const matrix <T>& mat_arg_b)
{

    matrix <T>  mat_ret;
    size_t      size_length;

    if ((mat_arg_b.no_rows() > 1 && mat_arg_b.no_cols() == 1) || (mat_arg_b.no_cols() > 1 && mat_arg_b.no_rows() == 1))
    { // mat_arg_b is a vector

        size_length = mat_arg_b.size() - 1;
        mat_ret = filter(mat_arg_b, matrix <T> (1,1,1), mat_arg_a, size_length);

    } else if ((mat_arg_a.no_rows() > 1 && mat_arg_a.no_cols() == 1) || (mat_arg_a.no_cols() > 1 && mat_arg_a.no_rows() == 1))
    { // mat_arg_a is a vector

        size_length = mat_arg_a.size() - 1;
        mat_ret = filter(mat_arg_a, matrix <T> (1,1,1), mat_arg_b, size_length);

    } else if (mat_arg_b.no_cols() == 1 && mat_arg_b.no_rows() == 1)
    {
        mat_ret = mat_arg_b(0) * mat_arg_a;
    } else if (mat_arg_a.no_cols() == 1 && mat_arg_a.no_rows() == 1)
    {
        mat_ret = mat_arg_a(0) * mat_arg_b;
    } else if ((mat_arg_b.no_cols() > 1 && mat_arg_b.no_rows() > 1) && (mat_arg_a.no_cols() > 1 && mat_arg_a.no_rows() > 1) && (mat_arg_a.no_cols() == mat_arg_b.no_cols()))
    {
        SUSA_ASSERT_MESSAGE(false, "unsupported codition.");
        for (size_t size_col = 0; size_col < mat_arg_a.no_cols(); size_col++)
        {
            size_length = mat_arg_a.no_rows() - 1;
            // TODO: set_row and set_col missing in susa::matrix
            // 2009-02-22
            // I need a [set_row/set_col] to complete this part
        }
    }
    else
    {
        SUSA_ASSERT_MESSAGE(false, "unsupported codition.");
    }

    return mat_ret;
}

template <class T> matrix <T> convmtx(const matrix <T> &mat_arg, size_t size_len)
{
    SUSA_ASSERT_MESSAGE(mat_arg.is_vector(), "the input argument is not a vector");
    
    size_t      size_m = mat_arg.size();
    matrix <T>  mat_ret;
    
    if (mat_arg.no_cols() == 1 && mat_arg.no_rows() >= 1)
    { // It is a column vector
        mat_ret = matrix <T> (size_m + size_len - 1, size_len, 0);
        for (size_t size_col = 0; size_col < size_len; size_col++)
        {
            for (size_t uint_row = 0; uint_row < size_col; uint_row++)
            {
                mat_ret(uint_row,size_col) = 0;
            }

            for (size_t uint_row = size_col; uint_row < size_m + size_col; uint_row++)
            {
                mat_ret(uint_row,size_col) = mat_arg(uint_row - size_col);
            }

            for (size_t uint_row = size_m + size_col ; uint_row < size_m + size_len - 1; uint_row++)
            {
                mat_ret(uint_row,size_col) = 0;
            }
        }
    }
    else if (mat_arg.no_rows() == 1 && mat_arg.no_cols() >= 1)
    { // It is a row vector
        mat_ret = matrix <T> (size_len, size_m + size_len - 1, 0);
        for (size_t uint_row = 0; uint_row < size_len; uint_row++)
        {
            for (size_t size_col = 0; size_col < uint_row; size_col++)
            {
                mat_ret(uint_row,size_col) = 0;
            }

            for (size_t size_col = uint_row; size_col < size_m + uint_row; size_col++)
            {
                mat_ret(uint_row,size_col) = mat_arg(size_col - uint_row);
            }

            for (size_t size_col = size_m + uint_row ; size_col < size_m + size_len - 1; size_col++)
            {
                mat_ret(uint_row,size_col) = 0;
            }
        }
    }

    return mat_ret;
}

template <class T> matrix <T> toeplitz(const matrix <T>& mat_col, const matrix <T>& mat_row)
{
    size_t size_cols = mat_col.size();
    size_t size_rows = mat_row.size();

    matrix <T> mat_ret(size_cols, size_rows);

    for ( size_t uint_row = 0; uint_row < size_rows; uint_row++)
    {
        for ( size_t size_col = 0; size_col < size_cols; size_col++)
        {
            mat_ret(uint_row, size_col) = mat_col(std::abs((long int)(size_col - uint_row)));
        }
    }
    return mat_ret;
}

template <class T> matrix <T> toeplitz(const matrix <T>& mat_col)
{
    size_t size_len = mat_col.size();

    matrix <T> mat_ret(size_len, size_len);

    for ( size_t uint_row = 0; uint_row < size_len; uint_row++)
    {
        for ( size_t size_col = 0; size_col < size_len; size_col++)
        {
            mat_ret(uint_row, size_col) = mat_col(std::abs((long int)(size_col - uint_row)));
        }
    }
    return mat_ret;
}

} // NAMESPACE SUSA
#endif // SIGNAL_H
