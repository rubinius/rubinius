require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.fork" do
  not_supported_on :jruby, :windows do
    before :each do
      @file = tmp('i_exist')
      File.delete(@file) if File.exist?(@file)
    end

    after :each do
      File.delete(@file) if File.exist?(@file)
    end

    it "return nil for the child process" do
      child_id = Process.fork
      if child_id == nil
        File.open(@file,'w'){|f| f.write 'rubinius'}
        Process.exit!
      else
        Process.waitpid(child_id)
      end
      File.exist?(@file).should == true
    end

    it "runs a block in a child process" do
      pid = Process.fork {
        File.open(@file,'w'){|f| f.write 'rubinius'}
        Process.exit!
      }
      Process.waitpid(pid)
      File.exist?(@file).should == true
    end
  end
end
