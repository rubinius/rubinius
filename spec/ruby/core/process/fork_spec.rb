require File.expand_path('../../../spec_helper', __FILE__)

describe "Process.fork" do

  ruby_version_is "1.9" do
    platform_is :windows do
      it "returns false from #respond_to?" do
        Process.respond_to?(:fork).should be_false
      end

      it "raises a NotImplementedError when called" do
        lambda { Process.fork }.should raise_error(NotImplementedError)
      end
    end
  end

  platform_is_not :windows do
    not_supported_on :jruby do
      before :each do
        @file = tmp('i_exist')
        rm_r @file
      end

      after :each do
        rm_r @file
      end

      it "return nil for the child process" do
        child_id = Process.fork
        if child_id == nil
          touch(@file) { |f| f.write 'rubinius' }
          Process.exit!
        else
          Process.waitpid(child_id)
        end
        File.exist?(@file).should == true
      end

      it "runs a block in a child process" do
        pid = Process.fork {
          touch(@file) { |f| f.write 'rubinius' }
          Process.exit!
        }
        Process.waitpid(pid)
        File.exist?(@file).should == true
      end
    end
  end
end
