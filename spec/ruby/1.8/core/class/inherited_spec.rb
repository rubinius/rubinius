require File.dirname(__FILE__) + '/../../spec_helper'

describe "Class.inherited" do
  it "is invoked with the child Class when self is subclassed" do
    begin
      top = Class.new do
        def self.inherited(cls)
          $child_class = cls
        end
      end
      
      child = Class.new(top)
      $child_class.should == child

      other_child = Class.new(top)
      $child_class.should == other_child
    ensure
      $child_class = nil
    end
  end

  it "will be invoked by child class regardless of visibility" do
    top = Class.new do
      class << self
        def inherited(cls); end
      end
    end

    class << top; private :inherited; end
    lambda { Class.new(top) }.should_not raise_error

    class << top; protected :inherited; end
    lambda { Class.new(top) }.should_not raise_error
  end
end
