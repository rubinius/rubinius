##
# MissingMethod is used to implement the Method returned by Kernel#method
# when #respond_to_missing? returns true.

module Rubinius
  class MissingMethod < Executable
    attr_reader :receiver
    attr_reader :method

    def initialize(receiver,  method)
      @receiver = receiver
      @method = method
    end

    def call(called_object, called_method, *args, &block)
      @receiver.__send__(@method, *args, &block)
    end

    def arity
      -1
    end

    def parameters
      [[:rest]]
    end

    def ==(other)
      other.kind_of?(MissingMethod) &&
      @receiver == other.receiver   &&
      @method   == other.method
    end

    alias_method :eql?, :==
  end
end
