module MonoInlineCacheSpec
  module Foo
    FooCompiledCode = instance_method(def foo
    end).executable
  end

  class Bar
    include Foo
    CallSiteTest = instance_method(def call_site_test
      foo
    end).executable

    HitTest = instance_method(def hits_test
      foo
    end).executable
  end
end

