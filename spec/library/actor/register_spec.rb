require File.dirname(__FILE__) + '/../../spec_helper'
require 'actor'

describe "Actor.register with [identifier, actor]" do
  it "stores actors by identifier" do
    actor = Actor.spawn(Actor.current) do |master|
      Actor.register(:foo, Actor.current)
      msg = Actor.receive
      master.send msg
    end

    Actor.lookup(:foo).should == actor

    actor.send "test"
    msg = Actor.receive
    msg.should == "test"

    Actor.unregister(:foo)
    Actor.lookup(:foo).should == nil
  end
end
