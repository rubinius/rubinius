class Regexp
  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, Regexp, :to_regexp
  end
end
