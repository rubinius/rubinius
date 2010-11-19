require 'rubinius/debugger'

warn "debugger.rb is deprecated, use rubinius/debugger and Rubinius::Debugger"

# To be deprecated in 1.2+
Debugger = Rubinius::Debugger
