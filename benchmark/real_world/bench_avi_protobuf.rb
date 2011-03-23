require 'benchmark'
require 'benchmark/ips'

require 'stringio'
require 'base64'

cls = Class.new do
  def read_varint(io)
    value = index = 0
    begin
      byte = io.readchar
      value |= (byte & 0x7f) << (7 * index)
      index += 1
    end while (byte & 0x80).nonzero?
    value
  end

  def decode_base64_protobuf(string)
    values = []
    io = StringIO.new(Base64.decode64(string))  
    until io.eof?
      bits = read_varint(io)
      values[(bits >> 3) - 1] = 
        case bits & 0x07
        when 0; read_varint(io)
        when 2; io.read(read_varint(io))
        end
    end
    values
  end
end

input = "CKeTAxCmmfRHGK6trLv1prbkEiCglsTnBDgBMAA="
obj = cls.new

Benchmark.ips do |x|
  x.report "decoding protobuf" do
    obj.decode_base64_protobuf(input)
  end
end

