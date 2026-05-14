#pragma once
#include "Tuple.h"
#include "Constants.h"
#include <cassert>

namespace Math
{
    template <std::size_t Width, std::size_t Height>
    class Matrix
    {
    public:
        // Constructors
        Matrix() = default;
        Matrix(std::initializer_list<float> values)
        {
            if (values.size() != Width * Height)
            {
                throw std::invalid_argument("Wrong number of values");
            }

            int i = 0;
            for (float v : values)
            {
                m_data[i++] = v;
            }
        }

        // Raw data access
        float *Data() { return m_data; }
        const float *Data() const { return m_data; }

        // Member access
        float *operator[](std::size_t col) { return &m_data[col * Height]; }
        const float *operator[](std::size_t col) const { return &m_data[col * Height]; }

        // Arithmatic
        template <std::size_t OtherWidth>
        Matrix<OtherWidth, Height> operator*(const Matrix<OtherWidth, Width> &other) const;
        Matrix<Width, Height> operator*(float scalar) const;
        Matrix<Width, Height> &operator*=(float scalar);

        // Comparison
        bool operator==(const Matrix<Width, Height> &other) const;
        bool operator!=(const Matrix<Width, Height> &other) const { return !(*this == other); }

        // Matrix Operations
        Matrix<Height, Width> Transpose() const;
        Matrix<Width, Height> Inverse() const;
        Matrix<Width - 1, Height - 1> SubMatrix(int rowRemoved, int colRemoved) const;
        float Determinant() const;
        float Minor(int row, int col) const;
        float Cofactor(int row, int col) const;
        bool IsInvertible() const { return std::abs(Determinant()) > EPSILON; }

        // Stream Output
        friend std::ostream &operator<<(std::ostream &os, const Matrix<Width, Height> &m)
        {
            os << "\n";

            for (int i = 0; i < Width * Height; i++)
            {
                os << m.m_data[i] << " ";
                if (i % Width == Width - 1)
                {
                    os << "\n";
                }
                else
                {
                    os << "| ";
                }
            }
            return os;
        }

    private:
        float m_data[Width * Height] = {};
    };

    // =============================================================================
    // Template definitions
    // =============================================================================

    template <std::size_t Width, std::size_t Height>
    template <std::size_t OtherWidth>
    inline Matrix<OtherWidth, Height> Matrix<Width, Height>::operator*(const Matrix<OtherWidth, Width> &other) const
    {
        Matrix<OtherWidth, Height> ret = Matrix<OtherWidth, Height>();

        for (int row = 0; row < Width; row++)
        {
            for (int col = 0; col < OtherWidth; col++)
            {
                for (int k = 0; k < Height; k++)
                {
                    ret[row][col] += (*this)[row][k] * other[k][col];
                }
            }
        }

        return ret;
    }

    template <std::size_t Width, std::size_t Height>
    inline Matrix<Width, Height> Matrix<Width, Height>::operator*(float scalar) const
    {
        Matrix<Width, Height> result = *this;

        for (int i = 0; i < Width * Height; ++i)
        {
            result.m_data[i] *= scalar;
        }

        return result;
    }

    template <std::size_t Width, std::size_t Height>
    inline Matrix<Width, Height> &Matrix<Width, Height>::operator*=(float scalar)
    {
        for (int i = 0; i < Width * Height; ++i)
        {
            m_data[i] *= scalar;
        }

        return *this;
    }

    template <std::size_t Width, std::size_t Height>
    inline bool Matrix<Width, Height>::operator==(const Matrix<Width, Height> &other) const
    {

        for (std::size_t i = 0; i < Width * Height; i++)
        {
            if (std::abs(m_data[i] - other.m_data[i]) > 1e-3f)
                return false;
        }

        return true;
    }

    template <std::size_t Width, std::size_t Height>
    inline Matrix<Height, Width> Matrix<Width, Height>::Transpose() const
    {
        Matrix<Height, Width> ret;

        for (int col = 0; col < Width; col++)
        {
            for (int row = 0; row < Height; row++)
            {
                ret[row][col] = (*this)[col][row];
            }
        }

        return ret;
    }

    template <std::size_t Width, std::size_t Height>
    inline Matrix<Width, Height> Matrix<Width, Height>::Inverse() const
    {
        static_assert(Width == Height, "Only square matrices can be inverted");
        assert(IsInvertible() && "Attempted to invert a non-invertible matrix");

        Matrix m2 = Matrix<Width, Height>();
        float det = Determinant();

        for (int row = 0; row < Height; row++)
        {
            for (int col = 0; col < Width; col++)
            {
                m2[row][col] = Cofactor(col, row) / det;
            }
        }

        return m2;
    }

    template <size_t Width, size_t Height>
    inline Matrix<Width - 1, Height - 1> Matrix<Width, Height>::SubMatrix(int rowRemoved, int colRemoved) const
    {
        Matrix<Width - 1, Height - 1> result;
        int rowIndex = 0;

        for (int row = 0; row < Height; row++)
        {
            if (row == rowRemoved)
                continue;
            int colIndex = 0;

            for (int col = 0; col < Width; col++)
            {
                if (col == colRemoved)
                    continue;
                result[rowIndex][colIndex++] = (*this)[row][col];
            }
            rowIndex++;
        }

        return result;
    }

    template <std::size_t Width, std::size_t Height>
    inline float Matrix<Width, Height>::Determinant() const
    {
        static_assert(Width == Height, "Determinant only exists for square matrices");

        if constexpr (Width == 2)
        {
            // ad - bc
            return m_data[0] * m_data[3] - m_data[1] * m_data[2];
        }
        else
        {
            float determinant = 0;

            for (int col = 0; col < Width; col++)
            {
                determinant += (*this)[0][col] * Cofactor(0, col);
            }

            return determinant;
        }
    }

    template <std::size_t Width, std::size_t Height>
    inline float Matrix<Width, Height>::Minor(int row, int col) const
    {
        Matrix<Width - 1, Height - 1> m = SubMatrix(row, col);
        return m.Determinant();
    }

    template <std::size_t Width, std::size_t Height>
    inline float Matrix<Width, Height>::Cofactor(int row, int col) const
    {
        // even sum is positive, odd sum is negative
        float sign = ((row + col) % 2) ? -1.0f : 1.0f;

        return Minor(row, col) * sign;
    }

    // =============================================================================
    // Non-member operators
    // =============================================================================

    // Scalar * Matrix (commutative counterpart to Matrix * scalar)
    template <std::size_t Width, std::size_t Height>
    inline Matrix<Width, Height> operator*(float scalar, const Matrix<Width, Height> &mat)
    {
        return mat * scalar;
    }

    // Tuples can only multiply by 4x4 Matrices because they are a 1x4 Matrix
    inline Tuple operator*(const Matrix<4, 4> &mat, const Tuple &t)
    {
        return Tuple(
            mat[0][0] * t.x + mat[0][1] * t.y + mat[0][2] * t.z + mat[0][3] * t.w,
            mat[1][0] * t.x + mat[1][1] * t.y + mat[1][2] * t.z + mat[1][3] * t.w,
            mat[2][0] * t.x + mat[2][1] * t.y + mat[2][2] * t.z + mat[2][3] * t.w,
            mat[3][0] * t.x + mat[3][1] * t.y + mat[3][2] * t.z + mat[3][3] * t.w);
    }

    // =============================================================================
    // Constants
    // =============================================================================

    inline const Matrix<4, 4> IdentityMatrix = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};
}
