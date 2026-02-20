/*
 * Global definitions.
 *
 * Created by Michael Grossniklaus on 4/7/20.
 */

#ifndef OBERON_LLVM_GLOBAL_H
#define OBERON_LLVM_GLOBAL_H

#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>

using std::streampos;
using std::string;
using std::stringstream;

struct FilePos {
  string fileName;
  int lineNo, charNo;
  streampos offset;

  bool operator==(const FilePos &) const = default;
};
static const FilePos EMPTY_POS = {"", 0, 0, 0};

inline std::ostream &operator<<(std::ostream &stream, const FilePos &pos) {
  return stream << pos.lineNo << ":" << pos.charNo;
}

template <typename T> static string to_string(T obj) {
  stringstream stream;
  stream << obj;
  return stream.str();
}

template <typename T> static string to_string(T *obj) {
  stringstream stream;
  stream << *obj;
  return stream.str();
}

template <typename Derived, typename Base>
std::unique_ptr<Derived> dynamic_unique_ptr_copy_cast(Base *p) {
  auto cast = dynamic_cast<Derived *>(p);
  if (cast) {
    return std::make_unique<Derived>(*cast);
  }
  return {};
}

#endif // OBERON_LLVM_GLOBAL_H
