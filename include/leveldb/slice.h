// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Slice is a simple structure containing a pointer into some external
// storage and a size.  The user of a Slice must ensure that the slice
// is not used after the corresponding external storage has been
// deallocated.
//
// Multiple threads can invoke const methods on a Slice without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Slice must use
// external synchronization.

#ifndef STORAGE_LEVELDB_INCLUDE_SLICE_H_
#define STORAGE_LEVELDB_INCLUDE_SLICE_H_

#include <cassert>
#include <cstddef>
#include <cstring>
#include <string>

#include "leveldb/export.h"

namespace leveldb {

class LEVELDB_EXPORT Slice {
 public:
  // Create an empty slice.
  Slice() : data_(""), size_(0) {}

  // Create a slice that refers to d[0,n-1].
  Slice(const char* d, size_t n) : data_(d), size_(n) {}

  // Create a slice that refers to the contents of "s"
  Slice(const std::string& s) : data_(s.data()), size_(s.size()) {}

  // Create a slice that refers to s[0,strlen(s)-1]
  Slice(const char* s) : data_(s), size_(strlen(s)) {}

  // Intentionally copyable.
  Slice(const Slice&) = default;
  Slice& operator=(const Slice&) = default;

  // Return a pointer to the beginning of the referenced data
  const char* data() const { return data_; }

  // Return the length (in bytes) of the referenced data
  size_t size() const { return size_; }

  // Return true iff the length of the referenced data is zero
  bool empty() const { return size_ == 0; }

  // Return the ith byte in the referenced data.
  // REQUIRES: n < size()
  char operator[](size_t n) const {
    assert(n < size());
    return data_[n];
  }

  // Change this slice to refer to an empty array
  void clear() {
    data_ = "";
    size_ = 0;
  }

  // Drop the first "n" bytes from this slice.
  void remove_prefix(size_t n) {
    assert(n <= size());
    data_ += n;
    size_ -= n;
  }

  // Return a string that contains the copy of the referenced data.
  std::string ToString() const { return std::string(data_, size_); }

  // Three-way comparison.  Returns value:
  //   <  0 iff "*this" <  "b",
  //   == 0 iff "*this" == "b",
  //   >  0 iff "*this" >  "b"
  int compare(const Slice& b) const;

  // Return true iff "x" is a prefix of "*this"
  bool starts_with(const Slice& x) const {
    return ((size_ >= x.size_) && (memcmp(data_, x.data_, x.size_) == 0));
  }

 protected:  // 由于添加所加
  const char* data_;
  size_t size_;
};

inline bool operator==(const Slice& x, const Slice& y) {
  return ((x.size() == y.size()) &&
          (memcmp(x.data(), y.data(), x.size()) == 0));
}

inline bool operator!=(const Slice& x, const Slice& y) { return !(x == y); }

inline int Slice::compare(const Slice& b) const {
  const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
  int r = memcmp(data_, b.data_, min_len);
  if (r == 0) {
    if (size_ < b.size_)
      r = -1;
    else if (size_ > b.size_)
      r = +1;
  }
  return r;
}
class LEVELDB_EXPORT Slice_v : public Slice {
 public:
  Slice_v() : dst_(0), src_(0) {}
  Slice_v(int src, int dst) : s_(std::move(std::to_string(src)+'|'+std::to_string(dst))), src_(src), dst_(dst) {
    data_ = s_.data();
    size_ = s_.size();
  }
  // Slice_v& operator=(Slice_v&) = default;
  /**
  * 比较规则，有出度顶点进行排序若出度顶点相同由入度顶点进行排序
  * @param b 比较元素
  * @return// Three-way comparison.  Returns value:
          //   <  0 iff "*this" <  "b",
          //   == 0 iff "*this" == "b",
          //   >  0 iff "*this" >  "b"
  */
  int compare(const Slice_v& b) const;
  int src() const { return src_; }
  int dst() const { return dst_; }

 private:
  int src_;
  int dst_;
  std::string s_;
};

inline bool operator==(Slice_v x, Slice_v& y) {
  return x.src() == y.src() && (x.dst() == y.dst() || x.dst() == -1 ||
         y.dst() == -1);
}

inline bool operator!=(const Slice_v& x, const Slice_v& y) { return !(x == y); }

inline int Slice_v::compare(const Slice_v& b) const {
    if(src_<b.src_) return -1;
    else if(src_>b.src_) return 1;
    else if(dst_<b.dst_) return -1;
    else if(dst_>b.dst_) return 1;
    else  return 0;
}
}  // namespace leveldb


#endif  // STORAGE_LEVELDB_INCLUDE_SLICE_H_
