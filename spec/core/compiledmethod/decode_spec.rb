require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "CompiledMethod#decode" do
  before do
    @cm = CompiledMethodSpecs.instance_method(:simple_puts).compiled_method
  end

  it "returns an array of Instruction objects representing the instruction sequence" do
    dc = @cm.decode
    dc.kind_of?(Array).should == true
    dc.each do |inst|
      inst.kind_of?(CompiledMethod::Instruction).should == true
    end
  end
end

describe "CompiledMethod::Instruction" do
  before do
    @cm = CompiledMethodSpecs.instance_method(:simple_puts).compiled_method
    @code = [[:check_argcount, 0, 0], [:push_literal, 'hello'],
            [:string_dup], [:set_local, :a], [:pop], [:push_local, :a],
            [:push_self], [:set_call_flags, 1],
            #[:set_cache_index, 0],
            [:meta_send_stack_1, :puts], [:sret]]
  end

  it "#opcode returns a symbol representing the opcode for the corresponding instruction" do
    @cm.decode.each_with_index do |inst,i|
      inst.opcode.should == @code[i].first
    end
  end

  it "#args returns an array containing the opcode arguments" do
    @cm.decode.each_with_index do |inst,i|
      inst.args.size.should == @code[i].size - 1
    end
  end

  it "#args returns literal values, rather than the index into the literals tuple" do
    @cm.decode[1].args.first.should == 'hello'
  end

  it "#args returns local variable names, rather than the index into the locals tuple" do
    @cm.decode.each_with_index do |inst,i|
      if inst.instruction.args.include? :slot_local
        inst.args.first.inspect.should == @code[i][1].inspect
      end
    end
  end

end
