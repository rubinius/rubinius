# -*- encoding: us-ascii -*-

module Rubinius
  class RespondToCache < CallSite

    attr_reader :name
    attr_reader :executable
    attr_reader :receiver_class
    attr_reader :message
    attr_reader :visibility
    attr_reader :responds
    attr_reader :fallback_call_site

    def inspect
      "#<#{self.class.name}:0x#{self.object_id.to_s(16)} #{location}##{@name} receiver_class=#{@receiver_class} message=#{@message} visibility=#{@visibility} responds=#{@responds}>"
    end

  end
end
