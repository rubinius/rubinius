require File.expand_path('../../../spec_helper', __FILE__)

describe "Process.fork" do
  # As of 1.9 Process.respond_to?(:fork) returns false on platforms that do
  # not implement it.
  ruby_version_is "1.9" do
    platform_is :windows do
      it "is not implemented" do
        Process.respond_to?(:fork).should be_false
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
