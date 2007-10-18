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
  end
end
