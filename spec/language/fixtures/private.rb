module Private
  class A
    def foo
      "foo"
    end

    private
    def bar
      "bar"
    end
  end

  class B
    def foo
      "foo"
    end

    private
    class C
      def baz
        "baz"
      end
    end

    def bar
      "bar"
    end
  end
  
  module D
    private
    def foo
      "foo"
    end
  end
   
   class E
     include D
   end
end
