require File.dirname(__FILE__) + '/../../spec_helper'

# TODO: share with Kernel.abort, abort.
describe "Process.abort" do
  before :each do
    @name = tmp("process_abort.txt")
  end

  after :each do
    rm_r @name
  end

  platform_is_not :windows do
    it "terminates execution immediately" do
      Process.fork do
        Process.abort
        touch(@name) { |f| f.write 'rubinius' }
      end

      File.exists?(@name).should == false
    end
  end
end
