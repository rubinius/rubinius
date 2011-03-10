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

    def self.spawn_thread
      return if @thread
      require 'rubinius/bert'

      @thread = Thread.new do

        dec = BERT::Decode.new Rubinius::FROM_AGENT
        enc = BERT::Encode.new Rubinius::TO_AGENT

        while true
          id, op, var, val = dec.read_any_raw

          case op
          when :set
            begin
              code, ret = primary.run_set(var, val)
            rescue Exception => e
              code = :error
              ret = "#{e.message} (#{e.class})"
            end
          when :get
            begin
              code, ret = primary.run_get(var)
            rescue Exception => e
              code = :error
              ret = "#{e.message} (#{e.class})"
            end
          else
            code = :error
            ret = "unknown operation: #{op}"
          end

          enc.write_any_raw BERT::Tuple[id, code, ret]
          Rubinius::TO_AGENT.flush
        end
      end
    end
  end
end
