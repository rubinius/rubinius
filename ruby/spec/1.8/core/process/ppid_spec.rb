require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.ppid" do
  it "returns the process id of the parent of this process" do
    @file = '/tmp/ppid'
    File.delete(@file) if File.exist?(@file)

    child_pid = Process.fork {
      File.open(@file,'w'){|f| f.write Process.ppid }
    }
    Process.wait(child_pid)
    File.read(@file).to_i.should == Process.pid
  end
end
