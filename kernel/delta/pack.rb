# -*- encoding: us-ascii -*-

module Rubinius
  def self.pack_to_int(obj)
    Rubinius::Type.coerce_to obj, Integer, :to_int
  end

  def self.pack_to_str(obj)
    Rubinius::Type.coerce_to obj, String, :to_str
  end

  def self.pack_to_str_or_nil(obj)
    return "" if obj.nil?
    Rubinius::Type.coerce_to obj, String, :to_str
  end

  def self.pack_to_s(obj)
    str = obj.to_s
    str = obj.inspect unless Type.object_kind_of? str, String
    str
  end
end
