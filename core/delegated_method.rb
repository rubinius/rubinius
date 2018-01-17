##
# DelegatedMethod is used to implement Module#define_method

module Rubinius
  class DelegatedMethod < Executable
    def initialize(name, method, receiver, pass_self=false)
      @name = name
      @method = method
      @receiver = receiver
      @pass_self = pass_self
    end

    private :initialize

    attr_reader :name
    attr_reader :receiver

    def call(called_object, called_method, *args, &block)
      args.unshift called_object if @pass_self
      @receiver.__send__(@method, *args, &block)
    end

    alias_method :activate, :call

    def arity
      @receiver.arity
    end

    def parameters
      @receiver.parameters
    end

    def source_location
      @receiver.source_location
    end

    def scope
      nil
    end
  end
end
