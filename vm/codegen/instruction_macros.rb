require "#{File.dirname(__FILE__)}/../../kernel/compiler/iseq"

File.open "#{File.dirname(__FILE__)}/../gen/inst_list.hpp", "w" do |f|
  InstructionSet::OpCodes.each do |ins|
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
