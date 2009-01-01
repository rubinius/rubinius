require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.lchmod" do
  platform_is_not :os => [:linux, :windows, :openbsd] do
    before :each do
      @fname = tmp('file_chmod_test')
      @lname = @fname + '.lnk'
      File.delete @fname rescue nil
      File.delete @lname rescue nil
      File.open(@fname, 'w') { |f| f.write "rubinius" }
      File.symlink @fname, @lname
    end
    
    after :each do
      # the link should be removed first
      File.delete @lname if File.exist? @lname
      File.delete @fname if File.exist? @fname
    end
    
    it "changes the file mode of the link and not of the file" do
      File.chmod(0222, @lname).should  == 1
      File.lchmod(0755, @lname).should == 1
      
      File.lstat(@lname).executable?.should == true
      File.lstat(@lname).readable?.should   == true
      File.lstat(@lname).writable?.should   == true
  
      File.stat(@lname).executable?.should == false
      File.stat(@lname).readable?.should   == false
      File.stat(@lname).writable?.should   == true
    end
  end
end
