{
  pkgs,
  makeWrapper,
}:
pkgs.stdenv.mkDerivation rec {
  pname = "oberon0c";
  version = "0.1.0";

  src = ./.;

  nativeBuildInputs = with pkgs; [
    ninja
    cmake
    clang
    clang-tools
    libgcc
  ];

  buildInputs = with pkgs; [
    boost
    libllvm
  ];

  checkInputs = with pkgs; [
    catch2_3
  ];

  doCheck = true;

  configurePhase = ''
    cmake . -G Ninja
  '';

  buildPhase = ''
    ninja
  '';

  checkPhase = ''
    ninja test
  '';

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
