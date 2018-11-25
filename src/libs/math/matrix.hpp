#pragma once

#include "vector.hpp"
#include "util.hpp"
#include <array>

namespace gorc {

template <size_t m, typename G> class matrix_row_vector;
template <size_t m, typename G> class matrix_col_vector;
template <size_t m, typename G> class matrix_row_vector_iterator;
template <size_t m, typename G> class matrix_row_element_iterator;
template <size_t m, typename G> class matrix_col_vector_iterator;
template <size_t m, typename G> class matrix_col_element_iterator;
template <size_t m, typename G> class matrix_row_vector_const_iterator;
template <size_t m, typename G> class matrix_row_element_const_iterator;
template <size_t m, typename G> class matrix_col_vector_const_iterator;
template <size_t m, typename G> class matrix_col_element_const_iterator;

template <size_t n, typename F = float> class matrix {
    template <size_t i, size_t j, size_t m, typename G> friend G& get(matrix<m, G>&);
    template <size_t i, size_t j, size_t m, typename G> friend G get(const matrix<m, G>&);
    template <size_t m, typename G, typename... H> friend matrix<m, G> make_matrix(H&&...);
    template <size_t m, typename G> friend const G* make_opengl_matrix(const matrix<m, G>&);
    template <size_t m, typename G> friend matrix<m, G> make_scale_matrix(const vector<m, G>&);

    template <size_t m, typename G> friend class matrix_row_vector;
    template <size_t m, typename G> friend class matrix_col_vector;
    template <size_t m, typename G> friend class matrix_row_vector_iterator;
    template <size_t m, typename G> friend class matrix_row_element_iterator;
    template <size_t m, typename G> friend class matrix_col_vector_iterator;
    template <size_t m, typename G> friend class matrix_col_element_iterator;
    template <size_t m, typename G> friend class matrix_row_vector_const_iterator;
    template <size_t m, typename G> friend class matrix_row_element_const_iterator;
    template <size_t m, typename G> friend class matrix_col_vector_const_iterator;
    template <size_t m, typename G> friend class matrix_col_element_const_iterator;

private:
    F data[n][n];

    inline F& get(size_t i, size_t j) {
        return data[j][i];
    }

    inline F get(size_t i, size_t j) const {
        return data[j][i];
    }

    matrix() {
        return;
    }

public:
    matrix transpose() const {
        matrix rv;
        for(size_t i = 0; i < n; ++i) {
            for(size_t j = 0; j < n; ++j) {
                rv.get(i, j) = get(j, i);
            }
        }

        return rv;
    }

    inline matrix operator+(const matrix& m) const {
        matrix rv;
        for(size_t i = 0; i < n; ++i) {
            for(size_t j = 0; j < n; ++j) {
                rv.get(i, j) = get(i, j) + m.get(i, j);
            }
        }
        return rv;
    }

    inline matrix operator-(const matrix& m) const {
        matrix rv;
        for(size_t i = 0; i < n; ++i) {
            for(size_t j = 0; j < n; ++j) {
                rv.get(i, j) = get(i, j) - m.get(i, j);
            }
        }
        return rv;
    }

    inline matrix operator*(const matrix& m) const {
        matrix rv;
        for(size_t i = 0; i < n; ++i) {
            for(size_t j = 0; j < n; ++j) {
                rv.get(i, j) = 0;
                for(size_t r = 0; r < n; ++r) {
                    rv.get(i, j) += get(i, r) * m.get(r, j);
                }
            }
        }
        return rv;
    }

    inline vector<n, F> operator*(const vector<n, F>& v) const {
        auto rv = make_zero_vector<n, F>();
        auto rt = rv.begin();
        for(size_t i = 0; i < n; ++i, ++rt) {
            *rt = 0;
            auto it = v.begin();
            for(size_t j = 0; j < n; ++j, ++it) {
                *rt += get(i, j) * (*it);
            }
        }

        return rv;
    }

    inline vector<n - 1, F> transform(const vector<n - 1, F>& v) const {
        auto rv = make_zero_vector<n - 1, F>();
        auto rt = rv.begin();
        for(size_t i = 0; i < n - 1; ++i, ++rt) {
            *rt = 0;
            auto it = v.begin();
            for(size_t j = 0; j < n - 1; ++j, ++it) {
                *rt += get(i, j) * (*it);
            }

            *rt += get(i, n - 1);
        }

        return rv;
    }

    inline vector<n - 1, F> transform_normal(const vector<n - 1, F>& v) const {
        auto rv = make_zero_vector<n - 1, F>();
        auto rt = rv.begin();
        for(size_t i = 0; i < n - 1; ++i, ++rt) {
            *rt = 0;
            auto it = v.begin();
            for(size_t j = 0; j < n - 1; ++j, ++it) {
                *rt += get(i, j) * (*it);
            }
        }

        return rv;
    }

    inline matrix_row_vector_iterator<n, F> row_begin() {
        return matrix_row_vector_iterator<n, F>(*this, 0);
    }

    inline matrix_row_vector_iterator<n, F> row_end() {
        return matrix_row_vector_iterator<n, F>(*this, n);
    }

    inline matrix_col_vector_iterator<n, F> col_begin() {
        return matrix_col_vector_iterator<n, F>(*this, 0);
    }

    inline matrix_col_vector_iterator<n, F> col_end() {
        return matrix_col_vector_iterator<n, F>(*this, n);
    }

    inline matrix_row_vector_const_iterator<n, F> row_begin() const {
        return matrix_row_vector_const_iterator<n, F>(*this, 0);
    }

    inline matrix_row_vector_const_iterator<n, F> row_end() const {
        return matrix_row_vector_const_iterator<n, F>(*this, n);
    }

    inline matrix_col_vector_const_iterator<n, F> col_begin() const {
        return matrix_col_vector_const_iterator<n, F>(*this, 0);
    }

    inline matrix_col_vector_const_iterator<n, F> col_end() const {
        return matrix_col_vector_const_iterator<n, F>(*this, n);
    }
};

template <size_t n, typename F> class matrix_row_vector {
private:
    matrix<n, F>& mat;
    int row;

public:
    matrix_row_vector(matrix<n, F>& mat, int row) : mat(mat), row(row) {
        return;
    }

    const matrix_row_vector& operator=(const vector<n, F>& vec) {
        auto vt = vec.begin();
        for(size_t i = 0; i < n; ++i, ++vt) {
            mat.get(row, i) = *vt;
        }

        return *this;
    }
};

template <size_t n, typename F> class matrix_col_vector {
private:
    matrix<n, F>& mat;
    int col;

public:
    matrix_col_vector(matrix<n, F>& mat, int col) : mat(mat), col(col) {
        return;
    }

    const matrix_col_vector& operator=(const vector<n, F>& vec) {
        auto vt = vec.begin();
        for(size_t i = 0; i < n; ++i, ++vt) {
            mat.get(i, col) = *vt;
        }

        return *this;
    }
};

template <size_t n, typename F> class matrix_row_element_iterator {
private:
    matrix<n, F>& mat;
    int row = 0;
    int col = 0;

public:
    matrix_row_element_iterator(matrix<n, F>& mat, int row, int col) : mat(mat), row(row), col(col) {
        return;
    }

    inline matrix_row_element_iterator operator++(int) {
        return matrix_row_element_iterator(mat, row, col++);
    }

    inline matrix_row_element_iterator operator++() {
        ++col;
        return *this;
    }

    inline matrix_row_element_iterator operator--(int) {
        return matrix_row_element_iterator(mat, row, col--);
    }

    inline matrix_row_element_iterator& operator--() {
        --col;
        return *this;
    }

    inline matrix_row_element_iterator operator+(int v) const {
        return matrix_row_element_iterator(mat, row, col + v);
    }

    inline matrix_row_element_iterator operator-(int v) const {
        return matrix_row_element_iterator(mat, row, col - v);
    }

    inline const matrix_row_element_iterator& operator+=(int v) {
        col += v;
        return *this;
    }

    inline const matrix_row_element_iterator& operator-=(int v) {
        col -= v;
        return *this;
    }

    inline F& operator*() {
        return mat.get(row, col);
    }

    inline bool operator==(const matrix_row_element_iterator& it) const {
        return it.row == row && it.col == col;
    }

    inline bool operator!=(const matrix_row_element_iterator& it) const {
        return it.row != row || it.col != col;
    }
};

template <size_t n, typename F> class matrix_row_vector_iterator {
private:
    matrix<n, F>& mat;
    int row = 0;

public:
    matrix_row_vector_iterator(matrix<n, F>& mat, int row) : mat(mat), row(row) {
        return;
    }

    inline matrix_row_vector_iterator operator++(int) {
        return matrix_row_vector_iterator(mat, row++);
    }

    inline matrix_row_vector_iterator& operator++() {
        ++row;
        return *this;
    }

    inline matrix_row_vector_iterator operator--(int) {
        return matrix_row_vector_iterator(mat, row--);
    }

    inline matrix_row_vector_iterator& operator--() {
        --row;
        return *this;
    }

    inline matrix_row_vector_iterator operator+(int v) const {
        return matrix_row_vector_iterator(mat, row + v);
    }

    inline matrix_row_vector_iterator operator-(int v) const {
        return matrix_row_vector_iterator(mat, row - v);
    }

    inline const matrix_row_vector_iterator& operator+=(int v) {
        row += v;
        return *this;
    }

    inline const matrix_row_vector_iterator& operator-=(int v) {
        row -= v;
        return *this;
    }

    inline matrix_row_vector<n, F> operator*() {
        return matrix_row_vector<n, F>(mat, row);
    }

    inline matrix_row_element_iterator<n, F> begin() {
        return matrix_row_element_iterator<n, F>(mat, row, 0);
    }

    inline matrix_row_element_iterator<n, F> end() {
        return matrix_row_element_iterator<n, F>(mat, row, n);
    }

    inline bool operator==(const matrix_row_vector_iterator& it) const {
        return it.row == row;
    }

    inline bool operator!=(const matrix_row_vector_iterator& it) const {
        return it.row != row;
    }
};

template <size_t n, typename F> class matrix_col_element_iterator {
private:
    matrix<n, F>& mat;
    int row = 0;
    int col = 0;

public:
    matrix_col_element_iterator(matrix<n, F>& mat, int row, int col) : mat(mat), row(row), col(col) {
        return;
    }

    inline matrix_col_element_iterator operator++(int) {
        return matrix_col_element_iterator(mat, row++, col);
    }

    inline matrix_col_element_iterator operator++() {
        ++row;
        return *this;
    }

    inline matrix_col_element_iterator operator--(int) {
        return matrix_col_element_iterator(mat, row--, col);
    }

    inline matrix_col_element_iterator& operator--() {
        --row;
        return *this;
    }

    inline matrix_col_element_iterator operator+(int v) const {
        return matrix_col_element_iterator(mat, row + v, col);
    }

    inline matrix_col_element_iterator operator-(int v) const {
        return matrix_col_element_iterator(mat, row - v, col);
    }

    inline const matrix_col_element_iterator& operator+=(int v) {
        row += v;
        return *this;
    }

    inline const matrix_col_element_iterator& operator-=(int v) {
        row -= v;
        return *this;
    }

    inline F& operator*() {
        return mat.get(row, col);
    }

    inline bool operator==(const matrix_col_element_iterator& it) const {
        return it.row == row && it.col == col;
    }

    inline bool operator!=(const matrix_col_element_iterator& it) const {
        return it.row != row || it.col != col;
    }
};

template <size_t n, typename F> class matrix_col_vector_iterator {
private:
    matrix<n, F>& mat;
    int col = 0;

public:
    matrix_col_vector_iterator(matrix<n, F>& mat, int col) : mat(mat), col(col) {
        return;
    }

    inline matrix_col_vector_iterator operator++(int) {
        return matrix_col_vector_iterator(mat, col++);
    }

    inline matrix_col_vector_iterator& operator++() {
        ++col;
        return *this;
    }

    inline matrix_col_vector_iterator operator--(int) {
        return matrix_col_vector_iterator(mat, col--);
    }

    inline matrix_col_vector_iterator& operator--() {
        --col;
        return *this;
    }

    inline matrix_col_vector_iterator operator+(int v) const {
        return matrix_col_vector_iterator(mat, col + v);
    }

    inline matrix_col_vector_iterator operator-(int v) const {
        return matrix_col_vector_iterator(mat, col - v);
    }

    inline const matrix_col_vector_iterator& operator+=(int v) {
        col += v;
        return *this;
    }

    inline const matrix_col_vector_iterator& operator-=(int v) {
        col -= v;
        return *this;
    }

    inline matrix_col_vector<n, F> operator*() {
        return matrix_col_vector<n, F>(mat, col);
    }

    inline matrix_col_element_iterator<n, F> begin() {
        return matrix_col_element_iterator<n, F>(mat, 0, col);
    }

    inline matrix_col_element_iterator<n, F> end() {
        return matrix_col_element_iterator<n, F>(mat, n, col);
    }

    inline bool operator==(const matrix_col_vector_iterator& it) const {
        return it.col == col;
    }

    inline bool operator!=(const matrix_col_vector_iterator& it) const {
        return it.col != col;
    }
};

template <size_t n, typename F> class matrix_row_element_const_iterator {
private:
    const matrix<n, F>& mat;
    int row = 0;
    int col = 0;

public:
    matrix_row_element_const_iterator(const matrix<n, F>& mat, int row, int col) : mat(mat), row(row), col(col) {
        return;
    }

    inline matrix_row_element_const_iterator operator++(int) {
        return matrix_row_element_const_iterator(mat, row, col++);
    }

    inline matrix_row_element_const_iterator operator++() {
        ++col;
        return *this;
    }

    inline matrix_row_element_const_iterator operator--(int) {
        return matrix_row_element_const_iterator(mat, row, col--);
    }

    inline matrix_row_element_const_iterator& operator--() {
        --col;
        return *this;
    }

    inline matrix_row_element_const_iterator operator+(int v) const {
        return matrix_row_element_const_iterator(mat, row, col + v);
    }

    inline matrix_row_element_const_iterator operator-(int v) const {
        return matrix_row_element_const_iterator(mat, row, col - v);
    }

    inline const matrix_row_element_const_iterator& operator+=(int v) {
        col += v;
        return *this;
    }

    inline const matrix_row_element_const_iterator& operator-=(int v) {
        col -= v;
        return *this;
    }

    inline F& operator*() {
        return mat.get(row, col);
    }

    inline bool operator==(const matrix_row_element_const_iterator& it) const {
        return it.row == row && it.col == col;
    }

    inline bool operator!=(const matrix_row_element_const_iterator& it) const {
        return it.row != row || it.col != col;
    }
};

template <size_t n, typename F> class matrix_row_vector_const_iterator {
private:
    const matrix<n, F>& mat;
    int row = 0;

public:
    matrix_row_vector_const_iterator(const matrix<n, F>& mat, int row) : mat(mat), row(row) {
        return;
    }

    inline matrix_row_vector_const_iterator operator++(int) {
        return matrix_row_vector_const_iterator(mat, row++);
    }

    inline matrix_row_vector_const_iterator& operator++() {
        ++row;
        return *this;
    }

    inline matrix_row_vector_const_iterator operator--(int) {
        return matrix_row_vector_const_iterator(mat, row--);
    }

    inline matrix_row_vector_const_iterator& operator--() {
        --row;
        return *this;
    }

    inline matrix_row_vector_const_iterator operator+(int v) const {
        return matrix_row_vector_const_iterator(mat, row + v);
    }

    inline matrix_row_vector_const_iterator operator-(int v) const {
        return matrix_row_vector_const_iterator(mat, row - v);
    }

    inline const matrix_row_vector_const_iterator& operator+=(int v) {
        row += v;
        return *this;
    }

    inline const matrix_row_vector_const_iterator& operator-=(int v) {
        row -= v;
        return *this;
    }

    inline matrix_row_element_const_iterator<n, F> begin() {
        return matrix_row_element_const_iterator<n, F>(mat, row, 0);
    }

    inline matrix_row_element_const_iterator<n, F> end() {
        return matrix_row_element_const_iterator<n, F>(mat, row, n);
    }

    inline bool operator==(const matrix_row_vector_const_iterator& it) const {
        return it.row == row;
    }

    inline bool operator!=(const matrix_row_vector_const_iterator& it) const {
        return it.row != row;
    }
};

template <size_t n, typename F> class matrix_col_element_const_iterator {
private:
    const matrix<n, F>& mat;
    int row = 0;
    int col = 0;

public:
    matrix_col_element_const_iterator(const matrix<n, F>& mat, int row, int col) : mat(mat), row(row), col(col) {
        return;
    }

    inline matrix_col_element_const_iterator operator++(int) {
        return matrix_col_element_const_iterator(mat, row++, col);
    }

    inline matrix_col_element_const_iterator operator++() {
        ++row;
        return *this;
    }

    inline matrix_col_element_const_iterator operator--(int) {
        return matrix_col_element_const_iterator(mat, row--, col);
    }

    inline matrix_col_element_const_iterator& operator--() {
        --row;
        return *this;
    }

    inline matrix_col_element_const_iterator operator+(int v) const {
        return matrix_col_element_const_iterator(mat, row + v, col);
    }

    inline matrix_col_element_const_iterator operator-(int v) const {
        return matrix_col_element_const_iterator(mat, row - v, col);
    }

    inline const matrix_col_element_const_iterator& operator+=(int v) {
        row += v;
        return *this;
    }

    inline const matrix_col_element_const_iterator& operator-=(int v) {
        row -= v;
        return *this;
    }

    inline F& operator*() {
        return mat.get(row, col);
    }

    inline bool operator==(const matrix_col_element_const_iterator& it) const {
        return it.row == row && it.col == col;
    }

    inline bool operator!=(const matrix_col_element_const_iterator& it) const {
        return it.row != row || it.col != col;
    }
};

template <size_t n, typename F> class matrix_col_vector_const_iterator {
private:
    const matrix<n, F>& mat;
    int col = 0;

public:
    matrix_col_vector_const_iterator(const matrix<n, F>& mat, int col) : mat(mat), col(col) {
        return;
    }

    inline matrix_col_vector_const_iterator operator++(int) {
        return matrix_col_vector_const_iterator(mat, col++);
    }

    inline matrix_col_vector_const_iterator& operator++() {
        ++col;
        return *this;
    }

    inline matrix_col_vector_const_iterator operator--(int) {
        return matrix_col_vector_const_iterator(mat, col--);
    }

    inline matrix_col_vector_const_iterator& operator--() {
        --col;
        return *this;
    }

    inline matrix_col_vector_const_iterator operator+(int v) const {
        return matrix_col_vector_const_iterator(mat, col + v);
    }

    inline matrix_col_vector_const_iterator operator-(int v) const {
        return matrix_col_vector_const_iterator(mat, col - v);
    }

    inline const matrix_col_vector_const_iterator& operator+=(int v) {
        col += v;
        return *this;
    }

    inline const matrix_col_vector_const_iterator& operator-=(int v) {
        col -= v;
        return *this;
    }

    inline matrix_col_element_const_iterator<n, F> begin() {
        return matrix_col_element_const_iterator<n, F>(mat, 0, col);
    }

    inline matrix_col_element_const_iterator<n, F> end() {
        return matrix_col_element_const_iterator<n, F>(mat, n, col);
    }

    inline bool operator==(const matrix_col_vector_const_iterator& it) const {
        return it.col == col;
    }

    inline bool operator!=(const matrix_col_vector_const_iterator& it) const {
        return it.col != col;
    }
};

template <size_t i, size_t j, size_t n, typename F> F& get(matrix<n, F>& matrix) {
    static_assert(i < n, "row index out of bounds");
    static_assert(j < n, "column index out of bounds");
    return matrix.get(i, j);
}

template <size_t i, size_t j, size_t n, typename F> F get(const matrix<n, F>& matrix) {
    static_assert(i < n, "row index out of bounds");
    static_assert(j < n, "column index out of bounds");
    return matrix.get(i, j);
}

template <size_t n, typename F, typename G, typename... H> void apply_row(matrix_row_vector_iterator<n, F> it, G&& row0, H&&... rows) {
    *it = row0;
    apply_row(++it, std::forward<H>(rows)...);
}

template <size_t n, typename F, typename G> void apply_row(matrix_row_vector_iterator<n, F> it, G&& row0) {
    *it = row0;
}

template <size_t n, typename F, typename... G> matrix<n, F> make_matrix(G&&... rows) {
    static_assert(sizeof...(G) == n, "row count mismatch");

    matrix<n, F> rv;
    apply_row(rv.row_begin(), std::forward<G>(rows)...);

    return rv;
}

template <size_t n, typename F> const F* make_opengl_matrix(const matrix<n, F>& matrix) {
    return &matrix.data[0][0];
}

template <size_t n, typename F = float> matrix<n, F> make_scale_matrix(const vector<n, F>& amt) {
    matrix<n, F> rv;
    auto kt = amt.begin();
    auto it = rv.row_begin();
    for(size_t i = 0; i < n; ++i) {

        auto jt = it.begin();
        for(size_t j = 0; j < n; ++j) {
            *jt = (i == j) ? *kt : 0;

            ++jt;
        }

        ++it;
        ++kt;
    }

    return rv;
}

template <size_t n, typename F = float> matrix<n, F> make_identity_matrix() {
    return make_scale_matrix(make_fill_vector<n, F>(1));
}

template <typename F> matrix<4, F> make_frustum_matrix(F left, F right, F bottom, F top, F znear, F zfar) {
    float temp, temp2, temp3, temp4;
    temp = F(2) * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;

    return make_matrix<4, F>(
            make_vector(temp / temp2, F(0),         (right + left) / temp2,  F(0) ),
            make_vector(F(0),         temp / temp3, (top + bottom) / temp3,  F(0) ),
            make_vector(F(0),         F(0),         (-zfar - znear) / temp4, (-temp * zfar) / temp4 ),
            make_vector(F(0),         F(0),         F(-1),                F(0) ));
}

template <typename F> matrix<4, F> make_perspective_matrix(F fovy_in_degrees, F aspect_ratio, F znear, F zfar) {
    F ymax = znear * static_cast<F>(tan(to_radians(static_cast<double>(fovy_in_degrees)) * 0.5));
    F xmax = ymax * aspect_ratio;
    return make_frustum_matrix(-xmax, xmax, -ymax, ymax, znear, zfar);
}

template <typename F> matrix<4, F> make_orthographic_matrix(F left, F right, F bottom, F top, F znear, F zfar) {
    return make_matrix<4, F>(
        make_vector(F(2) / (right - left), F(0), F(0), -(right + left) / (right - left)),
        make_vector(F(0), F(2) / (top - bottom), F(0), -(top + bottom) / (top - bottom)),
        make_vector(F(0), F(0), -F(2) / (zfar - znear), -(zfar + znear) / (zfar - znear)),
        make_vector(F(0), F(0), F(0), F(1))
    );
}

template <size_t n, typename F> matrix<n + 1, F> make_translation_matrix(const vector<n, F>& translation) {
    auto rv = make_identity_matrix<n + 1, F>();

    *(rv.col_end() - 1) = extend_vector<n + 1, F>(translation, static_cast<F>(1));

    return rv;
}

template <typename F> matrix<4, F> make_look_matrix(const vector<3, F>& camera_position, const vector<3, F>& camera_look, const vector<3, F>& camera_up) {
    auto camera_side = cross(camera_look, camera_up);
    auto new_camera_up = cross(camera_side, camera_look);

    return make_matrix<4, F>(
            extend_vector<4, F>(camera_side, F(0)),
            extend_vector<4, F>(new_camera_up, F(0)),
            extend_vector<4, F>(-camera_look, F(0)),
            make_vector(F(0), F(0), F(0), F(1))) * make_translation_matrix(-camera_position);
}

template <typename F> matrix<4, F> make_rotation_matrix(F angle, const vector<3, F>& axis) {
    F ax = get<0>(axis);
    F ay = get<1>(axis);
    F az = get<2>(axis);

    F cost = static_cast<F>(cos(to_radians(angle)));
    F sint = static_cast<F>(sin(to_radians(angle)));
    F icost = F(1) - cost;

    return make_matrix<4, F>(
            make_vector(cost + ax*ax*icost, ax*ay*icost - az*sint, ax*az*icost + ay*sint, F(0)),
            make_vector(ax*ay*icost + az*sint, cost + ay*ay*icost, ay*az*icost - ax*sint, F(0)),
            make_vector(ax*az*icost - ay*sint, ay*az*icost + ax*sint, cost + az*az*icost, F(0)),
            make_vector(F(0), F(0), F(0), F(1)));
}

template <typename F> matrix<4, F> make_shadow_bias_matrix() {
    return make_matrix<4, F>(
            make_vector(F(0.5), F(0.0), F(0.0), F(0.5)),
            make_vector(F(0.0), F(0.5), F(0.0), F(0.5)),
            make_vector(F(0.0), F(0.0), F(0.5), F(0.5)),
            make_vector(F(0.0), F(0.0), F(0.0), F(1.0)));
}

}
