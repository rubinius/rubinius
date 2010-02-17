module DefinedSpecs
  class ClassWithMethod
    def test
    end
  end

  class ClassUndefiningMethod < ClassWithMethod
    undef :test
  end

  class ClassWithoutMethod < ClassUndefiningMethod
    # If an undefined method overridden in descendants
    # define?(super) should return nil
    def test
      defined?(super)
    end
  end
end
