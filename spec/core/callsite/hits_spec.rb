describe "Rubinius::CallSite#hits" do
  before :each do
    @klass = Class.new do
      def m(a)
        a.to_s
      end
    end
  end

  it "returns 0 before the call site has executed" do
    @klass.new.method(:m).executable.call_sites.first.hits.should == 0
  end

  it "returns the call site invocations that have hit a cache entry" do
    obj = @klass.new

    obj.m :a
    obj.m :b
    obj.m :c

    obj.method(:m).executable.call_sites.first.hits.should == 1
  end
end

