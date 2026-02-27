{
  pkgs,
  lib,
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

  nativeBuildInputs = [
    pkgs.cmake
  ];

  buildInputs = [
    oberon0c.buildInputs
    pkgs.aflplusplus
  ];

  configurePhase = ''
    mkdir -p build/afl
    cmake --preset afl
  '';

  buildPhase = ''
    cmake --build --preset afl-asan --parallel $NIX_BUILD_CORES
    mv build/afl/oberon0c build/afl/oberon0c_asan
    cmake --build --preset afl --parallel $NIX_BUILD_CORES
  '';

  installPhase = ''
    runHook preInstall

    mkdir -p $out/bin
    mv build/afl/oberon0c $out/bin
    mv build/afl/oberon0c_asan $out/bin

    runHook postInstall
  '';

  postInstall = ''
    afl-cmin -i tests/resources -o test_unique -- $out/bin/oberon0c @@
    mv test_unique $out
  '';

  meta = with pkgs.lib; {
    description = "Basic setup for the Oberon-0 Compiler project of the course \"Compiler Construction\".";
    homepage = "https://gitlab.inf.uni-konstanz.de/anton.pogrebnjak/${pname}";
    license = licenses.mit;
    platforms = platforms.unix;
  };
}
