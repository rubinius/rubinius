require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is ""..."1.9" do

  require 'ftools'

  describe "File.copy" do
    before(:each) do
      system "echo 'hello rubinius' > copy_test"
      system "chmod a+x copy_test"
    end
    
    after(:each) do
      File.unlink "copy_test"
      File.unlink "copy_test_dest" rescue nil
    end
    
    it "copies the file at 1st arg to the file at 2nd arg" do
      File.copy("copy_test", "copy_test_dest")
      fd = File.open("copy_test_dest")
      data = fd.read
      data.should == "hello rubinius\n"
      fd.close
      
      omode = File.stat("copy_test").mode
      mode = File.stat("copy_test_dest").mode
      
      omode.should == mode
    end
  end
end
