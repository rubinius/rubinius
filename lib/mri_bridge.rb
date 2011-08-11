# Defines a few fundamental constants that are needed on MRI to run the
# compiler. Also requires files that are available by default in Rubinius.

module Rubinius
  RUBY_LIB_VERSION = 18

  LookupTable = Hash
  class Tuple < Array; end

  class Executable
    attr_accessor :primitive
  end

  def self.ruby18?
    RUBY_VERSION =~ /^1\.8/
  end

  def self.ruby19?
    RUBY_VERSION =~ /^1\.9/
  end

  def self.ruby20?
    RUBY_VERSION =~ /^2\.0/
  end

  class Channel
    def receive
    end

    def <<(val)
    end

    def as_lock
      yield
    end
  end
end

class Array
  def to_tuple
    Rubinius::Tuple.new self
  end
end

class String
  alias_method :append, :<<
end

class SyntaxError
  def self.from(message, column, line, code, file)
    message << " #{file}:#{line}:#{column}\n  #{code}"
    SyntaxError.new message
  end
end

require File.expand_path("../../kernel/common/compiled_method", __FILE__)
require File.expand_path("../../kernel/delta/options", __FILE__)
