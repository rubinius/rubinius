require File.expand_path('../../../spec_helper', __FILE__)

describe "CType#isspace" do
  it "returns true if self is ASCII whitespace" do
    a = []
    "\tt\fi a\r\nt\v".each_byte do |b|
      a << Rubinius::CType.isspace(b)
    end

    a.should == [true, false, true, false, true, false, true, true, false, true]
  end
end
