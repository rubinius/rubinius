require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

describe "ByteArray.new" do
  it "creates an instance with the correct number of fields" do
    ba = Rubinius::ByteArray.new(16)
    ba.should be_kind_of(Rubinius::ByteArray)
    ba.size.should >= 16
  end

  it "raises ArgumentError for negative size" do
    lambda { Rubinius::ByteArray.new(-20) }.should raise_error(ArgumentError)
  end
end
