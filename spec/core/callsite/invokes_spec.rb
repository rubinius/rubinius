require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::CallSite#invokes" do
  it "returns 0 before the call site has executed" do
    executable = CallSiteSpecs::A.executable(:c)
    executable.call_sites.first.invokes.should == 0
  end

  it "returns the call site invocations that do not hit the cache" do
    obj = Class.new do
      def m(a)
        a.to_s
      end
    end.new

    obj.m 1
    obj.m :a
    obj.m :b
    obj.m "2"

    obj.method(:m).executable.call_sites.first.invokes.should == 3
  end
end
