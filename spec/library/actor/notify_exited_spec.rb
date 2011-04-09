require File.expand_path('../../../spec_helper', __FILE__)
require 'actor'

describe "Actor#notify_exited" do
  it "kills actors not trapping exits" do
    sync = Rubinius::Channel.new
    actor = Actor.spawn do
      begin
        Actor.receive
      rescue Exception => e
        sync << e
      ensure
        sync << nil
      end
    end
    actor2 = Actor.spawn {}
    actor.notify_exited(actor2, :reason)
    ex = sync.receive
    ex.should be_an_instance_of(Actor::DeadActorError)
    ex.actor.should == actor2
    ex.reason.should == :reason
  end

  it "delivers a message to an actor trapping exits" do
    sync = Rubinius::Channel.new
    actor = Actor.spawn do
      Actor.trap_exit = true
      sync << nil
      begin
        sync << Actor.receive
      ensure
        sync << nil
      end
    end
    actor2 = Actor.spawn {}
    sync.receive
    actor.notify_exited(actor2, :reason)
    ex = sync.receive
    ex.should be_an_instance_of(Actor::DeadActorError)
    ex.actor.should == actor2
    ex.reason.should == :reason
  end
end
