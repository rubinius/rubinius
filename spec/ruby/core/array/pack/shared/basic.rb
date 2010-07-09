describe :array_pack_basic, :shared => true do
  it "raises an ArgumentError if there are fewer elements than the format requires" do
    lambda { [].pack(pack_format(1)) }.should raise_error(ArgumentError)
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
