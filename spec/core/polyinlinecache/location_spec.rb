require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::PolyInlineCache#location" do
  before :each do
    PolyInlineCacheSpec::Bar.new.run
    PolyInlineCacheSpec::Qux.new.run
    @poly_inline_cache = PolyInlineCacheSpec::Foo::FooCallSite.call_sites[0]
  end

  it "has the correct location" do
    @poly_inline_cache.location.should =~ %r{spec/core/polyinlinecache/fixtures/classes\.rb:4$}
  end
end
