require File.dirname(__FILE__) + '/../../spec_helper'

describe "ByteArray.new" do
  it "creates an instance with the correct number of fields" do
    ba = ByteArray.new(16)
    ba.should be_kind_of(ByteArray)
    ba.size.should >= 16
  end
end
