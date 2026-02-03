/*
 * Global definitions.
 *
 * Created by Michael Grossniklaus on 4/7/20.
 */

#ifndef OBERON_LLVM_GLOBAL_H
#define OBERON_LLVM_GLOBAL_H

#include <ostream>
#include <sstream>
#include <string>

using std::streampos;
using std::string;
using std::stringstream;

struct FilePos {
  string fileName;
  int lineNo, charNo;
  streampos offset;
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

#endif // OBERON_LLVM_GLOBAL_H
