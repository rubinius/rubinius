require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::CallSite#invokes" do
  before :each do
    @executable = CallSiteSpecs::A.executable(:miss)
  end

  it "returns 0 before the call site has executed" do
    @executable.call_sites.first.invokes.should == 0
  end

  it "returns the number of invocations that do not hit a cache entry" do
    obj = CallSiteSpecs::A.new
    obj.miss(1).should == "1"
    obj.miss("2").should == "2"

    @executable.call_sites.first.invokes.should == 2
  end
end

