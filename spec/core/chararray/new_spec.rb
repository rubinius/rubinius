require File.expand_path('../../../spec_helper', __FILE__)

describe "CharArray.new" do
  it "creates an instance with the correct number of fields" do
    ba = Rubinius::CharArray.new(16)
    ba.should be_kind_of(Rubinius::CharArray)
    ba.size.should >= 16
  end
end
