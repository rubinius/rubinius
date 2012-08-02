# -*- encoding: us-ascii -*-

module Rubinius
  class Thunk
    def self.new(value)
      Rubinius.primitive :thunk_create
      raise PrimitiveFailure, "Thunk.new primitive failed"
    end
  end

  class CallUnit
    def self.constant_value(obj)
      Rubinius.primitive :callunit_constant_value
      raise PrimitiveFailure, "CallUnit.constant_value primitive failed"
    end

    def self.for_method_parts(mod, exec, name)
      Rubinius.primitive :callunit_for_method_parts
      raise PrimitiveFailure, "CallUnit.for_method_parts primitive failed"
    end

    def self.for_method(mo)
      for_method_parts mo.defined_in, mo.executable, mo.name.to_sym
    end

    def self.test(cond, c_then, c_else)
      Rubinius.primitive :callunit_test
      raise PrimitiveFailure, "CallUnit.test primitive failed"
    end

    def self.test_kind_of(mod, which)
      Rubinius.primitive :callunit_kind_of
      raise PrimitiveFailure, "CallUnit.test_kind_of primitive failed"
    end
  end

  class CallUnitAdapter
    def self.new(unit)
      Rubinius.primitive :callunitadapter_create
      raise PrimitiveFailure, "CallUnitAdapter.new primitive failed"
    end
  end

end
