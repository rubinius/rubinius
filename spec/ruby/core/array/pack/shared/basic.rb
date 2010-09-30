describe :array_pack_basic, :shared => true do
  it "raises an ArgumentError if there are fewer elements than the format requires" do
    lambda { [].pack(pack_format(1)) }.should raise_error(ArgumentError)
  end
end

describe :array_pack_safe, :shared => true do
  it "taints the output string if the format string is tainted" do
    obj = mock("string float int")
    obj.stub!(:to_int).and_return(1)
    obj.stub!(:to_str).and_return("1")
    obj.stub!(:to_f).and_return(1.0)
    [obj, obj].pack("x"+pack_format.taint).tainted?.should be_true
  end
end

describe :array_pack_no_platform, :shared => true do
  it "raises ArgumentError when the format modifier is '_'" do
    lambda{ [1].pack(pack_format("_")) }.should raise_error(ArgumentError)
  end

  it "raises ArgumentError when the format modifier is '!'" do
    lambda{ [1].pack(pack_format("!")) }.should raise_error(ArgumentError)
  end
end
