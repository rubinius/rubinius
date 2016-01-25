module Rubinius
  class CallCustomCache < CallSite

    attr_reader :name
    attr_reader :executable
    attr_reader :call_unit

    def inspect
      "#<#{self.class.name}:0x#{self.object_id.to_s(16)} #{location}##{@name} call_unit=#{@call_unit}>"
    end

  end
end

