#pragma once

namespace nall {

namespace Matrix {

template<typename T> inline auto Multiply(T* output, const T* xdata, unsigned xrows, unsigned xcols, const T* ydata, unsigned yrows, unsigned ycols) -> void {
  if(xcols != yrows) return;

  for(unsigned y = 0; y < xrows; y++) {
    for(unsigned x = 0; x < ycols; x++) {
      T sum = 0;
      for(unsigned z = 0; z < xcols; z++) {
        sum += xdata[y * xcols + z] * ydata[z * ycols + x];
      }
      *output++ = sum;
    }
  }
}

template<typename T> inline auto Multiply(const T* xdata, unsigned xrows, unsigned xcols, const T* ydata, unsigned yrows, unsigned ycols) -> vector<T> {
  vector<T> output;
  output.resize(xrows * ycols);
  Multiply(output.data(), xdata, xrows, xcols, ydata, yrows, ycols);
  return output;
}

}

}
