require File.dirname(__FILE__) + '/../../spec_helper'

# TODO: share with Kernel.abort, abort.
describe "Process.abort" do
  platform_is_not :windows do
    it "terminates execution immediately" do
      @file = '/tmp/i_exist'
      File.delete(@file) if File.exist?(@file)
      pid = Process.fork {
        Process.abort
        File.open(@file,'w'){|f| f.write 'rubinius'}
      }
      File.exist?(@file).should == false
    end
  end
end
