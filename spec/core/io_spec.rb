require File.dirname(__FILE__) + '/../spec_helper'

context "IO class method" do
  specify "pipe creates a two-ended pipe" do
    r, w = IO.pipe
    w.puts "test_create_pipe\\n"
    w.close
    puts r.read(16).should == "test_create_pipe"
    r.close
  end
end

context "IO instance method" do
end

