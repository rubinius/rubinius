module Kernel
  # Proper kcode support
  get = proc { |key| Rubinius.kcode.to_s }
  set = proc { |key, val| Rubinius.kcode = val }

  Rubinius::Globals.set_hook(:$KCODE, get, set)
end
