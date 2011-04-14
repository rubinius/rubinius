require File.expand_path('../../../spec_helper', __FILE__)

describe "Float#round" do
  it "returns the nearest Integer" do
    5.5.round.should == 6
    0.4.round.should == 0
    -2.8.round.should == -3
    0.0.round.should == 0
    0.49999999999999994.round.should == 0 # see http://jira.codehaus.org/browse/JRUBY-5048
  end

  ruby_version_is "1.9" do
    it "rounds self to an optionally given precision" do
      5.5.round(0).should == 6
      1.2345678.round(2).should be_close(1.23, TOLERANCE)
      123456.78.round(-2).should == 123500 # rounded up
      12.345678.round(2.9999999999999999999).should be_close(12.346, TOLERANCE)
      0.8346268.round(-2.0**30).should == 0
    end
  end

end
