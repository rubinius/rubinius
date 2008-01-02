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
end