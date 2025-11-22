{
  description = "A Nix-flake-based C/C++ development environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  inputs.systems.url = "github:nix-systems/default";
  inputs.flake-utils = {
    url = "github:numtide/flake-utils";
    inputs.systems.follows = "systems";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
      ...
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        devShells.default = pkgs.mkShell {
          packages =
            pkgs.callPackage ./dependencies.nix { inherit pkgs; }
            ++ (if system == "aarch64-darwin" then [ ] else [ pkgs.gdb ]);
        };
        packages.default = pkgs.callPackage ./default.nix { inherit pkgs; };
      }
    );
}
