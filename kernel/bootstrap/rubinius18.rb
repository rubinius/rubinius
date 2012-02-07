# -*- encoding: us-ascii -*-

module Rubinius
  # Ruby 1.8 returns strings for method and constant names
  def self.convert_to_names(list)
    list.map { |x| x.to_s }
  end

  def self.convert_to_name(sym)
    sym.to_s
  end

  def self.binary_string(string)
    string
  end
end
