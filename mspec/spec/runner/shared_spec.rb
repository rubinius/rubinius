require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../runner/shared'

describe Object, "#shared" do
  it "sets an instance variable on Object for the passed block" do
    proc = lambda { :shared }
    shared :shared, &proc
    Object.instance_variable_get(:@shared).should == proc
  end
end

describe Object, "#it_behaves_like" do
  before :each do
  end
  
  it "retrieves the instance variable set on Object and calls the proc" do
    proc = lambda { |a| raise Exception, "visited with #{a.inspect}" }
    shared :shared, &proc
    lambda {
      it_behaves_like(:shared, nil)
    }.should raise_error(Exception, "visited with nil")
  end
  
  it "accepts an optional argument to specify the class" do
    proc = lambda { |a, b| raise Exception, "visited with #{a.inspect}, #{b.inspect}" }
    shared :shared, &proc
    lambda {
      it_behaves_like(:shared, :method, :klass)
    }.should raise_error(Exception, "visited with :method, :klass")
  end
end
