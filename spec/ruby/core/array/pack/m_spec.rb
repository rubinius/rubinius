# -*- encoding: ascii-8bit -*-
require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)
require File.expand_path('../shared/basic', __FILE__)

describe "Array#pack with format 'M'" do
  it_behaves_like :array_pack_basic, 'M'

  it "encodes an empty string as an empty string" do
    [""].pack("M").should == ""
  end

  it "appends a soft line break at the end of an encoded string" do
    ["a"].pack("M").should == "a=\n"
  end

  it "does not append a soft break if the string ends with a newline" do
    ["a\n"].pack("M").should == "a\n"
  end

  it "encodes one element for each directive" do
    ["a", "b", "c"].pack("MM").should == "a=\nb=\n"
  end

  it "encodes byte values 33..60 directly" do
    [ [["!\"\#$%&'()*+,-./"], "!\"\#$%&'()*+,-./=\n"],
      [["0123456789"],        "0123456789=\n"],
      [[":;<"],               ":;<=\n"]
    ].should be_computed_by(:pack, "M")
  end

  it "encodes byte values 62..126 directly" do
    [ [[">?@"],                         ">?@=\n"],
      [["ABCDEFGHIJKLMNOPQRSTUVWXYZ"],  "ABCDEFGHIJKLMNOPQRSTUVWXYZ=\n"],
      [["[\\]^_`"],                     "[\\]^_`=\n"],
      [["abcdefghijklmnopqrstuvwxyz"],  "abcdefghijklmnopqrstuvwxyz=\n"],
      [["{|}~"],                        "{|}~=\n"]
    ].should be_computed_by(:pack, "M")
  end

  it "encodes an '=' character in hex format" do
    ["="].pack("M").should == "=3D=\n"
  end

  it "encodes an embedded space directly" do
    ["a b"].pack("M").should == "a b=\n"
  end

  it "encodes a space at the end of the string directly" do
    ["a "].pack("M").should == "a =\n"
  end

  it "encodes an embedded tab directly" do
    ["a\tb"].pack("M").should == "a\tb=\n"
  end

  it "encodes a tab at the end of the string directly" do
    ["a\t"].pack("M").should == "a\t=\n"
  end

  it "encodes an embedded newline directly" do
    ["a\nb"].pack("M").should == "a\nb=\n"
  end

  it "encodes 0..31 except tab and newline in hex format" do
    [ [["\x00\x01\x02\x03\x04\x05\x06"],  "=00=01=02=03=04=05=06=\n"],
      [["\a\b\v\f\r"],                    "=07=08=0B=0C=0D=\n"],
      [["\x0e\x0f\x10\x11\x12\x13\x14"],  "=0E=0F=10=11=12=13=14=\n"],
      [["\x15\x16\x17\x18\x19\x1a"],      "=15=16=17=18=19=1A=\n"],
      [["\e"],                            "=1B=\n"],
      [["\x1c\x1d\x1e\x1f"],              "=1C=1D=1E=1F=\n"]
    ].should be_computed_by(:pack, "M")
  end

  it "encodes a tab followed by a newline with an encoded newline" do
    ["\t\n"].pack("M").should == "\t=\n\n"
  end

  it "encodes 127..255 in hex format" do
    [ [["\x7f\x80\x81\x82\x83\x84\x85\x86"], "=7F=80=81=82=83=84=85=86=\n"],
      [["\x87\x88\x89\x8a\x8b\x8c\x8d\x8e"], "=87=88=89=8A=8B=8C=8D=8E=\n"],
      [["\x8f\x90\x91\x92\x93\x94\x95\x96"], "=8F=90=91=92=93=94=95=96=\n"],
      [["\x97\x98\x99\x9a\x9b\x9c\x9d\x9e"], "=97=98=99=9A=9B=9C=9D=9E=\n"],
      [["\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6"], "=9F=A0=A1=A2=A3=A4=A5=A6=\n"],
      [["\xa7\xa8\xa9\xaa\xab\xac\xad\xae"], "=A7=A8=A9=AA=AB=AC=AD=AE=\n"],
      [["\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6"], "=AF=B0=B1=B2=B3=B4=B5=B6=\n"],
      [["\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe"], "=B7=B8=B9=BA=BB=BC=BD=BE=\n"],
      [["\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6"], "=BF=C0=C1=C2=C3=C4=C5=C6=\n"],
      [["\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce"], "=C7=C8=C9=CA=CB=CC=CD=CE=\n"],
      [["\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6"], "=CF=D0=D1=D2=D3=D4=D5=D6=\n"],
      [["\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde"], "=D7=D8=D9=DA=DB=DC=DD=DE=\n"],
      [["\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6"], "=DF=E0=E1=E2=E3=E4=E5=E6=\n"],
      [["\xe7\xe8\xe9\xea\xeb\xec\xed\xee"], "=E7=E8=E9=EA=EB=EC=ED=EE=\n"],
      [["\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6"], "=EF=F0=F1=F2=F3=F4=F5=F6=\n"],
      [["\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe"], "=F7=F8=F9=FA=FB=FC=FD=FE=\n"],
      [["\xff"], "=FF=\n"]
    ].should be_computed_by(:pack, "M")
  end

  it "emits a soft line break when the output exceeds 72 characters when passed '*', 0, 1, or no count modifier" do
    s1 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    r1 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa=\na=\n"
    s2 = "\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19\x19"
    r2 = "=19=19=19=19=19=19=19=19=19=19=19=19=19=19=19=19=19=19=19=19=19=19=19=19=19=\n=19=\n"
    s3 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\x15a"
    r3 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa=15=\na=\n"
    s4 = "\x15aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\x15a"
    r4 = "=15aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa=\na=15a=\n"

    [ [[s1], "M",  r1],
      [[s1], "M0", r1],
      [[s1], "M1", r1],
      [[s2], "M",  r2],
      [[s2], "M0", r2],
      [[s2], "M1", r2],
      [[s3], "M",  r3],
      [[s3], "M0", r3],
      [[s3], "M1", r3],
      [[s4], "M",  r4],
      [[s4], "M0", r4],
      [[s4], "M1", r4]
    ].should be_computed_by(:pack)
  end

  it "emits a soft line break when the output exceeds count characters" do
    [ [["abcdefghi"],        "M2", "abc=\ndef=\nghi=\n"],
      [["abcdefghi"],        "M3", "abcd=\nefgh=\ni=\n"],
      [["abcdefghi"],        "M4", "abcde=\nfghi=\n"],
      [["abcdefghi"],        "M5", "abcdef=\nghi=\n"],
      [["abcdefghi"],        "M6", "abcdefg=\nhi=\n"],
      [["\x19\x19\x19\x19"], "M2", "=19=\n=19=\n=19=\n=19=\n"],
      [["\x19\x19\x19\x19"], "M3", "=19=19=\n=19=19=\n"],
      [["\x19\x19\x19\x19"], "M4", "=19=19=\n=19=19=\n"],
      [["\x19\x19\x19\x19"], "M5", "=19=19=\n=19=19=\n"],
      [["\x19\x19\x19\x19"], "M6", "=19=19=19=\n=19=\n"],
      [["\x19\x19\x19\x19"], "M7", "=19=19=19=\n=19=\n"]
    ].should be_computed_by(:pack)
  end

  it "encodes a recursive array" do
    empty = ArraySpecs.empty_recursive_array
    empty.pack('M').should be_kind_of(String)

    array = ArraySpecs.recursive_array
    array.pack('M').should == "1=\n"
  end

  it "calls #to_s to convert an object to a String" do
    obj = mock("pack M string")
    obj.should_receive(:to_s).and_return("packing")

    [obj].pack("M").should == "packing=\n"
  end

  it "converts the object to a String representation if #to_s does not return a String" do
    obj = mock("pack M non-string")
    obj.should_receive(:to_s).and_return(2)

    [obj].pack("M").should be_an_instance_of(String)
  end

  it "encodes a Symbol as a String" do
    [:symbol].pack("M").should == "symbol=\n"
  end

  it "encodes an Integer as a String" do
    [ [[1],             "1=\n"],
      [[bignum_value],  "#{bignum_value}=\n"]
    ].should be_computed_by(:pack, "M")
  end

  it "encodes a Float as a String" do
    [1.0].pack("M").should == "1.0=\n"
  end

  ruby_bug "#3273", "1.8.7" do
    it "converts Floats to the minimum unique representation" do
      [1.0 / 3.0].pack("M").should == "0.3333333333333333=\n"
    end
  end

  ruby_version_is "".."1.9" do
    describe "with a multibyte $KCODE" do
      before :each do
        @kcode = $KCODE
      end

      after :each do
        $KCODE = @kcode
      end

      it "encodes multibyte characters" do
        $KCODE = "UTF8"
        ["あ"].pack("M").should == "=E3=81=82=\n"
      end
    end
  end

  ruby_version_is "1.9" do
    it "returns an US-ASCII string" do
      ["abcd"].pack("M").encoding.should == Encoding::US_ASCII
    end
  end
end

describe "Array#pack with format 'm'" do
  it_behaves_like :array_pack_basic, 'm'
end
