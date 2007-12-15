require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../runner/shared'

class Object
  def spec_runner
    @runner ||= Object.new
  end
end

describe Object, "#shared" do
  it "sets an instance variable on Object for the passed block" do
    proc = lambda { :shared }
    shared :shared, &proc
    spec_runner.instance_variable_get(:@shared).should == proc
  end
end

describe Object, "#it_behaves_like" do
  it "retrieves the instance variable set on Object and calls the proc" do
    proc = lambda { raise Exception, "visited" }
    shared :shared, &proc
    lambda {
      it_behaves_like(:shared, nil)
    }.should raise_error(Exception, "visited")
  end
end
