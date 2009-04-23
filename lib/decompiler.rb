require 'kernel/core/iseq' unless defined? RUBY_ENGINE and RUBY_ENGINE == 'rbx'


# Like String#unpack('N'), but returns a Fixnum, rather than an array containing a string
class String
  def unpack_int(endian = ?b)
    if ?b == endian
      # Int is encoded big-endian
      i = (self[3] | (self[2] << 8) | (self[1] << 16) | (self[0] << 24))
    else
      # Int is encoded little-endian
      i = (self[0] | (self[1] << 8) | (self[2] << 16) | (self[3] << 24))
    end
    i
  end
end



class MarshalEmitter
  def initialize(ver, str, start=0)
    @rbc_version = ver
    @string = str
    @index = start
    @decoder = Rubinius::InstructionSequence::Encoder.new
  end

  attr_reader :rbc_version

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
    ?A => :array,
    ?m => :method,
    ?M => :method2,
    ?r => :object,
    ?S => :send_site
  }

  NoBody = [:nil, :true, :false]

  def self.process_rbc(file)
    # Binary mode needs to be specified on Win
    str = ""
    File.open(file, 'rb') do |f|
      str = f.read
    end
    raise "Not a Rubinius compiled file" unless 'RBIX' == str[0..3]
    ver = str[4..7].unpack_int

    return new(ver, str, 28)
  end

  def process
    tag = @string[@index]
    @index += 1

    name = TagNames[tag]
    raise "Unrecognised tag '" << (tag || '\0') << "' at #{@index} (#{sprintf('%#x', @index)})" unless name

    if NoBody.include? name
      name
    else
      body = __send__ "process_#{name}"
      if name == :bignum
        body.to_i
      else
        [name, body]
      end
    end
  end

  def process_int
    body = @string[@index, 5]
    @index += 5
    sign = body[0]
    int = body[1..-1].unpack_int
    if sign == ?n
      int = -int
    end
    return int
  end

  def process_string
    sz = @string[@index,4].unpack_int
    @index += 4
    body = @string[@index, sz]
    @index += sz
    return body
  end

  def process_num
    body = process_string
    @index += 1  # Discard trailing \0
    body
  end

  alias :process_float     :process_num
  alias :process_bignum    :process_num
  alias :process_symbol    :process_string
  alias :process_bytes     :process_string
  alias :process_send_site :process_string

  def process_tuple
    sz = @string[@index,4].unpack_int
    @index += 4
    body = []
    sz.times do
      body << process()
    end

    body
  end

  alias :process_method :process_tuple
  alias :process_object :process_tuple

  # Support for version 2 of compiled method, which replaces size with a version number
  def process_method2
    ver = @string[@index,4].unpack_int
    @index += 4
    body = []
    sz = 16 if 1 == ver
    raise "Unsupported version (#{ver}) of CompiledMethod" unless sz

    sz.times do
      body << process()
    end

    body
  end

  def process_instructions
    endian = @string[@index]
    @index += 1
    body = process_string()
    body = @decoder.decode_iseq(body)
    body.map! {|i| [i.first.opcode].concat i[1..-1]}
    body
  end
end

# If file is run, dump content of .rbc to STDOUT
if __FILE__ == $0
  if ARGV.size > 0
    require 'pp'
    while rbc = ARGV.shift
      emit = MarshalEmitter.process_rbc(rbc)
      STDOUT.puts "\nContent of #{rbc}:"
      pp emit.process
    end
  else
    STDOUT.puts "Usage: #{__FILE__} <rbc_file> [<rbc_file> ...]"
  end
end
