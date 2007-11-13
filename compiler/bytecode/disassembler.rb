require 'bytecode/encoder'
require 'digest/sha1'

class MarshalEmitter
  def initialize(str, start=0)
    @string = str
    @index = start
  end
  
  TagNames = {
    ?n => :nil,
    ?t => :true,
    ?f => :false,
    ?i => :int,
    ?s => :string,
    ?d => :float,
    ?B => :bignum,
    ?x => :symbol,
    ?b => :bytes,
    ?I => :instructions,
    ?p => :tuple,
    ?m => :method,
    ?r => :object
  }
  
  NoBody = [:nil, :true, :false]
  
  def self.process_rbc(file)
    # Binary mode needs to be specified on Win
    str = ""
    File.open(file, 'rb') do |f|
      str = f.read
    end
    raise "Not a Rubinius compiled file" unless 'RBIX' == str[0..3]
    ver = str[4..7].unpack("N").first
    raise "Unsupported .rbc version #{ver}" unless 5 == ver
    new(str, 8)
  end
  
  def digest
    dig = Digest::SHA1.new
    dig.update @string
    return dig.hexdigest
  end
  
  def process
    tag = @string[@index]
    @index += 1
    
    name = TagNames[tag]
    raise "Unrecognised tag '" << (tag || '\0') << "' at #{@index} (#{sprintf('%#x', @index)})" unless name
        
    if NoBody.include? name
      [name]
    else
      body = __send__ "process_#{name}"
      [name, body]
    end
  end
  
  def process_int
    body = @string[@index, 5]
    @index += 5
    sign = body[0]
    int = body[1..-1].unpack("N").first
    if sign == ?n
      int = -int
    end
    return [body, int]
  end
  
  def process_string
    sz = @string[@index,4].unpack("N").first
    @index += 4
    body = @string[@index, sz]
    @index += sz
    return body
  end
  
  alias :process_bignum :process_string
  alias :process_float  :process_string
  alias :process_symbol :process_string
  alias :process_bytes  :process_string
  
  def process_tuple
    sz = @string[@index,4].unpack("N").first
    @index += 4
    body = []
    sz.times do
      body << process()
    end
    
    body
  end
  
  alias :process_method :process_tuple
  alias :process_object :process_tuple
  
  def process_instructions
    endian = @string[@index]
    @index += 1
    body = process_string()
    dis = Bytecode::Disassembler.new(body, endian)
    dis.process_all()
  end  
end

module Bytecode
  class Disassembler
    def initialize(string, endian=?b)
      @string = string
      @endian = endian
      @index = 0
      @opcodes = Bytecode::InstructionEncoder::OpCodes
      @one = Bytecode::InstructionEncoder::IntArg
      @two = Bytecode::InstructionEncoder::TwoInt
      @codes = []
    end
    
    attr_reader :codes
    
    def num_args(bc)
      return 2 if @two.index(bc)
      return 1 if @one.index(bc)
      return 0
    end
    
    def get_dword
      if ?b == @endian
        # Instructions are encoded big-endian
        dw = @string[@index, 4].unpack("N").first
      else
        # Instructions are encoded little-endian
        dw = (@string[@index] | (@string[@index+1] << 8) | (@string[@index+2] << 16) | (@string[@index+3] << 24))
      end
      @index += 4
      dw
    end

    def process
      bc = get_dword
      kind = @opcodes[bc]
      code = [kind]
      num_args(kind).times do
        code << get_dword
      end
      @codes << code
    end
    
    def process_all
      while @index < @string.size
        process
      end
      @codes
    end
  end
end

emit = MarshalEmitter.process_rbc(ARGV.shift)

require 'pp'

puts emit.digest
pp emit.process
