class String
  def self.try_convert(obj)
    return nil unless obj.respond_to?(:to_str)
    Type.coerce_to(obj, String, :to_str)
  end
end