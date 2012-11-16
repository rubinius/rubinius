# -*- encoding: us-ascii -*-

class Struct
  def self.members
    return self::STRUCT_ATTRS.map { |member| member.to_sym }
  end

  def members
    return self.class.members
  end

  def to_s
    return "[...]" if Thread.guarding? self

    Thread.recursion_guard self do
      values = []

      _attrs.each do |var|
        val = instance_variable_get :"@#{var}"
        values << "#{var}=#{val.inspect}"
      end

      name = self.class.name

      if name.nil? || name.empty?
        "#<struct #{values.join(', ')}>"
      else
        "#<struct #{self.class.name} #{values.join(', ')}>"
      end
    end
  end

  alias_method :inspect, :to_s
end
