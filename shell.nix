let
  pkgs = import <nixpkgs> {};
in pkgs.mkShell {
  packages = with pkgs; [
    tree-sitter
    pcre2.dev # headers
    pcre2.out # libs
  ];
  shellHook = ''
    export TREESITTER_PARSERS="${pkgs.tree-sitter-grammars.tree-sitter-nix}/parser"

    # appending pcre2 man path to system man paths
    export MANPATH+="${pkgs.pcre2.devdoc}/share/man:"
  '';
}
