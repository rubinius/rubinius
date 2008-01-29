$: << File.dirname(__FILE__) + '/../..'
require 'kernel/core/iseq'
require 'yaml'


# Read shotgun/lib/instructions.rb, stripping out just the ShotgunInstruction class definition
# Code at the end of the class generates instructions* files, which we don't want to do here
code = ""; output = false
File.foreach(File.dirname(__FILE__) + '/../../shotgun/lib/instructions.rb') do |line|
  output ||= (line =~ /^class ShotgunInstructions$/)
  code << line if output
  output &&= !(line =~ /^end$/)
end
eval code


module OpCode
  # Class containing details about an op code
  # Some information is gleaned from Rubinius source files, while
  # descriptive info comes from opcode YAML files.
  class Info
    @@instructs = ShotgunInstructions.new

    def self.op_codes
      InstructionSet::OpCodes
    end

    attr_reader :mnemonic, :byte_code, :arg_count, :source

    def initialize(mnemonic)
      @op_code = InstructionSet[mnemonic.to_s.intern]
      @mnemonic = @op_code.opcode

      f = "op_codes/#{@mnemonic.to_s}.yaml"
      if File.exist? f
        YAML::load_documents(File.open(f)) do |op|
          @yaml = op
        end
      else
        @yaml = {}
      end
      @byte_code = @op_code.bytecode
      @arg_count = @op_code.arg_count
      @source = @@instructs.send(@mnemonic) if @@instructs.methods.include? @mnemonic.to_s
    end
    
    def args
      fmt = (@yaml['format'] || @mnemonic.to_s).split(' ')
      fmt.shift
      fmt <<= @op_code.args[0] if fmt.size == 0 && arg_count >= 1
      fmt <<= @op_code.args[1] if fmt.size == 1 && arg_count == 2
      fmt
    end

    def method_missing(method, *args)
      @yaml[method.to_s]
    end
    
    def to_s
      @mnemonic.to_s
    end
    
    def <=>(b)
      to_s <=> b.to_s
    end
  end
end
