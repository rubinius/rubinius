describe "Rubinius::CallSite#misses" do
  before :each do
    @klass = Class.new do
      def m(a)
        a.to_s
      end
    end
  end

  it "returns 0 before the call site has executed" do
    @klass.new.method(:m).executable.call_sites.first.misses.should == 0
  end

  it "returns the call site invocations that have missed the cache entries" do
    obj = @klass.new

    obj.m 1
    obj.m :a
    obj.m "a"
    obj.m Object.new
    obj.m []

    obj.method(:m).executable.call_sites.first.misses.should == 3
  end
end

