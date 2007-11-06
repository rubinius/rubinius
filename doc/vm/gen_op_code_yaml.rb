$: << '../../compiler'
require 'bytecode/encoder'
require 'bytecode/assembler'
require 'yaml'

# Read shotgun/lib/instructions.rb, stripping out just the ShotgunInstruction class definition
# Code at the end of the class generates instructions* files, which we don't want to do here
code = ""; output = false
File.foreach('../../shotgun/lib/instructions.rb') do |line|
  output ||= (line =~ /^class ShotgunInstructions$/)
  code << line if output
  output &&= !(line =~ /^end$/)
end
eval code

op_codes = Bytecode::InstructionEncoder::OpCodes
int_arg = Bytecode::InstructionEncoder::IntArg
two_int = Bytecode::InstructionEncoder::TwoInt
instructs = ShotgunInstructions.new
translations = Bytecode::Assembler::Translations.invert

def output_array(ary, output)
  ary.each do |item| 
    case item
    when String
      output << "  - #{item}\n"
    when Array
      output << "  -\n    - "
      output << item.join("\n    - ")
      output << "\n"
    when Hash
      output << "  -\n"
      item.each do |key,val|
        output << "    #{key}: #{val}\n"
      end
    end   
  end
end

# Folds a long string
def fold(str, width = 80)
  return '' if str.nil? || str.empty?
  fold_str = str[0,2] == "  " ? str : str.insert(0, "  ")
  fold_sep = "\n  "
  fold_str.gsub!(Regexp.new("(.{1,#{width-2}})(\s|$)")) {|match| match.rstrip << "\n"}
  fold_str.rstrip!
  fold_str.gsub!("\n", "\n  ")
  fold_str
end

# Generate a YAML file for each op code, retaining any existing
# operation, description, and notes
op_codes.each_with_index do |op,idx|
  # Load existing YAML document
  if File.exists?("op_codes/#{op.to_s}.yaml")
    prev = YAML::load(File.open("op_codes/#{op.to_s}.yaml"))
  else
    prev = {}
  end

  if prev['format']
    args = prev['format']
  else
    args = op.to_s
    args << " <arg1>" if int_arg.include? op
    args << "<arg1> <arg2>" if two_int.include? op
  end

  stack_before = prev['stack_before'] || ["<consumed>", "..."]
  stack_after = prev['stack_after'] || ["<produced>", "..."]  

  # Create the YAML file; hand-crafted due to need to control folding
  op_doc = "---\n"
  op_doc << "mnemonic: #{op.to_s}\n"
  op_doc << "alias: #{translations[op].to_s}\n" if translations[op]
  op_doc << "operation: >-\n"
  op_doc << fold(prev['operation']) << "\n"
  op_doc << "format: #{args}\n"
  op_doc << "opcode: #{idx}\n"
  op_doc << "stack_before:\n"
  output_array stack_before, op_doc
  op_doc << "stack_after:\n"
  output_array stack_after, op_doc
  unless prev['description']
    op_doc << "description: >-\n\n"
  end

  # Handle any additional tags added
  (prev.keys - ['mnemonic', 'alias', 'operation', 'format', 'opcode', 'stack_before',
    'stack_after', 'source']).each do |key|
    op_doc << key << ":"
    case prev[key]
    when String
      op_doc << " >-\n"
      op_doc << fold(prev[key]) << "\n"
    when Array
      op_doc << "\n"
      prev[key].each {|i| op_doc << "  - #{i}\n" }
    when Hash
      op_doc << "\n"
      prev[key].each {|k,v| op_doc << "  #{k}: #{v}\n" }
    end
  end
  
  # Output the source code from instructs.rb
  if instructs.methods.include? op.to_s
    op_doc << "source: |-\n"
    source = instructs.send(op)
    op_doc << "  " if source[0,1] != ' '
    op_doc << source
  end
  
  File.open("op_codes/#{op.to_s}.yaml", 'w') do |f|
    f.puts op_doc
  end
end
