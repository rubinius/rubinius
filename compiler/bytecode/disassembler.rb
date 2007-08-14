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
    ?B => :bignum,
    ?x => :symbol,
    ?b => :bytes,
    ?p => :tuple,
    ?m => :method
  }
  
  NoBody = [:nil, :true, :false]
  
  def self.process_rbc(file)
    str = File.read(file)
    new(str, 4)
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
  alias :process_symbol :process_string
  
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
  
  def process_bytes
    body = process_string()
    dis = Bytecode::Disassembler.new(body)
    [body, dis.process_all]
  end
  
end

module Bytecode
  class Disassembler
    def initialize(string)
      @string = string
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
    
    def process
      bc = @string[@index]
      @index += 1
      kind = @opcodes[bc]
      code = [kind]
      num_args(kind).times do
        code << @string[@index, 4].unpack("N").first
        @index += 4
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
