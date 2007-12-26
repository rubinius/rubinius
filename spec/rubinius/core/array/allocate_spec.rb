require File.dirname(__FILE__) + '/../../../spec_helper'

extension :rubinius do
  describe "Array.allocate" do
    it "creates an instance with the correct number of fields" do
      Array.allocate.fields.should == Array.instance_fields
    end
  end
end
