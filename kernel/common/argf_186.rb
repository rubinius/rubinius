module Rubinius
  class ARGFClass
    def each_line(&block)
      while stream = next_stream
        stream.each_line(&block)
      end
      self
    end

    def each(&block)
      each_line &block
      self
    end

    def each_byte(&block)
      while stream = next_stream
        stream.each_byte(&block)
      end
      self
    end
  end
end