# -*- encoding: us-ascii -*-

module Rubinius
  def self.convert_to_names(list)
    list
  end

  def self.convert_to_name(sym)
    sym
  end

  def self.binary_string(string)
    string.force_encoding(Encoding::BINARY)
  end
end
