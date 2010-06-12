require File.expand_path('../../../spec_helper', __FILE__)

describe "ByteArray.new" do
  it "creates an instance with the correct number of fields" do
    ba = Rubinius::ByteArray.new(16)
    ba.should be_kind_of(Rubinius::ByteArray)
    ba.size.should >= 16
  end
end
