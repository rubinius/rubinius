module NoMethodErrorSpecs
  class NoMethodErrorA; end

  class NoMethodErrorB; end

  class NoMethodErrorC;
    protected
    def a_protected_method;end
    private
    def a_private_method; end
  end

  class NoMethodErrorD; end
end
