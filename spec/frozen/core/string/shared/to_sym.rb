describe :string_to_sym, :shared => true do
  it "returns the symbol corresponding to self" do
    "Koala".send(@method).should == :Koala
    'cat'.send(@method).should == :cat
    '@cat'.send(@method).should == :@cat
    'cat and dog'.send(@method).should == :"cat and dog"
    "abc=".send(@method).should == :abc=
  end

  ruby_version_is ""..."1.9" do
    it "raises an ArgumentError when self can't be converted to symbol" do
      lambda { "".send(@method)           }.should raise_error(ArgumentError)
      lambda { "foo\x00bar".send(@method) }.should raise_error(ArgumentError)
    end
  end
end
