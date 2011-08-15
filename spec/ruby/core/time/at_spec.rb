require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/methods', __FILE__)

describe "Time.at" do
  platform_is_not :windows do
    ruby_version_is '' ... '1.9' do
      it "converts to time object" do
        with_timezone "GMT" do
          # the #chomp calls are necessary because of RSpec
          t = Time.at(1184027924)
          t.inspect.chomp.should == "Tue Jul 10 00:38:44 +0000 2007"
        end
      end
    end

    ruby_version_is '1.9' do
      it "converts to time object" do
        with_timezone "GMT" do
          # the #chomp calls are necessary because of RSpec
          t = Time.at(1184027924)
          t.inspect.chomp.should == "2007-07-10 00:38:44 +0000"
        end
      end
    end
  end

  it "creates a new time object with the value given by time" do
    t = Time.now
    Time.at(t).inspect.should == t.inspect
  end

  it "creates a dup time object with the value given by time" do
    t1 = Time.new
    t2 = Time.at(t1)
    t1.object_id.should_not == t2.object_id
  end

  it "is able to create a time object with a float" do
    t = Time.at(10.5)
    t.usec.should == 500000.0
    t.should_not == Time.at(10)
  end

  it "is able to create a time object with a microseconds" do
    t = Time.at(10, 500000)
    t.usec.should == 500000.0
    t.should_not == Time.at(10)
  end

  it "returns a subclass instance on a Time subclass" do
    c = Class.new(Time)
    t = c.at(0)
    t.should be_kind_of(c)
  end

  it "returns a subclass instance when passed a Time object" do
    c = Class.new(Time)
    t = c.at(Time.now)
    t.should be_kind_of(c)
  end

end
