# -*- encoding: us-ascii -*-

module Rubinius
  class ARGFClass

    
    def binmode
      self
    end

    #
    # Read all lines from stream.
    #
    # Reads all lines into an Array using #gets and
    # returns the Array.
    #
    # @see  #gets
    #
    def readlines(sep=$/)
      return nil unless advance!

      lines = []
      while line = gets(sep)
        lines << line
      end

      lines
    end

    alias_method :to_a, :readlines


    def default_value
      ""
    end

    def stream(file)
      file == "-" ? STDIN : File.open(file, "r")
    end
  end
end
