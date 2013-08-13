# -*- encoding: us-ascii -*-

module Rubinius
  class ARGFClass

    #
    # Set stream into binary mode.
    #
    # Stream is set into binary mode, i.e. 8-bit ASCII.
    # Once set, the binary mode cannot be undone. Returns
    # self.
    #
    def binmode
      @binmode = true
      @external = Encoding::ASCII_8BIT
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
      return [] unless advance!

      lines = []
      while line = gets(sep)
        lines << line
      end

      lines
    end

    alias_method :to_a, :readlines

    def binmode?
      @binmode
    end

    def encoding
      @external || Encoding.default_external
    end

    def default_value
      "".encode(encoding)
    end

    def stream(file)
      file == "-" ? STDIN : File.open(file, "r", :external_encoding => encoding)
    end

  end
end
