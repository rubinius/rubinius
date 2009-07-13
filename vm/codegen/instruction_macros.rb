module Rubinius
  LookupTable = Hash
end

require "#{File.dirname(__FILE__)}/../../kernel/compiler/iseq"

dir = "#{File.dirname(__FILE__)}/../gen/"

unless File.directory? dir
  Dir.mkdir dir
end

File.open "#{dir}/inst_list.hpp", "w" do |f|
  Rubinius::InstructionSet.opcodes.each do |ins|
    case ins.arg_count
    when 2
      f.puts "HANDLE_INST2(#{ins.bytecode}, #{ins.opcode});"
    when 1
      f.puts "HANDLE_INST1(#{ins.bytecode}, #{ins.opcode});"
    when 0
      f.puts "HANDLE_INST0(#{ins.bytecode}, #{ins.opcode});"
    end
  end
end

File.open "#{dir}/inst_stack.hpp", "w" do |f|
  f.puts "static inline int stack_difference(opcode op, opcode operand1 = 0, opcode operand2 = 0) {"
  f.puts "switch(op) {"
  Rubinius::InstructionSet::OpCodes.each do |op|
    f.puts "case InstructionSequence::insn_#{op.opcode}:"
    if variable = op.variable_stack
      extra, position = variable
      f.puts "  return #{op.stack_produced - extra} - operand#{position};"
    else
      f.puts "  return #{op.stack_difference(nil)};"
    end
  end
  f.puts "}"
  f.puts "abort();"
  f.puts "return 0;"
  f.puts "}"
end
