require 'singleton'

module SingletonSpecs
  class MyClass
    attr_accessor :data
    include Singleton
  end
end
