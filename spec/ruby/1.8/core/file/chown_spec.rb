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
      not_compliant_on :jruby do
        File.chown(nil, 0, 'chown_test').should == 1
      end
      # It seems JRuby doesn't allow nil to be passed instead of -1
      platform_is :jruby do
        File.chown(-1, 0, 'chown_test').should == 1
      end
      not_compliant_on :jruby do
        File.stat('chown_test').gid.should == 0
      end
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
      not_compliant_on :jruby do
        f.chown(nil, 0).should == 0
      end
      platform_is :jruby do
        f.chown(-1, 0).should == 0
      end
      f.close
      not_compliant_on :jruby do
        File.stat('chown_test').gid.should == 0
      end
    end
  
  end

end