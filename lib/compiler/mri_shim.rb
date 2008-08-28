
require 'rubygems'
gem 'ParseTree', '>= 2.2'
require 'parse_tree'

$" << "compiler/mri_shim"
$" << "compiler/compiler"

$:.unshift File.dirname(__FILE__) + "/.."

require 'compiler/compiler'
require 'compiler/text'
require 'compiler/marshal'
require 'kernel/delta/compiled_file'

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

require File.dirname(__FILE__) + '/../../kernel/delta/iseq'
require File.dirname(__FILE__) + '/../../kernel/common/compiled_method'
require File.dirname(__FILE__) + '/../../vm/gen/simple_field'

class String
  def to_sexp(file = "(string)")
    if ENV['RUBY_PARSER'] then
      $: << File.expand_path("~/Work/p4/zss/src/ruby_parser/dev/lib/") # HACK
      require 'ruby_parser'

      sexp = RubyParser.new.process(self, file)
      p sexp
      sexp
    else
      pt = ParseTree.new(true)
      top = pt.parse_tree_for_string(File.read(file), file)
      if top.size != 1
        raise "huh?"
      end
      top[0]
    end
  end
end

def File.to_sexp(file, opt=nil)
  File.read(file).to_sexp(file)
end

