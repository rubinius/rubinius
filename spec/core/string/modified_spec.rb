require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#modified?" do
  it "does not raise an exception if the string's ByteArray id and length have not changed" do
    str = "abcde"
    id = str.data.object_id
    size = str.size
    str.modified?(id, size).should == nil
  end

  it "raises a RuntimeError if the string's ByteArray id has changed" do
    str = "abcde"
    id = str.data.object_id
    size = str.size
    str.num_bytes -= 1
    lambda { str.modified?(id, size) }.should raise_error(RuntimeError)
  end

  it "raises a RuntimeError if the string's size has changed" do
    str = "abcde"
    id = str.data.object_id
    size = str.size
    str.data = str.data.dup
    lambda { str.modified?(id, size) }.should raise_error(RuntimeError)
  end
end
