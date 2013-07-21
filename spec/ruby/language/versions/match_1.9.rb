require File.expand_path('../../../spec_helper', __FILE__)

describe "The =~ operator with named captures" do
  before(:each) do
    @regexp = /(?<matched>foo)(?<unmatched>bar)?/
    @string = "foofoo"
  end

  describe "on syntax of /regexp/ =~ string_variable" do
    it "sets local variables by the captured pairs" do
      /(?<matched>foo)(?<unmatched>bar)?/ =~ @string
      local_variables.should == [:matched, :unmatched]
      matched.should == "foo"
      unmatched.should == nil
    end
  end

  describe "on syntax of string_variable =~ /regexp/" do
    it "does not set local variables" do
      @string =~ /(?<matched>foo)(?<unmatched>bar)?/
      local_variables.should == []
    end
  end

  describe "on syntax of regexp_variable =~ string_variable" do
    it "does not set local variables" do
      @regexp =~ @string
      local_variables.should == []
    end
  end

  describe "on the method calling" do
    it "does not set local variables" do
      @regexp.=~(@string)
      local_variables.should == []

      @regexp.send :=~, @string
      local_variables.should == []
    end
  end
end
