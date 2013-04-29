require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::PolyInlineCache#name" do
  before :each do
    PolyInlineCacheSpec::Bar.new.hit
    PolyInlineCacheSpec::Qux.new.hit
    @poly_inline_cache = PolyInlineCacheSpec::Foo::FooHitSite.call_sites[0]
  end

  it "returns the number of hits over all entries and has subtotals" do
    @poly_inline_cache.hits.should == 2
    entries = @poly_inline_cache.entries
    entries[0].hits.should == 1
    entries[1].hits.should == 1
    entries[2].should be_nil
  end

end

