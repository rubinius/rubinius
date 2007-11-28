require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../expectations'
require File.dirname(__FILE__) + '/../../matchers/raise_error'

class ExpectedException < Exception; end
class UnexpectedException < Exception; end

describe RaiseErrorMatcher do
  it "matches when the proc raises the expected exception" do
    proc = Proc.new { raise ExpectedException }
    RaiseErrorMatcher.new(ExpectedException, nil).matches?(proc).should == true
  end
  
  it "matches when the proc raises the expected exception with the expected message" do
    proc = Proc.new { raise ExpectedException, "message" }
    RaiseErrorMatcher.new(ExpectedException, "message").matches?(proc).should == true
  end
  
  it "does not match when the proc does not raise the expected exception" do
    proc = Proc.new { raise UnexpectedException }
    RaiseErrorMatcher.new(ExpectedException, nil).matches?(proc).should == false
  end
  
  it "does not match when the proc raises the expected exception with an unexpected message" do
    proc = Proc.new { raise UnexpectedException, "unexpected" }
    RaiseErrorMatcher.new(ExpectedException, "expected").matches?(proc).should == false
  end
  
  it "provides a useful failure message" do
    proc = Proc.new { raise UnexpectedException, "unexpected" }
    matcher = RaiseErrorMatcher.new(ExpectedException, "expected")
    matcher.matches?(proc)
    matcher.failure_message.should == 
      ["Expected ExpectedException (expected)", "but got UnexpectedException (unexpected)"]
  end
  
  it "provides a useful negative failure message" do
    proc = Proc.new { raise ExpectedException, "expected" }
    matcher = RaiseErrorMatcher.new(ExpectedException, "expected")
    matcher.matches?(proc)
    matcher.negative_failure_message.should == 
      ["Expected to not get ExpectedException (expected)", ""]
  end
end
