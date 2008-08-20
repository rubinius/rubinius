
$" << "compiler/mri_shim"
$" << "compiler/compiler"

$:.unshift File.dirname(__FILE__) + "/.."

require 'compiler/compiler'
require 'compiler/text'
require 'compiler/marshal'
require 'compiler/compiled_file'


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

require File.dirname(__FILE__) + '/../../kernel/core/iseq'
require File.dirname(__FILE__) + '/../../kernel/core/compiled_method'
require File.dirname(__FILE__) + '/../../vm/gen/simple_field'

def File.to_sexp(file, opt=nil)
  $: << File.expand_path "~/Work/p4/zss/src/ruby_parser/dev/lib/" # HACK
  require 'ruby_parser'

  top = RubyParser.new(true).process(File.read(file), file)
  raise "huh?" if top.size != 1
  top
end

