require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::CallSite#reset" do
  before :each do
    @klass = Class.new do
      def m(a)
        a.to_s
      end
    end

    @call_site = @klass.instance_method(:m).executable.call_sites.first
  end

  it "clears the caches" do
    obj = @klass.new

    obj.m 1
    obj.m :a

    @call_site.size.should == 1
    @call_site.reset

    @call_site.size.should == 0
  end

  it "sets the invokes to 0" do
    obj = @klass.new

    obj.m 1
    obj.m :a

    @call_site.invokes.should == 2
    @call_site.reset

    @call_site.invokes.should == 0
  end
end
