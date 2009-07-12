require "#{File.dirname(__FILE__)}/../../kernel/compiler/iseq"

puts "template <class SubClass>"
puts "void VisitInstructions::drive() {"
puts "  switch(current_instruction()) {"

InstructionSet.opcodes.each do |ins|
  puts "  case #{ins.bytecode}: // #{ins.opcode}"
  case ins.arg_count
  when 2
    puts "    SPECIFIC->visit_#{ins.opcode}(opcode_arg(0), opcode_arg(1));"
  when 1
    puts "    SPECIFIC->visit_#{ins.opcode}(opcode_arg(0));"
  when 0
    puts "    SPECIFIC->visit_#{ins.opcode}();"
  else
    raise "huh?"
  end
  puts "    advance(#{ins.arg_count + 1});"
  puts "    return;"
end

puts "  }"

puts "}"

InstructionSet.opcodes.each do |ins|
  puts "template <class SubClass>"
  case ins.arg_count
  when 2
    puts "void VisitInstructions::visit_#{ins.opcode}(opcode arg1, opcode arg2)"
    puts "{\n  SPECIFIC->visit(#{ins.bytecode}, arg1, arg2);\n}"
  when 1
    puts "void VisitInstructions::visit_#{ins.opcode}(opcode arg1)"
    puts "{\n  SPECIFIC->visit(#{ins.bytecode}, arg1, -1);\n}"
  when 0
    puts "void VisitInstructions::visit_#{ins.opcode}()"
    puts "{\n  SPECIFIC->visit(#{ins.bytecode}, -1, -1);\n}"
  else
    raise "huh?"
  end

  puts

end
