require File.dirname(__FILE__) + '/../../spec_helper'

# Only run this spec as root, as it may fail for other users
if `whoami` == "root\n"

  describe "File.chown" do
  
    before(:each) do
      File.open('chown_test', 'w') { |f| f.write('rubinius') }
    end
  
    after(:each) do
      File.delete('chown_test') if File.exist?('chown_test')
    end
  
    it "should change the ownerid and groupid of the file" do
      File.chown(nil, 0, 'chown_test').should == 1
      File.stat('chown_test').gid.should == 0
    end
  
  end


  describe "File#chown" do
  
    before(:each) do
      File.open('chown_test', 'w') { |f| f.write('rubinius') }
    end
  
    after(:each) do
      File.delete('chown_test') if File.exist?('chown_test')
    end
  
    it "should change the ownerid and groupid of the file" do
      f = File.open('chown_test', 'r')
      f.chown(nil, 0).should == 0
      f.close
      File.stat('chown_test').gid.should == 0
    end
  
  end

end