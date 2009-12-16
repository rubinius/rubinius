require File.dirname(__FILE__) + '/../../spec_helper'

describe "Regexp#inspect" do
  it "returns a formatted string that would eval to the same regexp" do
    /ab+c/ix.inspect.should == "/ab+c/ix"
    /a(.)+s/n.inspect.should =~ %r|/a(.)+s/n?|  # Default 'n' may not appear
    # 1.9 doesn't round-trip the encoding flags, such as 'u'. This is
    # seemingly by design.  
    /a(.)+s/m.inspect.should == "/a(.)+s/m"     # But a specified one does
  end
  
  it "correctly escapes forward slashes /" do
    Regexp.new("/foo/bar").inspect.should == "/\\/foo\\/bar/"
    Regexp.new("/foo/bar[/]").inspect.should == "/\\/foo\\/bar[\\/]/"
  end  
end
