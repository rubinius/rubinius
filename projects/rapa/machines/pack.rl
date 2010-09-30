// vim: filetype=ragel

%%{

  machine pack;

  include "pack_actions.rl";

  ignored = (space | 0)*;

  count = digit >start_digit digit* @count;

  count_modifier    = '*' %rest | count?;
  platform_modifier = ([_!] %platform)? count_modifier;
  modifier          = count_modifier | [_!] @non_native_error;

  comment = '#' [^\n]* '\n'?;

  # Integers
  S = (('S' | 's') platform_modifier) %check_size %S;
  I = (('I' | 'i') platform_modifier) %check_size %I;
  L = (('L' | 'l') platform_modifier) %check_size %L;

  C = (('C' | 'c') modifier) %check_size %C;
  n = ('n'         modifier) %check_size %n;
  N = ('N'         modifier) %check_size %N;
  v = ('v'         modifier) %check_size %v;
  V = ('V'         modifier) %check_size %V;
  Q = (('Q' | 'q') modifier) %check_size %Q;

  # Floats
  D = (('D' | 'd') modifier) %check_size %D;
  E = (('E'      ) modifier) %check_size %E;
  e = (('e'      ) modifier) %check_size %e;
  F = (('F' | 'f') modifier) %check_size %F;
  G = (('G'      ) modifier) %check_size %G;
  g = (('g'      ) modifier) %check_size %g;

  # Moves
  X  = ('X' modifier) %X;
  x  = ('x' modifier) %x;
  at = ('@' modifier) %at;

  # Strings
  A = ('A' modifier) %string_check_size %to_str_nil %string_append %A;
  a = ('a' modifier) %string_check_size %to_str_nil %string_append %a;
  Z = ('Z' modifier) %string_check_size %to_str_nil %string_append %Z;

  # Encodings
  B = ('B' modifier) %string_check_size %to_str_nil %B;
  b = ('b' modifier) %string_check_size %to_str_nil %b;
  H = ('H' modifier) %string_check_size %to_str_nil %H;
  h = ('h' modifier) %string_check_size %to_str_nil %h;
  M = ('M' modifier) %string_check_size %to_s %M;
  m = ('m' modifier) %string_check_size %b64_uu_size %to_str %m;
  U = ('U' modifier) %check_size %U;
  u = ('u' modifier) %string_check_size %b64_uu_size %to_str %u;
  w = ('w' modifier) %check_size %w;

  integers  = C | S | I | L | n | N | v | V | Q;
  floats    = D | E | e | F | G | g;
  encodings = B | b | H | h | M | m | U | u | w;
  strings   = A | a | Z;
  moves     = X | x | at;

  directives = integers | strings | encodings | moves | floats;

  main := ((directives >start) | comment | ignored)** %done;

  write data nofinal noerror noprefix;
  write init;
  write exec;

}%%
