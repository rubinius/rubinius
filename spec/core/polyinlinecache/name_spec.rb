require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::PolyInlineCache#name" do
  before :each do
    PolyInlineCacheSpec::Bar.new.run
    PolyInlineCacheSpec::Qux.new.run
    @poly_inline_cache = PolyInlineCacheSpec::Foo::FooCallSite.call_sites[0]
  end

  it "has the correct name" do
    @poly_inline_cache.name.should == :sub
  end
end
