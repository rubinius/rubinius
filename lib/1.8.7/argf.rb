module Rubinius
  class ARGFClass
    def bytes
      to_enum :each_byte
    end

    def chars
      to_enum :each_char
    end

    def each_line(&block)
      return to_enum :each_line unless block_given?
      while stream = next_stream
        stream.each_line(&block)
      end
      self
    end

    def each(&block)
      return to_enum :each unless block_given?
      each_line &block
      self
    end

    def each_byte(&block)
      return to_enum :each_byte unless block_given?
      while stream = next_stream
        stream.each_byte(&block)
      end
      self
    end

    def each_char(&block)
      return to_enum :each_char unless block_given?
      while stream = next_stream
        stream.each_char(&block)
      end
      self
    end

    def getbyte
      stream = next_stream
      stream.getbyte if stream
    end

    def lines(*args)
      to_enum :each_line, *args
    end

    def readbyte
      stream = next_stream
      raise EOFError unless stream
      stream.readbyte
    end

  end
end