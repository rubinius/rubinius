##
# A communication mechanism based on pi-calculus channels used primarily to
# communicate between ruby and the VM about events.

module Rubinius
  class Channel
    def inspect
      "#<Channel:0x%x @waiting=%p @value=%p>" % [object_id, waiting, value]
    end
  end
end
