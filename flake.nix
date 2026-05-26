{
  description = "NoteWrapper";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    notewrapper = {
      url = "github:tomasriveral/NoteWrapper";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, flake-utils, notewrapper }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        packages.default = pkgs.stdenv.mkDerivation (finalAttrs: {
          pname = "notewrapper";
          version = "2.0";

          src = notewrapper;

          nativeBuildInputs = with pkgs; [
            pkg-config
            gnumake
            git # The makefile uses git to get the tagged version. Without it notewrapper --version can't return the version.
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

          makeFlags = [
            "VERSION=2.0"
          ];

          buildPhase = ''
            make
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
