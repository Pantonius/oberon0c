{
  pkgs,
  lib,
  oberon0c,
  writeShellScriptBin,
  makeWrapper,
  oberon0c,
}:
let
  fs = lib.fileset;
  sourceFiles = ./.;
in

pkgs.stdenv.mkDerivation rec {
  pname = "oberon0c-fuzz";
  version = "0.1.0";

  src = fs.toSource {
    root = ./.;
    fileset = sourceFiles;
  };

  inputsFrom = [
    oberon0c
  ];

  buildInputs = [
    makeWrapper
    pkgs.aflplusplus
  ];

  dontConfigure = true;

  buildPhase = ''
    cmake -DCMAKE_C_COMPILER=afl-clang-lto -DCMAKE_CXX_COMPILER=afl-clang-lto++ -DCMAKE_AR=llvm-ar -DCMAKE_RANLIB=llvm-ranlib .
    cmake --build . --parallel $NIX_BUILD_CORES

    export AFL_USE_ASAN=1 AFL_SAN_NO_INST=1
    cmake -DCMAKE_C_COMPILER=afl-clang-lto -DCMAKE_CXX_COMPILER=afl-clang-lto++ -DCMAKE_AR=llvm-ar -DCMAKE_RANLIB=llvm-ranlib -DCMAKE_EXECUTABLE_SUFFIX_CXX=_asan .
    cmake --build . --parallel $NIX_BUILD_CORES
  '';

  installPhase = ''
    runHook preInstall

    mkdir -p $out/bin
    mv oberon0c $out/bin
    mv oberon0c_asan $out/bin

    runHook postInstall
  '';

  postInstall = ''
    afl-cmin -i test -o test_unique -- $out/bin/oberon0c @@
    mv test_unique $out
  '';

  meta = with pkgs.lib; {
    description = "Basic setup for the Oberon-0 Compiler project of the course \"Compiler Construction\".";
    homepage = "https://gitlab.inf.uni-konstanz.de/anton.pogrebnjak/${pname}";
    license = licenses.mit;
    platforms = platforms.unix;
  };
}
