# -*- encoding: us-ascii -*-

class Struct
  def select
    to_a.select do |v|
      yield v
    end
  end
  
  def to_s
    return "[...]" if Thread.guarding? self

    Thread.recursion_guard self do
      values = []

      _attrs.each do |var|
        val = instance_variable_get :"@#{var}"
        values << "#{var}=#{val.inspect}"
      end

      "#<struct #{self.class.inspect} #{values.join(', ')}>"
    end
  end

  alias_method :inspect, :to_s
end
