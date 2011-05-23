module ProcSpecs
  module SourceLocation
    def self.simple_proc
      proc do # line 4
        # nothing
      end
    end

    def self.stashed_block(&block)
      @stashed_block ||= block
      @stashed_block
    end

    stashed_block do # line 14
      # nothing
    end
  end
end