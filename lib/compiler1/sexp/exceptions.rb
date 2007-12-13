module Compiler1

##
# SexpProcessor base exception class.

class SexpProcessorError < StandardError; end

##
# Raised by SexpProcessor if it sees a node type listed in its
# unsupported list.

class UnsupportedNodeError < SexpProcessorError; end

##
# Raised by SexpProcessor if it is in strict mode and sees a node for
# which there is no processor available.

class UnknownNodeError < SexpProcessorError; end

##
# Raised by SexpProcessor if a processor did not process every node in
# a sexp and @require_empty is true.

class NotEmptyError < SexpProcessorError; end

##
# Raised if assert_type encounters an unexpected sexp type.

class SexpTypeError < SexpProcessorError; end

end
