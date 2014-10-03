module Rubinius
  module JIT
    class << self
      attr_accessor :available
      attr_accessor :enabled
      attr_accessor :properties

      alias_method :available?, :available
      alias_method :enabled?, :enabled
    end
  end
end
