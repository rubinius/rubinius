module Rubinius
  class Thunk
    def self.new(value)
      Ruby.primitive :thunk_create
      raise PrimitiveFailure, "Thunk.new failed"
    end
  end

  class CallUnit
    def self.constant_value(obj)
      Ruby.primitive :callunit_constant_value
      raise PrimitiveFailure, "CallUnit.constant_value failed"
    end

    def self.for_method_parts(mod, exec, name)
      Ruby.primitive :callunit_for_method_parts
      raise PrimitiveFailure, "CallUnit.for_method_parts failed"
    end

    def self.for_method(mo)
      for_method_parts mo.defined_in, mo.executable, mo.name.to_sym
    end

    def self.test(cond, c_then, c_else)
      Ruby.primitive :callunit_test
      raise PrimitiveFailure, "CallUnit.test failed"
    end

    def self.test_kind_of(mod, which)
      Ruby.primitive :callunit_kind_of
      raise PrimitiveFailure, "CallUnit.test_kind_of failed"
    end
  end

  class CallUnitAdapter
    def self.new(unit)
      Ruby.primitive :callunitadapter_create
      raise PrimitiveFailure, "CallUnitAdapter.new failed"
    end
  end

end
