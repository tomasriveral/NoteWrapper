{
  description = "NoteWrapper";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils}:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        packages.default = pkgs.stdenv.mkDerivation (finalAttrs: {
          pname = "notewrapper";
          version = "v2.2.1";

          src = ./.;

          nativeBuildInputs = with pkgs; [
            pkg-config
            gnumake
          ];

          buildInputs = with pkgs; [
            ncurses
            cjson
            vivify
            rsync
            ripgrep
            fzf
            gnused
          ];

          buildPhase = ''
            make VERSION=${finalAttrs.version}
          '';

          installPhase = ''
            mkdir -p $out/bin
            cp notewrapper $out/bin/
          '';
        });

        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            cjson
            ncurses
            pkg-config
            gdb
            valgrind
            clang-tools
            gnumake
          ];
        };

        apps.default = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/notewrapper";
        };
      });
}
