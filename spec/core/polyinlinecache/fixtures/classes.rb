module PolyInlineCacheSpec
  module Foo
    FooCallSite = instance_method(def foo
      sub
    end).executable

    FooHitSite = instance_method(def foo_hit
      sub
    end).executable

    FooCompiledCode = instance_method(def sub
    end).executable
  end

  class Bar
    include Foo
    def run
      foo
    end

    def hit
      foo_hit
    end
  end

  class Qux
    include Foo
    def run
      foo
    end

    def hit
      foo_hit
    end
  end
end

