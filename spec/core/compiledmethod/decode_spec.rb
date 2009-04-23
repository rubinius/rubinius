require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "CompiledMethod#decode" do
  before :each do
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
    @code = [[:push_literal, 'hello'],
            [:string_dup], [:set_local, 0], [:pop],
            [:push_self], [:push_local, 0],
            [:set_call_flags, 1], [:send_stack, :puts, 1],
            [:ret]]
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
    @cm.decode[0].args.first.should == 'hello'
  end

  it "#args returns local variable names, rather than the index into the locals tuple" do
    @cm.decode.each_with_index do |inst,i|
      if inst.instruction.args.include? :local
        inst.args.first.inspect.should == @code[i][1].inspect
      end
    end
  end

  it "#stack_consumed returns a count of the stack operands consumed by the instruction" do
    inst = CompiledMethod::Instruction.new([Rubinius::InstructionSet[:noop]], @cm, 0, 0)
    inst.stack_consumed.should == 0
    inst = CompiledMethod::Instruction.new([Rubinius::InstructionSet[:swap_stack]], @cm, 0, 0)
    inst.stack_consumed.should == 1
    inst = CompiledMethod::Instruction.new([Rubinius::InstructionSet[:send_stack],0,2], @cm, 0, 0)
    inst.stack_consumed.should == 3
  end

  it "#stack_produced returns a count of the stack operands produced by the instruction" do
    inst = CompiledMethod::Instruction.new([Rubinius::InstructionSet[:noop]], @cm, 0, 0)
    inst.stack_produced.should == 0
    inst = CompiledMethod::Instruction.new([Rubinius::InstructionSet[:swap_stack]], @cm, 0, 0)
    inst.stack_produced.should == 1
    inst = CompiledMethod::Instruction.new([Rubinius::InstructionSet[:send_stack],0,2], @cm, 0, 0)
    inst.stack_produced.should == 1
  end
end
