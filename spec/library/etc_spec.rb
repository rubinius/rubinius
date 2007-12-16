require File.dirname(__FILE__) + '/../spec_helper'
require 'etc'

describe "Etc.getlogin" do
  it "returns the name of the user who runs this process" do
    Etc.getlogin.should == `whoami`.strip
  end
end

describe "Etc.getpwnam" do
  it "returns a Passwd struct instance for the given user" do
    pw = Etc.getpwnam(`whoami`.strip)

    noncompliant :rubinius do
      pw.is_a?(Etc::Passwd).should == true
    end

    compliant :ruby do
      pw.is_a?(Struct::Passwd).should == true
    end
  end

  it "only accepts strings as argument" do
    lambda {
      Etc.getpwnam(123)
      Etc.getpwnam(nil)
    }.should raise_error(TypeError)
  end
end

describe "Etc.getpwuid" do
  it "returns a Passwd struct instance for the given user" do
    pw = Etc.getpwuid(`id -u`.strip.to_i)

    noncompliant :rubinius do
      pw.is_a?(Etc::Passwd).should == true
    end

    compliant :ruby do
      pw.is_a?(Struct::Passwd).should == true
    end
  end

  it "only accepts integers as argument" do
    lambda {
      Etc.getpwuid("foo")
      Etc.getpwuid(nil)
    }.should raise_error(TypeError)
  end

  noncompliant :rubinius, :ruby19 do
    it "uses Process.uid as the default value for the argument" do
      pw = Etc.getpwuid
    end
  end
end

describe "Etc.getgrnam" do
  it "returns a Group struct instance for the given group" do
    gr = Etc.getgrnam("daemon")

    noncompliant :rubinius do
      gr.is_a?(Etc::Group).should == true
    end

    compliant :ruby do
      gr.is_a?(Struct::Group).should == true
    end
  end

  it "only accepts strings as argument" do
    lambda {
      Etc.getgrnam(123)
      Etc.getgrnam(nil)
    }.should raise_error(TypeError)
  end
end

describe "Etc.getgrgid" do
  before(:all) do
    @gid = `id -g`.strip.to_i
    @name = `id -gn`.strip
  end

  noncompliant :rubinius, :ruby19 do
    it "returns a Group struct instance for the given user" do
      gr = Etc.getgrgid(@gid)

      noncompliant :rubinius do
        gr.is_a?(Etc::Group).should == true
      end

      compliant :ruby19 do
        gr.is_a?(Struct::Group).should == true
      end

      gr.gid.should == @gid
      gr.name.should == @name
    end
  end

  compliant :ruby do
    platform :not, :darwin do
      it "ignores its argument" do
        lambda { Etc.getgrgid("foo") }.should raise_error(TypeError)
        Etc.getgrgid(42)
        Etc.getgrgid(9876)
      end
    end

    it "returns a Group struct instance for the current user's group" do
      gr = Etc.getgrgid(@gid)
      gr.is_a?(Struct::Group).should == true
      gr.gid.should == @gid
      gr.name.should == @name
    end
  end

  noncompliant :rubinius, :ruby19 do
    it "only accepts integers as argument" do
      lambda {
        Etc.getgrgid("foo")
        Etc.getgrgid(nil)
      }.should raise_error(TypeError)
    end
  end

  noncompliant :rubinius, :ruby19 do
    it "uses Process.gid as the default value for the argument" do
      gr = Etc.getgrgid

      gr.gid.should == @gid
      gr.name.should == @name
    end
  end
end
