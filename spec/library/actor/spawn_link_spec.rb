require File.expand_path('../../../spec_helper', __FILE__)
require 'actor'

describe "Actor.spawn_link" do
  it "sends an exit message to linked Actors" do
    actor = Actor.spawn_link do
      Actor.spawn_link do
        Actor.spawn_link do
          Actor.receive do |m|
            m.when(:die) { raise 'dying' }
          end
        end
      end
    end

    actor << :die
    msg = Actor.receive
    msg[0].should == :exit
  end
end
