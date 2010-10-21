##
# A communication mechanism based on pi-calculus channels used primarily to
# communicate between ruby and the VM about events.

module Rubinius
  class Channel
    def inspect
      "#<Rubinius::Channel:0x%x @waiting=%p @value=%p>" % [object_id, waiting, value]
    end

    def as_lock(val=nil)
      receive

      begin
        yield
      ensure
        self << val
      end
    end
  end
end
