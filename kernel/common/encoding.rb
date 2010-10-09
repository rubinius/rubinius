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
    {}
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
  end

  def self.list
    []
  end

  def self.locale_charmap
  end

  def self.name_list
    []
  end

  def dummy?
  end

  def name
  end

  def names
  end
end
