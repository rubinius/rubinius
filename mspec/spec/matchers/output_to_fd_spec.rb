require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../expectations'
require File.dirname(__FILE__) + '/../../matchers/output_to_fd'

describe OutputToFDMatcher do
  # Figure out how in the hell to achieve this
  it "matches when running the block produces the expected output to the given FD" do
    s = "Hi\n"
    output_to_fd(s, STDERR).matches?(lambda { $stderr.print s }).should == true
  end

  it "does not match if running the block does not produce the expected output to the FD" do
    s = "Hi\n"
    output_to_fd(s, STDERR).matches?(lambda { $stderr.puts(s + s) }).should == false
  end

  it "defaults to matching against STDOUT" do
    s = "Hi\n"
    output_to_fd(s).matches?(lambda { $stdout.print s }).should == true
  end

  it "allows matching with a Regexp" do
    s = "Hi there\n"
    output_to_fd(/Hi/, STDERR).matches?(lambda { $stderr.print s }).should == true
    output_to_fd(/Hi?/, STDERR).matches?(lambda { $stderr.print s }).should == true
    output_to_fd(/[hH]i?/, STDERR).matches?(lambda { $stderr.print s }).should == true
    output_to_fd(/.*/, STDERR).matches?(lambda { $stderr.print s }).should == true
    output_to_fd(/H.*?here/, STDERR).matches?(lambda { $stderr.print s }).should == true
    output_to_fd(/Ahoy/, STDERR).matches?(lambda { $stderr.print s }).should == false
  end
end
