# -*- encoding: us-ascii -*-

module Rubinius
  class ARGFClass
    def each_codepoint
      return to_enum :each_codepoint unless block_given?

      while c = getc
        yield c.ord
      end

      self
    end
    alias_method :codepoints, :each_codepoint

    #
    # Read all lines from stream.
    #
    # Reads all lines into an Array using #gets and
    # returns the Array.
    #
    # @see  #gets
    #
    def readlines(sep=$/)
      return [] unless advance!

      lines = []
      while line = gets(sep)
        lines << line
      end

      lines
    end

    alias_method :to_a, :readlines
  end
end
