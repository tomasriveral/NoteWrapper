{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.cjson
    pkgs.ncurses
    pkgs.pkg-config
    pkgs.gdb
  ];
}
