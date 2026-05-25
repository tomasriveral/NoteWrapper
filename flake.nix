{
  description = "NoteWrapper";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        packages.default = pkgs.stdenv.mkDerivation (finalAttrs: {
          pname = "notewrapper";
          version = "2.0";

          src = pkgs.fetchFromGitHub {
            owner = "tomasriveral";
            repo = "NoteWrapper";
            tag = "v${finalAttrs.version}";
            hash = "sha256-JoI38n5RZ/s0n+Vr9Bri2oiFeyj7Xrt1AmglmcuMpmQ=";
          };

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
