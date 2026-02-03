{ pkgs }:
with pkgs;
[
  llvmPackages_19.clang-tools
  boost
  llvm
  catch2_3
  clang
  clang-tools
  cmake
  codespell
  conan
  cppcheck
  doxygen
  lcov
  vcpkg
  vcpkg-tool
  libgcc
]
