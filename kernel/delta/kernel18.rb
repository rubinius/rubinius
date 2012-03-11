# -*- encoding: us-ascii -*-

module Kernel
  # Proper kcode support
  get = proc { |key| Rubinius.kcode.to_s }
  set = proc { |key, val| Rubinius.kcode = val }

  Rubinius::Globals.set_hook(:$KCODE, get, set)

  get = proc do
    warn "$defout is obsolete; it will be removed any day now"
    $stdout
  end

  set = proc do |key, io|
    warn "$defout is obsolete; it will be removed any day now"
    $stdout = io
  end

  Rubinius::Globals.set_hook(:$defout, get, set)

  get = proc do
    warn "$deferr is obsolete; it will be removed any day now"
    $stderr
  end

  set = proc do |key, io|
    warn "$deferr is obsolete; it will be removed any day now"
    $stderr = io
  end

  Rubinius::Globals.set_hook(:$deferr, get, set)
end
