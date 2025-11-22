{
  pkgs,
  makeWrapper,
}:
pkgs.stdenv.mkDerivation rec {
  pname = "oberon0c-fuzz";
  version = "0.1.0";

  src = ./.;

  buildInputs =
    (import ./dependencies.nix {
      inherit pkgs;
    })
    ++ [
      makeWrapper
      pkgs.aflplusplus
    ];

  buildPhase = ''
    cmake -DCMAKE_C_COMPILER=afl-cc -DCMAKE_CXX_COMPILER=afl-c++ .
    cmake --build . --parallel $NIX_BUILD_CORES
  '';

  checkPhase = '''';

  installPhase = ''
    runHook preInstall

    mkdir -p $out/bin
    mv oberon0c $out/bin

    runHook postInstall
  '';

  meta = with pkgs.lib; {
    description = "Basic setup for the Oberon-0 Compiler project of the course \"Compiler Construction\".";
    homepage = "https://gitlab.inf.uni-konstanz.de/anton.pogrebnjak/${pname}";
    license = licenses.mit;
    platforms = platforms.unix;
  };
}
