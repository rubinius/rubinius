module Rubinius
  class ARGFClass
    def each_line(&block)
      return to_enum :each_line unless block_given?
      while stream = next_stream
        stream.each_line(&block)
      end
    end

    def each(&block)
      return to_enum :each unless block_given?
      each_line &block
    end

    def each_byte(&block)
      return to_enum :each_byte unless block_given?
      while stream = next_stream
        stream.each_byte(&block)
      end
    end
  end
end