require File.dirname(__FILE__) + '/../../spec_helper'

# Only run this spec as root, as it may fail for other users
if `whoami` == "root\n"

  def group_zero
    # HACK this is probably not the nicest way to find the name of the group
    # with id 0, but it works on OS X.
    `cat /etc/group`.split(/\n|\r/).reject { |x| !(x =~ /:0:[^:]*$/) }.first.gsub(/^([^:]*):.*$/, '\1')
  end

  describe "File.chown" do
  
    before(:each) do
      system "echo 'rubinius' > /tmp/chown_test"
      system "chown #{`whoami`.gsub(/\n/, '')} /tmp/chown_test"
    end
  
    after(:each) do
      system 'rm /tmp/chown_test' if File.exist?("/tmp/chown_test")
    end
  
    it "should change the ownerid and groupid of the file" do
      File.chown(nil, 0, '/tmp/chown_test').should == 1
      `ls -l /tmp/chown_test`.should =~ %r{#{group_zero}}
    end
  
  end


  describe "File#chown" do
  
    before(:each) do
      system "echo 'rubinius' > /tmp/chown_test"
      system "chown #{`whoami`.gsub(/\n/, '')} /tmp/chown_test"
    end
  
    after(:each) do
      system 'rm /tmp/chown_test' if File.exist?("/tmp/chown_test")
    end
  
    it "should change the ownerid and groupid of the file" do
      f = File.open('/tmp/chown_test', 'r')
      f.chown(nil, 0).should == 0
      f.close
      `ls -l /tmp/chown_test`.should =~ %r{#{group_zero}}
    end
  
  end

end