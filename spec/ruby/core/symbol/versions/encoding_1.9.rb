# encoding: utf-8

describe "Symbol#encoding for UTF-8 symbols" do
  it "should be UTF-8" do
    :åäö.encoding.name.should == "UTF-8"
  end

  it "should be UTF-8 after converting to string" do
    :åäö.to_s.encoding.name.should == "UTF-8"
  end
end
