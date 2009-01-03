require File.dirname(__FILE__) + '/../../spec_helper'

describe "ARGF.binmode" do
  before :each do
    @file1    = fixture __FILE__, "file1.txt"
    @file2    = fixture __FILE__, "file2.txt"
    @bin_file = fixture __FILE__, "bin_file.txt"
  end

  after :each do
    ARGF.close
  end

  it "returns self" do
    argv [@bin_file] do
      ARGF.binmode.should equal(ARGF)
    end
  end

  platform_is :windows do
    it "puts reading into binmode" do
      argv [@bin_file, @bin_file] do
        ARGF.gets.should == "test\n"
        ARGF.binmode
        ARGF.gets.should == "test\r\n"
      end
    end
    
    it "puts alls subsequent stream reading through ARGF into binmode" do
      argv [@bin_file, @bin_file, @bin_file, @bin_file] do
        ARGF.gets.should == "test\n"
        ARGF.binmode
        ARGF.gets.should == "test\r\n"
        ARGF.gets.should == "test\r\n"
        ARGF.gets.should == "test\r\n"
      end
    end
  end
  
  platform_is_not :windows do
    # This does nothing on Unix but it should not raise any errors.
    it "does not raise an error" do
      argv [@bin_file] do
        lambda { ARGF.binmode }.should_not raise_error
      end
    end
  end
end
