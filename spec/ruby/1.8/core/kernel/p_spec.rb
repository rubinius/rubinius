require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#p" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("p")
  end
  
  it "flushes output if receiver is a File" do
    filename = "/tmp/Kernel_p_flush" + $$.to_s
    begin
      File.open(filename, "w") do |f|
        begin
          old_stdout = $stdout
          $stdout = f
          p("abcde")
        ensure
          $stdout = old_stdout
        end

        File.open(filename) do |f2|
          f2.read(7).should == "\"abcde\""
        end
      end
    ensure
      File.delete(filename) rescue nil
    end
  end

=begin Not sure how to spec this, but wanted to note the behavior here  
  it "does not flush if receiver is not a TTY or a File" do
  end 
=end
end
