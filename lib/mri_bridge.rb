# Defines a few fundamental constants that are needed on MRI to run the
# compiler. Also requires files that are available by default in Rubinius.

module Rubinius
  LookupTable = Hash
  Tuple = Array

  class Executable
    attr_accessor :primitive
  end
end

class Array
  def to_tuple
    Rubinius::Tuple.new self
  end
end

require File.expand_path("../../kernel/common/compiled_method", __FILE__)
require File.expand_path("../../kernel/delta/options", __FILE__)
