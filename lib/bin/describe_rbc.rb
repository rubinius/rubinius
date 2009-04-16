class Reader
  def initialize(io)
    @io = io
  end

  def next_line
    @io.gets
  end

  def next_byte
    @io.getc
  end

  def next_string(length)
    @io.read length
  end

  ##
  # Decodes an unsigned varint to a positive integer.
  def next_varint
    val = 0
    shift = 0

    loop do
      byte = next_byte

      val += (byte & ~128) << shift
      break if byte < 128

      shift += 7
    end

    val
  end
end

f = File.open(ARGV.first, 'rb')
reader = Reader.new f

# magic
puts reader.next_line

# version
puts reader.next_line

# checksum
puts reader.next_line

loop do
  type = reader.next_byte

  # did we hit eof?
  break if type.nil?

  puts type.chr

  case type
  when ?I
    puts reader.next_varint
  when ?J
    puts -reader.next_varint
  when ?d
    puts reader.next_line
  when ?s
    length = reader.next_varint
    puts length

    puts reader.next_string(length)
  when ?x
    length = reader.next_varint
    puts length

    s = reader.next_string(length)
    puts s.to_sym
  when ?S
    length = reader.next_varint
    puts length

    puts reader.next_string(length)
  when ?A, ?p
    puts reader.next_varint
  when ?i
    length = reader.next_varint
    puts length

    length.times do
      puts reader.next_varint
    end
  when ?M
    puts reader.next_varint
  end
end
