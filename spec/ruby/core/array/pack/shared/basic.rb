describe :array_pack_basic, :shared => true do
  it "raises an ArgumentError if there are fewer elements than the format requires" do
    lambda { [].pack(pack_format(1)) }.should raise_error(ArgumentError)
  end
end
