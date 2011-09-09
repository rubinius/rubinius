require File.expand_path('../../../../spec_helper', __FILE__)

describe "String#unpack with format 'P'" do
  it "returns a random object after consuming a size-of a machine word bytes" do
    str = "\0" * 1.size
    str.unpack("P").should be_kind_of(Object)
  end
end
