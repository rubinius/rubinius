require File.expand_path('../../../spec_helper', __FILE__)
require 'actor'

describe "Actor.spawn" do
  before :each do
    @master = Actor.current
  end

  it "creates functioning actors" do
    Data = Struct.new :value
    NUM_ACTORS = 10

    actors = []
    results = []
    NUM_ACTORS.times do 
      actors << Actor.spawn(@master) do |master|
        Actor.receive do |filter|
          filter.when Data do |msg|
            @master.send(msg.value)
          end
        end
      end
    end

    actors.each_with_index do |actor, i|
      actor.send(Data.new(i+1))
    end

    NUM_ACTORS.times { |i| results << Actor.receive }
    results.sort.should == (1..NUM_ACTORS).to_a
  end
end
