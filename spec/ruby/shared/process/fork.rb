describe :process_fork, :shared => true do
  platform_is_not :windows do
    not_supported_on :jruby do
      before :each do
        @file = tmp('i_exist')
        rm_r @file
      end

      after :each do
        rm_r @file
      end

      it "returns nil for the child process" do
        child_id = @object.fork
        if child_id == nil
          touch(@file) { |f| f.write 'rubinius' }
          Process.exit!
        else
          Process.waitpid(child_id)
        end
        File.exist?(@file).should == true
      end

      it "runs a block in a child process" do
        pid = @object.fork {
          touch(@file) { |f| f.write 'rubinius' }
          Process.exit!
        }
        Process.waitpid(pid)
        File.exist?(@file).should == true
      end
    end
  end
end
