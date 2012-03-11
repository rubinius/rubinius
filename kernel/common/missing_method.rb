# -*- encoding: us-ascii -*-

##
# MissingMethod is used to implement the Method returned by Kernel#method
# when #respond_to_missing? returns true.

module Rubinius
  class MissingMethod < Executable
    def call(called_object, called_method, *args, &block)
      called_object.__send__(called_method, *args, &block)
    end

    def arity
      -1
    end

    def parameters
      [[:rest]]
    end
  end
end
