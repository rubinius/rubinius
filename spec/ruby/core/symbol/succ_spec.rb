require File.expand_path('../../../spec_helper', __FILE__)

describe :symbol_succ, :shared => true do
  it "returns symbol with last character incremented" do
    :a.send(@method).should == :b
    :A.send(@method).should == :B
    :abc.send(@method).should == :abd
    :a1.send(@method).should == :a2
  end

  it "wraps 'z' and 'Z' to 'aa' and 'AA' respectively" do
    :z.send(@method).should == :aa
    :Z.send(@method).should == :AA
    :AZ.send(@method).should == :BA
  end

  it "works with symbols with non-alphanumeric characters" do
    :"[".send(@method).should == :"\\"
    :"a[".send(@method).should == :"b["
  end
end

ruby_version_is "1.9" do
  describe "Symbol#succ" do
    it_behaves_like :symbol_succ, :succ
  end

  describe "Symbol#next" do
    it_behaves_like :symbol_succ, :next
  end
end
