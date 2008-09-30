# depends on: class.rb

##
# DelegatedMethod is used to implement Module#define_method

class DelegatedMethod < Executable
  def initialize(method, receiver, pass_self=false)
    @method = method
    @receiver = receiver
    @pass_self = pass_self
  end

  def call(called_methed, called_object, *args, &block)
    args.unshift called_object if @pass_self
    @receiver.__send__(@method, *args, &block)
  end
end
