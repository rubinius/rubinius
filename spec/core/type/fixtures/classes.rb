module TypeSpecs
  module M; end

  class A; end

  class B < A; end

  class C
    include M
  end

  class D < C; end

  class E
    def kind_of?(mod)
      true
    end

    alias_method :is_a?, :kind_of?
    alias_method :instance_of?, :kind_of?
  end
end
