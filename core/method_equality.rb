module Rubinius
  module MethodEquality
    def self.method_equal_to_delegated_method_receiver?(meth, other)
      self_receiver  = delegated_receiver_for(meth)  || meth.unbind
      other_receiver = delegated_receiver_for(other) || other.unbind
      either_method_is_delegated?(meth, other) && (self_receiver == other_receiver)
    end

    def self.either_method_is_delegated?(meth, other)
      (meth.executable.kind_of?(Rubinius::CompiledCode) && other.executable.kind_of?(Rubinius::DelegatedMethod)) ||
      (meth.executable.kind_of?(Rubinius::DelegatedMethod) && other.executable.kind_of?(Rubinius::CompiledCode))
    end

    def self.delegated_receiver_for(meth)
      meth.executable.receiver if meth.executable.kind_of?(Rubinius::DelegatedMethod)
    end
  end
end
