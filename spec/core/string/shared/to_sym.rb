shared :string_to_sym do |cmd|
  describe "String##{cmd}" do
    it "returns the symbol corresponding to self" do
      "Koala".send(cmd).should == :Koala
      'cat'.send(cmd).should == :cat
      '@cat'.send(cmd).should == :@cat
      
      'cat and dog'.send(cmd).should == :"cat and dog"
    end
    
    it "raises an ArgumentError when self can't be converted to symbol" do
      should_raise(ArgumentError) { "".send(cmd) }
      should_raise(ArgumentError) { "foo\x00bar".send(cmd) }
    end
  end
end