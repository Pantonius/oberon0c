{ pkgs }:
with pkgs;
[
  llvmPackages_19.clang-tools
  boost
  llvm
  catch2_3
  clang-tools
  cmake
  codespell
  conan
  cppcheck
  doxygen
  gtest
  lcov
  vcpkg
  vcpkg-tool
  libgcc
]
