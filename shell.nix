let
  pkgs = import <nixpkgs> {};
in pkgs.mkShell {
  packages = with pkgs; [
    tree-sitter
  ];
  shellHook = ''
    NIX_CFLAGS_COMPILE+=" -ltree-sitter ${pkgs.tree-sitter-grammars.tree-sitter-nix}/parser"
  '';
}
