module Rubinius
  class ARGFClass
    def each_line(&block)
      while stream = next_stream
        stream.each_line(&block)
      end
    end

    def each(&block)
      each_line &block
    end

    def each_byte(&block)
      while stream = next_stream
        stream.each_byte(&block)
      end
    end
  end
end