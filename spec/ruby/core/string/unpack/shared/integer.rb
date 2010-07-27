# -*- encoding: ascii-8bit -*-

describe :string_unpack_16bit_le, :shared => true do
  it "decodes one short for a single format character" do
    "ab".unpack(unpack_format).should == [25185]
  end

  it "decodes two shorts for two format characters" do
    "abcd".unpack(unpack_format(nil, 2)).should == [25185, 25699]
  end

  it "decodes the number or shorts requested by the count modifier" do
    "abcdef".unpack(unpack_format(3)).should == [25185, 25699, 26213]
  end

  it "decodes the remaining shorts when passed the '*' modifier" do
    "abcd".unpack(unpack_format('*')).should == [25185, 25699]
  end

  it "decodes the remaining shorts when passed the '*' modifier after another directive" do
    "abcd".unpack(unpack_format()+unpack_format('*')).should == [25185, 25699]
  end

  it "does not decode a short when fewer bytes than a short remain and the '*' modifier is passed" do
    "\xff".unpack(unpack_format('*')).should == []
  end

  it "adds nil for each element requested beyond the end of the String" do
    [ ["",     [nil, nil, nil]],
      ["abc",  [25185, nil, nil]],
      ["abcd", [25185, 25699, nil]]
    ].should be_computed_by(:unpack, unpack_format(3))
  end

  it "ignores NULL bytes between directives" do
    "abcd".unpack(unpack_format("\000", 2)).should == [25185, 25699]
  end

  it "ignores spaces between directives" do
    "abcd".unpack(unpack_format(' ', 2)).should == [25185, 25699]
  end
end

describe :string_unpack_16bit_le_signed, :shared => true do
  it "decodes a short with most significant bit set as a negative number" do
    "\x00\xff".unpack(unpack_format()).should == [-256]
  end
end

describe :string_unpack_16bit_le_unsigned, :shared => true do
  it "decodes a short with most significant bit set as a positive number" do
    "\x00\xff".unpack(unpack_format()).should == [65280]
  end
end

describe :string_unpack_16bit_be, :shared => true do
  it "decodes one short for a single format character" do
    "ba".unpack(unpack_format).should == [25185]
  end

  it "decodes two shorts for two format characters" do
    "badc".unpack(unpack_format(nil, 2)).should == [25185, 25699]
  end

  it "decodes the number or shorts requested by the count modifier" do
    "badcfe".unpack(unpack_format(3)).should == [25185, 25699, 26213]
  end

  it "decodes the remaining shorts when passed the '*' modifier" do
    "badc".unpack(unpack_format('*')).should == [25185, 25699]
  end

  it "decodes the remaining shorts when passed the '*' modifier after another directive" do
    "badc".unpack(unpack_format()+unpack_format('*')).should == [25185, 25699]
  end

  it "does not decode a short when fewer bytes than a short remain and the '*' modifier is passed" do
    "\xff".unpack(unpack_format('*')).should == []
  end

  it "adds nil for each element requested beyond the end of the String" do
    [ ["",     [nil, nil, nil]],
      ["bac",  [25185, nil, nil]],
      ["badc", [25185, 25699, nil]]
    ].should be_computed_by(:unpack, unpack_format(3))
  end

  it "ignores NULL bytes between directives" do
    "badc".unpack(unpack_format("\000", 2)).should == [25185, 25699]
  end

  it "ignores spaces between directives" do
    "badc".unpack(unpack_format(' ', 2)).should == [25185, 25699]
  end
end

describe :string_unpack_16bit_be_signed, :shared => true do
  it "decodes a short with most significant bit set as a negative number" do
    "\xff\x00".unpack(unpack_format()).should == [-256]
  end
end

describe :string_unpack_16bit_be_unsigned, :shared => true do
  it "decodes a short with most significant bit set as a positive number" do
    "\xff\x00".unpack(unpack_format()).should == [65280]
  end
end
