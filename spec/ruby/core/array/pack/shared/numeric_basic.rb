describe :array_pack_numeric_basic, :shared => true do
  it "returns an empty String if count is zero" do
    [1].pack(pack_format(0)).should == ""
  end

  it "raises ArgumentError when the format modifier is '_'" do
    lambda{ [1].pack(pack_format("_")) }.should raise_error(ArgumentError)
  end

  it "raises ArgumentError when the format modifier is '!'" do
    lambda{ [1].pack(pack_format("!")) }.should raise_error(ArgumentError)
  end
end
