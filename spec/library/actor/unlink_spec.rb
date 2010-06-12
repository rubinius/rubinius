require File.expand_path('../../../spec_helper', __FILE__)
require 'actor'

describe "Actor.unlink" do
  it "sends an exit message to linked Actors" do
    actor = Actor.spawn(Actor.current) do |master|
      Actor.link(master)

      Actor.receive do |f|
        f.when(:ping) { Actor.unlink(master) }
      end

      raise "foo"
    end

    actor.send :ping
    msg = Actor.receive do |f|
      f.after(0) do end
    end

    msg.should == nil
  end
end
