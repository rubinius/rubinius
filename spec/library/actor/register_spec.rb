require File.expand_path('../../../spec_helper', __FILE__)
require 'actor'

describe "Actor registration" do
  it "stores actors by identifier" do
    actor = Actor.spawn(Actor.current) do |master|
      Actor[:foo] = Actor.current
      msg = Actor.receive
      master.send msg

      master.send Actor.receive
    end

    actor.send "phase1"
    Actor.receive.should == "phase1"

    # We know that the actor is now between phases, so the registeration
    # must be visible by now.
    Actor[:foo].should == actor

    actor.send "phase2"
    Actor.receive.should == "phase2"

    Actor[:foo] = nil
    Actor[:foo].should == nil
  end
end
