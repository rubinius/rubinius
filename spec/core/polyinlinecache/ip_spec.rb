require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::PolyInlineCache#ip" do
  before :each do
    PolyInlineCacheSpec::Bar.new.run
    PolyInlineCacheSpec::Qux.new.run
    @poly_inline_cache = PolyInlineCacheSpec::Foo::FooCallSite.call_sites[0]
  end

  it "has the correct ip" do
    @poly_inline_cache.ip.should == 1
  end
end

