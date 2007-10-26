load_path = ["runtime/compiler", "lib", "stdlib", "."]
loaded_features = []

Globals = {
  :$; => nil, 
  :$/ => "\n",                # Input record separator 
  :$\ => nil,                 # Output record separator
  :$> => STDOUT, 
  :$: => load_path,
  :$LOAD_PATH => load_path,
  :$" => loaded_features,
  :$LOADED_FEATURES => loaded_features,
  :$, => '',                  # Output field separator
  :$stderr => STDERR, 
  :$stdout => STDOUT,
  :$CONSOLE => STDOUT,
  :$DEBUG => false
}
