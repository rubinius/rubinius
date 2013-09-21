module Rubinius
  class AgentRegistry
    def self.primary
      @primary ||= new
    end

    def self.get(var, &blk)
      primary.get(var, &blk)
    end

    def self.set(var, &blk)
      primary.set(var, &blk)
    end

    def initialize
      @get = Hash.new
      @set = Hash.new
    end

    def get(var, &blk)
      @get[var] = blk
    end

    def set(var, &blk)
      @set[var] = blk
    end

    def run_get(var)
      if blk = @get[var]
        return :ok, blk.call(var)
      else
        return :error, "unknown variable: #{var}"
      end
    end

    def run_set(var, val)
      if blk = @set[var]
        return :ok, blk.call(var, val)
      else
        return :error, "unknown variable: #{var}"
      end
    end
  end
end
