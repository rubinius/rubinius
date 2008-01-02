# depends on: class.rb

class Channel
  alias_method :<<, :send

  def inspect
    "#<Channel:0x%x @waiting=%p @value=%p>" % [object_id, @waiting, @value]
  end
end
