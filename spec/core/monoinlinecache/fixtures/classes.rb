module MonoInlineCacheSpec
  module Foo
    FooCompiledCode = def foo
    end
  end

  class Bar
    include Foo
    CallSiteTest = def call_site_test
      foo
    end

    HitTest = def hits_test
      foo
    end
  end
end

