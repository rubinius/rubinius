module Rubinius
  
  class ARGFClass
    
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