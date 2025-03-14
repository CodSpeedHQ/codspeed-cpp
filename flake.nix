{
  description = "A minimal C++ development environment using flake-utils";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };

        # Common build inputs for both shells
        commonBuildInputs = with pkgs; [
          gcc
          cmake
          pkg-config
        ];

      in
      {
        devShells = {
          default = pkgs.mkShell {
            buildInputs = commonBuildInputs;
            shellHook = ''
              echo "Welcome to the C++ development environment!"
            '';
          };

          clang = pkgs.mkShell {
            buildInputs = with pkgs; [ clang ] ++ commonBuildInputs;
            shellHook = ''
              export CXX=clang++;
              echo "Welcome to the C++ development environment with Clang!"
            '';
          };

          lsp = pkgs.mkShell {
            buildInputs =
              with pkgs;
              [
                gersemi
                clang-tools
              ]
              ++ commonBuildInputs;
            shellHook = ''
              export CMAKE_EXPORT_COMPILE_COMMANDS=ON
              echo "Welcome to the C++ development environment with LSP tools!"
            '';
          };
        };
      }
    );
}
