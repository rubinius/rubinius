
require 'rubygems'

$:.push "lib"
require 'ruby_parser'
$:.pop

module Rubinius
  class Executable
    attr_accessor :serial
    attr_accessor :primitive
  end
end

module Rubinius
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

  LookupTable = Hash
end

$: << File.expand_path(File.dirname(__FILE__) + '/../../kernel')
require 'compiler/blocks'
require 'compiler/blocks_graph'
require 'compiler/compiler'
require 'compiler/node'
require 'compiler/nodes'
require 'compiler/iseq'
require 'compiler/opcodes'
require 'compiler/generator'
require 'compiler/bytecode'
require 'compiler/compiled_file'
require 'compiler/execute'
require 'compiler/text'
require 'compiler/graph'
require 'compiler/local'
require 'compiler/plugins'
require 'compiler/stack'
require 'common/compiled_method'
$:.pop

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

