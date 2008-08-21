class Compiler
  class CompiledFile
    def initialize(magic, ver, sum, stream=nil)
      @magic, @version, @sum = magic, ver, sum
      @stream = stream
      @data = nil
    end

    attr_reader :magic, :version, :sum, :stream

    def self.load(stream)
      magic = stream.gets.strip
      version = Integer(stream.gets.strip)
      sum = stream.gets.strip

      return new(magic, version, sum, stream)
    end

    def encode_to(stream, body)
      stream.puts @magic
      stream.puts @version.to_s
      stream.puts @sum.to_s

      mar = Compiler::Marshal.new
      stream << mar.marshal(body)
    end

    def body
      return @data if @data

      mar = Compiler::Marshal.new
      @data = mar.unmarshal(stream)
    end
  end
end

