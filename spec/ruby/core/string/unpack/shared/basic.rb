describe :string_unpack_no_platform, :shared => true do
  it "raises an ArgumentError when the format modifier is '_'" do
    lambda { "abcdefgh".unpack(unpack_format("_")) }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError when the format modifier is '!'" do
    lambda { "abcdefgh".unpack(unpack_format("!")) }.should raise_error(ArgumentError)
  end
end
