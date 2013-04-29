require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::PolyInlineCache#executable" do
  before :each do
    PolyInlineCacheSpec::Bar.new.run
    PolyInlineCacheSpec::Qux.new.run
    @poly_inline_cache = PolyInlineCacheSpec::Foo::FooCallSite.call_sites[0]
  end

  it "has the correct executable" do
    @poly_inline_cache.executable.should == PolyInlineCacheSpec::Foo::FooCallSite
  end
end
