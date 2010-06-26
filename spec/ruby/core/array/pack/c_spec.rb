# -*- coding: ISO-8859-1 -*-

require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)
require File.expand_path('../shared/basic', __FILE__)
require File.expand_path('../shared/numeric_basic', __FILE__)

describe :array_pack_8bit, :shared => true do
  it "encodes the least significant eight bits of a positive number" do
    [49].pack(pack_format).should == encode('1', "binary")
  end

  it "encodes the least significant eight bits of a negative number" do
    [ [[-1],          encode("\xFF", "binary")],
      [[-0b10000000], encode("\x80", "binary")]
    ].should be_computed_by(:pack, pack_format)
  end

  it "reduces value to fit in byte" do
    [ [[2**8-1],  encode("\xFF", "binary")],
      [[2**8  ],  encode("\x00", "binary")],
      [[2**8+1],  encode("\x01", "binary")],
      [[-2**8+1], encode("\x01", "binary")],
      [[-2**8  ], encode("\x00", "binary")],
      [[-2**8-1], encode("\xFF", "binary")]
    ].should be_computed_by(:pack, pack_format)
  end

  it "encodes a Float truncated as an Integer" do
    [ [[5.2], encode("\x05", "binary")],
      [[5.8], encode("\x05", "binary")]
    ].should be_computed_by(:pack, pack_format)
  end

  it "calls #to_int to convert the pack argument to an Integer" do
    obj = mock('to_int')
    obj.should_receive(:to_int).and_return(5)
    [obj].pack(pack_format).should == encode("\x05", "binary")
  end

  not_compliant_on :rubinius do
    ruby_version_is '' ... '1.9' do
      it "accepts a Symbol as a pack argument because it responds to #to_int" do
        [:hello].pack(pack_format).should == [:hello.to_i].pack('C')
      end
    end
  end

  it "raises a TypeError when passed a String" do
    lambda { ["5"].pack(pack_format) }.should raise_error(TypeError)
  end

  it "raises a TypeError when the object does not respond to #to_int" do
    obj = mock('not an integer')
    lambda { [obj].pack(pack_format) }.should raise_error(TypeError)
  end

  it "processes count number of array elements if count given" do
    [ [[1, 2, 3], pack_format(3), encode("\x01\x02\x03", "binary")],
      [[1, 2, 3], pack_format(2) + pack_format(1), encode("\x01\x02\x03", "binary")]
    ].should be_computed_by(:pack)
  end

  it "with star parameter processes all remaining array items" do
    [1, 2, 3, 4, 5].pack(pack_format('*')).should == encode("\x01\x02\x03\x04\x05", "binary")
  end

  ruby_version_is '1.9' do
    it "returns an ASCII-8BIT string" do
      [0x41].pack(pack_format).encoding.should == Encoding::ASCII_8BIT
      [0xFF].pack(pack_format).encoding.should == Encoding::ASCII_8BIT
      [0xE3, 0x81, 0x82].pack(pack_format(3)).encoding.should == Encoding::ASCII_8BIT
    end
  end
end

describe "Array#pack with format 'C'" do
  it_behaves_like :array_pack_8bit, 'C'
  it_behaves_like :array_pack_basic, 'C'
  it_behaves_like :array_pack_numeric_basic, 'C'
end

describe "Array#pack with format 'c'" do
  it_behaves_like :array_pack_8bit, 'c'
  it_behaves_like :array_pack_basic, 'c'
  it_behaves_like :array_pack_numeric_basic, 'c'
end
