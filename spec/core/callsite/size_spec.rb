require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::CallSite#size" do
  before :each do
    @executable = CallSiteSpecs::A.executable(:size)
  end

  it "returns 0 before the call site has executed" do
    @executable.call_sites.first.size.should == 0
  end

  it "returns 0 after the first invocation" do
    CallSiteSpecs::A.new.size(:x)
    @executable.call_sites.first.size.should == 0
  end

  it "returns 2 for a bi-morphic call site" do
    executable = CallSiteSpecs::N.instance_method(:size).executable

    a = CallSiteSpecs::MIA.new
    a.size 1
    a.size 2

    executable.call_sites.first.size.should == 1

    b = CallSiteSpecs::MIB.new
    b.size :y
    b.size 42

    b.size(42).should == "42"
    executable.call_sites.first.size.should == 2
  end
end

