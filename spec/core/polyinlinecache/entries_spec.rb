require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::PolyInlineCache#entries" do
  before :each do
    CallSiteSpecs::A.new.p
    @cache = CallSiteSpecs::A.call_sites(:a).first
  end

  it "returns the entries for the cache" do
    entries = @cache.entries

    entries[0].receiver_class.should == CallSiteSpecs::B
    entries[0].stored_module.should == CallSiteSpecs::B
    entries[0].method.should == CallSiteSpecs::B.executable(:c)

    entries[1].receiver_class.should == CallSiteSpecs::BB
    entries[1].stored_module.should == CallSiteSpecs::BB
    entries[1].method.should == CallSiteSpecs::BB.executable(:c)

    entries[2].should be_nil
  end
end
