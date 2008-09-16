
require 'rubygems'
gem 'ParseTree', '>= 2.2'
require 'parse_tree'

$" << "compiler/mri_shim"
$" << "compiler/compiler"

$: << "lib"
require 'compiler/compiler'
require 'compiler/text'
$:.pop

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
  def to_sexp(file = "(eval)", line = 1)
    $: << "lib/compiler"
    require 'ruby_parser'
    $:.pop

    RubyParser.new.process(self, file)
  end
end

def File.to_sexp(file, opt=nil)
  File.read(file).to_sexp(file)
end

