FROM alpine:latest

# Install build deps
RUN apk add build-base clang clang-dev llvm llvm-dev llvm-static llvm-test-utils llvm-gtest clang-extra-tools cmake extra-cmake-modules autoconf automake make pkgconf curl-dev boost-dev catch2-3
