#pragma once
#include <cstddef>
#include "Tuple.h"

template <std::size_t width, std::size_t height> struct Matrix {
    public:
    inline static const Matrix<4,4> Identity = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    
    // For access to the underlying matrix for performance sensitive contexts.
    // I want it to be private by default so that the bracket operators get used instead
    float* data() { return mat; }
    const float* data() const { return mat; }

    Matrix() = default;
    Matrix(std::initializer_list<float> values) {
        if (values.size() != width * height)
            throw std::invalid_argument("Wrong number of values");
        std::size_t i = 0;
        for (float v : values)
            mat[i++] = v;
    }

    // Apparently a indexing on a pointer does pointer arithmatic on it and dereferences it, so this is actually how you index a matrix
    // I did not know that this was how that worked in c++ until I tried to implement this
    float* operator[](std::size_t row) {
        return &mat[row * width];
    }

    const float* operator[](std::size_t row) const {
        return &mat[row * width];
    }

    // If the 3 dimension values don't line up, the IDE will flag it in the editor
    template <std::size_t other_width>
    Matrix<other_width,height> operator*(const Matrix<other_width, width>& other) const {
        Matrix<other_width, height> ret = Matrix<other_width, height>();

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < other_width; j++) {
                for (int k = 0; k < height; k++) {
                    ret[i][j] += (*this)[i][k] * other[k][j];
                }
            }
        }

        return ret;
    }

    bool operator==(const Matrix<width, height>& other) const {
        const float* mat2 = other.data();

        for (std::size_t i = 0; i < width * height; i++)
            if (std::abs(mat[i] - mat2[i]) > 1e-6f)
                return false;

        return true;
    }

    bool operator!=(const Matrix<width, height>& other) const {
        return !(*this == other);
    }

    Matrix<height, width> Transpose() const {
        Matrix ret = Matrix<height, width>();
        
        for (int x = 0; x < height; x++) {
            for (int y = 0; y < width; y++) {
                ret[x][y] = (*this)[y][x];
            }
        }

        return ret;
    }

    float Determinant() const requires (width == height) {
        return 0.0;
    }

    // Lets the matrix print to screen
    friend std::ostream& operator<<(std::ostream& os, const Matrix<width, height>& m) {
        os << "\n";

        for (std::size_t i = 0; i < width * height; i++) {
            os << m.mat[i] << " ";
            if (i % width == width - 1) {
                os << "\n";
            }
            else {
                os << "| ";
            }
        }
        return os;
    }

    private:
    float mat[width * height] = {};
};


// Tuples can only multiply by 4x4 Matrices because they ar a 1x4 Matrix
Tuple operator*(const Matrix<4, 4>& mat, const Tuple& other) {
    float arr[4] = {other.x, other.y, other.z, other.w};
    float ret[4] = {}; 

    for (int i = 0; i < 4; i++) {
        float sum = 0;
        for (int j = 0; j < 4; j++) {
            sum += mat[i][j] * arr[j];
        }
        ret[i] = sum;
    }

    return Tuple(ret[0], ret[1], ret[2], ret[3]);
}

template <std::size_t width, std::size_t height>
Matrix<width, height> operator*(const Matrix<width, height>& mat, float scalar) {
    Matrix<width, height> result = mat;
    float* d = result.data();

    for (std::size_t i = 0; i < width * height; i++)
        d[i] *= scalar;
    return result;
}

template <std::size_t width, std::size_t height>
Matrix<width, height> operator*(float scalar, const Matrix<width, height>& mat) {
    return mat * scalar;
}