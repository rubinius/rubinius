require File.dirname(__FILE__) + '/../../spec_helper'

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
    
    # HACK this is probably not the nicest way to find the name of the group
    # with id 0, but it works on OS X.
    group = `cat /etc/group`.split(/\n|\r/).reject { |x| !(x =~ /:0:[^:]*$/) }.first.gsub(/^([^:]*):.*$/, '\1')
    `ls -l /tmp/chown_test`.should =~ %r{#{group}}
  end
  
end

