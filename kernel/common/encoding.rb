class EncodingError < StandardError
end

class Encoding
  class UndefinedConversionError < EncodingError
  end

  class Converter
    def initialize(from, to, options={})
    end

    def convert(str)
    end
  end

  def self.aliases
    aliases = {}
    Rubinius::Encoding::SymbolMap.each do |n, e|
      aliases[n] = e.name unless n.to_s == e.name
    end

    aliases
  end

  def self.compatible?(str1, str2)
    false
  end

  def self.default_external
  end

  def self.default_external=(enc)
  end

  def self.default_internal
  end

  def self.default_internal=(enc)
  end

  def self.find(name)
    key = StringValue(name).upcase


    Rubinius::Encoding::SymbolMap.each do |n, e|
      return e if n.to_s.upcase == key
    end

    raise ArgumentError, "unknown encoding name - #{name}"
  end

  def self.list
    list = []
    Rubinius::Encoding::SymbolMap.each do |n, e|
      list << e if n.to_s == e.name
    end

    list
  end

  def self.locale_charmap
  end

  def self.name_list
    Rubinius::Encoding::SymbolMap.keys.map { |name| name.to_s }
  end

  def inspect
    "#<Encoding:#{name}>"
  end

  def names
  end
end

# TODO: This psuedo variable should represent a scripts encoding.
Object.send :define_method, :__ENCODING__ do
  Encoding.new
end
