module PolyInlineCacheSpec
  module Foo
    FooCallSite = def foo
      sub
    end

    FooHitSite = def foo_hit
      sub
    end

    FooCompiledCode = def sub
    end
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

