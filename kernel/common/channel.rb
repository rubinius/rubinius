# -*- encoding: us-ascii -*-

##
# A communication mechanism based on pi-calculus channels used primarily to
# communicate between ruby and the VM about events.

module Rubinius
  class Channel
    def inspect
      "#<Rubinius::Channel:0x#{object_id.to_s(16)}>"
    end
  end
end
