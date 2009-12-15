require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

platform_is_not :windows do
  not_supported_on :jruby do
    as_superuser do
      describe "Dir.chroot as root" do
        before :all do
          DirSpecs.create_mock_dirs

          @real_root = "../" * (File.dirname(__FILE__).count('/') - 1)
          @ref_dir = File.join("/", Dir.new('/').entries.first)
        end
        
        after(:all) do
          until File.exists?(@ref_dir)
            Dir.chroot("../") or break
          end

          DirSpecs.delete_mock_dirs
        end

        it "can be used to change the process' root directory" do
          lambda { Dir.chroot(File.dirname(__FILE__)) }.should_not raise_error
          File.exists?("/#{File.basename(__FILE__)}").should be_true
        end

        it "returns 0 if successful" do
          Dir.chroot('/').should == 0
        end

        it "raises an Errno::ENOENT exception if the directory doesn't exist" do
          lambda { Dir.chroot('xgwhwhsjai2222jg') }.should raise_error(Errno::ENOENT)
        end

        it "can be escaped from with ../" do
          Dir.chroot(@real_root)
          File.exists?(@ref_dir).should be_true
          File.exists?("/#{File.basename(__FILE__)}").should be_false
        end

        ruby_version_is "1.9" do
          it "calls #to_path on non-String argument" do
            p = mock('path')
            p.should_receive(:to_path).and_return(@real_root)
            Dir.chroot(p)
          end
        end
      end
    end

    as_user do
      describe "Dir.chroot as regular user" do
        before :all do
          DirSpecs.create_mock_dirs
        end

        after :all do
          DirSpecs.delete_mock_dirs
        end

        it "raises an Errno::EPERM exception if the directory exists" do
          lambda { Dir.chroot('.') }.should raise_error(Errno::EPERM)
        end

        it "raises a SystemCallError if the directory doesn't exist" do
          lambda { Dir.chroot('xgwhwhsjai2222jg') }.should raise_error(SystemCallError)
        end

        ruby_version_is "1.9" do
          it "calls #to_path on non-String argument" do
            p = mock('path')
            p.should_receive(:to_path).and_return('.')
            lambda { Dir.chroot(p) }.should raise_error
          end
        end
      end
    end
  end
end
