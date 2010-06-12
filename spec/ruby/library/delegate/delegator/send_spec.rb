require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)

describe "SimpleDelegator.new" do
  before :each do
    @simple = DelegateSpecs::Simple.new
    @delegate = SimpleDelegator.new(@simple)
  end

  it "forwards public method calls" do
    @delegate.pub.should == :foo
  end

  it "forwards protected method calls" do
    @delegate.prot.should == :protected
  end

  it "doesn't forward private method calls" do
    lambda{ @delegate.priv }.should raise_error( NoMethodError )
  end

  ruby_version_is "" ... "1.9" do
    ruby_bug "redmine:2206", "1.8.7" do
      it "forwards private method calls made via send or __send__" do
        @delegate.send(:priv, 42).should == [:priv, 42]
        @delegate.__send__(:priv, 42).should == [:priv, 42]
        lambda{ @delegate.__send__(:priv, 42){@delegate.priv} }.should raise_error( NoMethodError )
      end
    end
  end

  ruby_version_is "1.9" do
    it "doesn't forward private method calls even via send or __send__" do
      lambda{ @delegate.send(:priv, 42)     }.should raise_error( NoMethodError )
      lambda{ @delegate.__send__(:priv, 42) }.should raise_error( NoMethodError )
    end
  end
end
