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
  end
end
