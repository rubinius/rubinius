##
# Platform specific behavior for the File class.

module Rubinius
module FFI::Platform::File
  SEPARATOR = '/'
  ALT_SEPARATOR = nil
  PATH_SEPARATOR = ':'
end
end
