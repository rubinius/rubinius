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

  class PrivateToAry
    private

    def to_ary
      [1, 2, 3]
    end
  end

  class PublicToAry
    def to_ary
      [1, 2, 3]
    end
  end

  class MethodVisibility
    def public_method
    end

    private

    def private_method
    end
  end
end
