require File.dirname(__FILE__) + '/../../../spec_helper'

extension :rubinius do
  describe "ByteArray.new" do
    it "creates an instance with the correct number of fields" do
      ByteArray.new(16).fields.should == 4
    end
  end
end
