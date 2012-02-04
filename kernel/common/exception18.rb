# -*- encoding: us-ascii -*-

class Exception
  alias_method :to_str, :message

  def to_s
    @reason_message || self.class.to_s
  end

  # This is here rather than in yaml.rb because it contains "private"
  # information, ie, the list of ivars. Putting it over in the yaml
  # source means it's easy to forget about.
  def to_yaml_properties
    list = super
    list.delete "@backtrace"
    list.delete "@custom_backtrace"
    return list
  end
end
