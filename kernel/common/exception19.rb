# -*- encoding: us-ascii -*-

class Exception
  def ==(other)
    other.kind_of?(Exception) &&
      message == other.message &&
      backtrace == other.backtrace
  end

  def to_s
    if @reason_message
      @reason_message.to_s
    else
      self.class.to_s
    end
  end

  # This is here rather than in yaml.rb because it contains "private"
  # information, ie, the list of ivars. Putting it over in the yaml
  # source means it's easy to forget about.
  def to_yaml_properties
    list = super
    list.delete :@backtrace
    list.delete :@custom_backtrace
    return list
  end
end
