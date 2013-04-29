require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::PolyInlineCache#entries" do
  before :each do
    PolyInlineCacheSpec::Bar.new.run
    PolyInlineCacheSpec::Qux.new.run
    @poly_inline_cache = PolyInlineCacheSpec::Foo::FooCallSite.call_sites[0]
  end

  it "has the correct entries" do
    entries = @poly_inline_cache.entries
    entries[0].receiver_class.should == PolyInlineCacheSpec::Bar
    entries[0].stored_module.should == PolyInlineCacheSpec::Foo
    entries[0].method.should == PolyInlineCacheSpec::Foo::FooCompiledCode

    entries[1].receiver_class.should == PolyInlineCacheSpec::Qux
    entries[1].stored_module.should == PolyInlineCacheSpec::Foo
    entries[0].method.should == PolyInlineCacheSpec::Foo::FooCompiledCode

    entries[2].should be_nil
  end
end
