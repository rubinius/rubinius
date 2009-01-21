
require 'rubygems'

$:.push "lib"
require 'ruby_parser'
require 'compiler/text'
require 'compiler/stack'
$:.pop

class Executable
end

require File.dirname(__FILE__) + '/../../kernel/delta/compiled_file'
require File.dirname(__FILE__) + '/../../kernel/delta/iseq'
require File.dirname(__FILE__) + '/../../kernel/common/compiled_method'

module Compile
  def self.compiler=(compiler)
    @compiler = compiler
  end

  def self.compiler
    @compiler
  end
end

Compile.compiler = Compiler

class SendSite
  def initialize(name)
    @name = name
  end

  attr_reader :name
  attr_accessor :sender

  def ==(other)
    other.kind_of?(SendSite) and @name == other.name
  end
end

class Tuple < Array
end

class LookupTable < Hash
end

class Exception
  def awesome_backtrace
    ary = backtrace()
    def ary.show
      join("\n")
    end
    return ary
  end
end

class Object
  def __show__
  end
end


class String
  def to_sexp(file = "(eval)", line = 1)
    RubyParser.new.process(self, file)
  end

  def prefix?(str)
    self =~ /^#{str}/
  end
end

def File.to_sexp(file, opt=nil)
  File.read(file).to_sexp(file)
end

