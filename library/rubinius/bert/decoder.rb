module BERT
  class Decoder
    # Decode a BERT into a Ruby object.
    #   +bert+ is the BERT String
    #
    # Returns a Ruby object
    def self.decode(bert)
      Decode.decode(bert)
    end
  end
end