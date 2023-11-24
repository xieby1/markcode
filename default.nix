{ lib
, stdenv
, tree-sitter
, tree-sitter-grammars
, pcre2
}: stdenv.mkDerivation {
  name = "markcode";
  version = "0.0.1";
  src = ./.;
  buildInputs = [
    tree-sitter
    pcre2
  ];
  TREESITTER_PARSERS = lib.concatMapStringsSep " " (x: x+"/parser") (with tree-sitter-grammars; [
    tree-sitter-bash
    tree-sitter-c
    tree-sitter-cpp
    tree-sitter-nix
  ]);
  MANPATH = "${pcre2.devdoc}/share/man:";
  installPhase = ''
    mkdir -p $out
    cp -r bin $out/
  '';
}
