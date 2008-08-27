# depends on: class.rb

##
# A communication mechanism based on pi-calculus channels used primarily to
# communicate between ruby and the VM about events.

class Channel
  alias_method :<<, :send

  def inspect
    "#<Channel:0x%x @waiting=%p @value=%p>" % [object_id, waiting, value]
  end
end
