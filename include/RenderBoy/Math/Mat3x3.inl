#pragma once

namespace RB {

template <typename T>
auto Mat3x3<T>::operator==(const Mat3x3 &rhs) const -> bool {
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; i < 3; ++i) {
      if ((*this)[i][j] != rhs[i][j]) {
        return false;
      }
    }
  }
  return true;
}

template <typename T>
auto Mat3x3<T>::operator!=(const Mat3x3 &rhs) const -> bool {
  return !(*this == rhs);
}

template <typename T> auto Mat3x3<T>::operator*(T v) const -> Mat3x3 {
  Mat3x3 out;
  for (size_t i = 0; i < 9; i++) {
    (reinterpret_cast<T *>(&out))[i] = v * (reinterpret_cast<T *>(this))[i];
  }

  return out;
}

template <typename T> auto Mat3x3<T>::operator*=(T v) -> Mat3x3 & {
  for (size_t i = 0; i < 9; i++) {
    (reinterpret_cast<T *>(&this))[i] *= v;
  }

  return *this;
}

template <typename T>
auto Mat3x3<T>::operator*(const Mat3x3 &rhs) const -> Mat3x3 {
  Mat3x3 out;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      for (int k = 0; k < 3; k++) {
        out.data[i][j] += this->data[i][k] * rhs.data[k][j];
      }
    }
  }

  return out;
}

template <typename T>
auto Mat3x3<T>::operator*=(const Mat3x3 &rhs) -> Mat3x3 & {
  *this = *this * rhs;
  return *this;
}

template <typename T> auto Mat3x3<T>::transposed() const -> Mat3x3 {
  return Mat3x3{_11, _21, _31, _12, _22, _32, _13, _23, _33};
}

template <typename T> auto Mat3x3<T>::determinant() const -> T {
  T det = 0;
  det += _11 * (_22 * _33 - _32 * _23);
  det -= _12 * (_21 * _33 - _31 * _23);
  det += _13 * (_21 * _32 - _31 * _22);
  return det;
}

template <typename T> auto Mat3x3<T>::Identity() -> Mat3x3 {
  return {1, 0, 0, 0, 1, 0, 0, 0, 1};
}

template <typename T> auto Mat3x3<T>::From(const T *values) -> Mat3x3 {
  return {values[0], values[1], values[2], values[3], values[4],
          values[5], values[6], values[7], values[8]};
}

} // namespace RB