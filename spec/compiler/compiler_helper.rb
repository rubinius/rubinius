
require File.dirname(__FILE__) + '/../spec_helper'

require 'bytecode/compiler'
require 'bytecode/rubinius'

class CompilerHarness

  def initialize(show=false)
    @compiler = Bytecode::Compiler.new
    @show = show
  end

  attr_accessor :show
  
  def encode(str)
    state = RsLocalState.new
    sexp = str.to_sexp
    desc = @compiler.compile_as_method(sexp, :__spec_script__, state)
    if @show
      puts ""
      puts desc.assembly
    end
    return desc.assembly
  end

end

