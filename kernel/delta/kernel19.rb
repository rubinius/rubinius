# -*- encoding: us-ascii -*-

module Kernel
  get = proc do
    warn "$KCODE is unused in Ruby 1.9"
    nil
  end

  set = proc do |key, io|
    warn "$KCODE is unused in Ruby 1.9, changes are ignored"
    nil
  end

  Rubinius::Globals.set_hook(:$KCODE, get, set)
end
