require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/helpers/environment'

describe "#env" do
  before(:all) do
    @ruby_platform = Object.const_get :RUBY_PLATFORM
  end

  after(:all) do
    Object.const_set :RUBY_PLATFORM, @ruby_platform
  end

  it "returns a hash of variables" do
    env.class.should == Hash
  end
  
  it "calls `env` on non-Windows" do
    Object.const_set :RUBY_PLATFORM, "notwindows"
    should_receive(:`).with("env").and_return("one=two\nthree=four")
    env
  end

  it "calls `cmd.exe /C set` on Windows (mswin32)" do
    Object.const_set :RUBY_PLATFORM, "mswin32"
    should_receive(:`).with("cmd.exe /C set").and_return("one=two\nthree=four")
    env
  end

  it "calls `cmd.exe /C set` on Windows (mingw)" do
    Object.const_set :RUBY_PLATFORM, "mingw"
    should_receive(:`).with("cmd.exe /C set").and_return("one=two\nthree=four")
    env
  end

  it "returns the current user's environment variables" do
    Object.const_set :RUBY_PLATFORM, "notwindows"
    should_receive(:`).with("env").and_return("one=two\nthree=four")
    env.should == {"one" => "two", "three" => "four"}

    Object.const_set :RUBY_PLATFORM, "mswin32"
    should_receive(:`).with("cmd.exe /C set").and_return("five=six\nseven=eight")
    env.should == {"five" => "six", "seven" => "eight"}
  end
end

describe "#username" do
  before(:all) do
    @ruby_platform = Object.const_get :RUBY_PLATFORM
  end

  after(:all) do
    Object.const_set :RUBY_PLATFORM, @ruby_platform
  end

  it "calls `cmd.exe /C ECHO %USERNAME%` on Windows (mswin32)" do
    Object.const_set :RUBY_PLATFORM, "mswin32"
    should_receive(:`).with("cmd.exe /C ECHO %USERNAME%").and_return("john")
    username
  end

  it "calls `cmd.exe /C ECHO %USERNAME%` on Windows (mingw)" do
    Object.const_set :RUBY_PLATFORM, "mingw"
    should_receive(:`).with("cmd.exe /C ECHO %USERNAME%").and_return("john")
    username
  end

  it "calls `env` on non-Windows" do
    Object.const_set :RUBY_PLATFORM, "notwindows"
    should_receive(:`).with("whoami").and_return("john")
    username
  end

  it "returns the user's username" do
    Object.const_set :RUBY_PLATFORM, "mswin32"
    should_receive(:`).with("cmd.exe /C ECHO %USERNAME%").and_return("johnonwin")
    username.should == "johnonwin"

    Object.const_set :RUBY_PLATFORM, "notwindows"
    should_receive(:`).with("whoami").and_return("john")
    username.should == "john"
  end
end
