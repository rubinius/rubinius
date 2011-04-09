require File.expand_path('../../../spec_helper', __FILE__)
require 'actor'

describe "Actor#trap_exit" do
  it "accesses the trap_exit flag for the current actor" do
    sync = Rubinius::Channel.new
    Actor.spawn do
      begin
        Actor.trap_exit?.should be_false
        Actor.trap_exit.should be_false
        Actor.trap_exit = true
        Actor.trap_exit?.should be_true
        Actor.trap_exit.should be_true
        Actor.trap_exit = false
        Actor.trap_exit?.should be_false
        Actor.trap_exit.should be_false
      rescue Exception => e 
        sync << e
      ensure
        sync << nil
      end
    end
    ex = sync.receive
    raise ex if ex
  end
end
