require File.expand_path('../../../spec_helper', __FILE__)
require 'actor'

describe "Actor#notify_exited" do
  it "should kill actors not trapping exits" do
    sync = Rubinius::Channel.new
    actor = Actor.spawn do
      begin
        Actor.receive
      rescue Exception => exc
        sync << exc
      ensure
        sync << nil
      end
    end
    actor.notify_exited(:actor, :reason)
    ex = sync.receive
    ex.actor.should == :actor
    ex.reason.should == :reason
  end
end
