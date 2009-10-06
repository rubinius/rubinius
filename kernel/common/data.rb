module Rubinius
  def self.set_data(str)
    require 'stringio'
    Object.const_set :DATA, StringIO.new(str)
  end
end
