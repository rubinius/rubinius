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

  class PrivateMarshalMethods
    private

    def _dump
    end

    def marshal_dump
    end

    def marshal_load
    end
  end

  class PublicMarshalMethods
    def _dump
    end

    def marshal_dump
    end

    def marshal_load
    end
  end

  class MethodVisibility
    def public_method
    end

    private

    def private_method
    end
  end

  module RespondToMissing
    def respond_to_missing?(name, include_private)
      if :missing_public == name
        true
      elsif :missing_private == name && include_private
        true
      else
        false
      end
    end
    module_function :respond_to_missing?
  end
end
