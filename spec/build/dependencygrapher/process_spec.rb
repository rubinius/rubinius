require File.expand_path('../../../spec_helper', __FILE__)
require 'rakelib/dependency_grapher'

describe "DependencyGrapher#process" do
  before :each do
    @grapher = DependencyGrapher.new []
    @grapher.should_receive(:get_system_defines)
  end

  it "maps zero dependencies" do
    @grapher.file_names = [fixture(__FILE__, "a.cpp")]
    @grapher.process

    dep = @grapher.sources.first
    dep.dependencies.should == [fixture(__FILE__, "a.cpp")]
  end

  it "maps one dependency" do
    @grapher.file_names = [fixture(__FILE__, "b.cpp")]
    @grapher.directories = [fixture(__FILE__)]
    @grapher.process

    dep = @grapher.sources.first
    dep.dependencies.should == [
      fixture(__FILE__, "b.cpp"),
      fixture(__FILE__, "b.hpp")
    ]
  end

  it "maps one transitive dependency" do
    @grapher.file_names = [fixture(__FILE__, "c.cpp")]
    @grapher.directories = [fixture(__FILE__)]
    @grapher.process

    dep = @grapher.sources.first
    dep.dependencies.should == [
      fixture(__FILE__, "c.cpp"),
      fixture(__FILE__, "c.hpp"),
      fixture(__FILE__, "d.hpp")
    ]
  end

  it "maps two direct dependencies" do
    @grapher.file_names = [fixture(__FILE__, "d.cpp")]
    @grapher.directories = [fixture(__FILE__)]
    @grapher.process

    dep = @grapher.sources.first
    dep.dependencies.should == [
      fixture(__FILE__, "d.cpp"),
      fixture(__FILE__, "d.hpp"),
      fixture(__FILE__, "e.hpp")
    ]
  end

  it "maps direct and transitive dependencies" do
    @grapher.file_names = [fixture(__FILE__, "e.cpp")]
    @grapher.directories = [fixture(__FILE__)]
    @grapher.process

    dep = @grapher.sources.first
    dep.dependencies.should == [
      fixture(__FILE__, "c.hpp"),
      fixture(__FILE__, "d.hpp"),
      fixture(__FILE__, "e.cpp"),
      fixture(__FILE__, "e.hpp"),
      fixture(__FILE__, "f.hpp")
    ]
  end

  it "includes a file in an \#ifdef that evaluates to true" do
    @grapher.file_names = [fixture(__FILE__, "f.cpp")]
    @grapher.directories = [fixture(__FILE__)]
    @grapher.system_defines = { "F" => "1" }
    @grapher.process

    dep = @grapher.sources.first
    dep.dependencies.should == [
      fixture(__FILE__, "f.cpp"),
      fixture(__FILE__, "f.hpp")
    ]
  end

  it "includes a file in an \#ifndef that evaluates to true" do
    @grapher.file_names = [fixture(__FILE__, "g.cpp")]
    @grapher.directories = [fixture(__FILE__)]
    @grapher.system_defines = { }
    @grapher.process

    dep = @grapher.sources.first
    dep.dependencies.should == [
      fixture(__FILE__, "g.cpp"),
      fixture(__FILE__, "g.hpp")
    ]
  end

  it "includes a file in an \#if that evaluates to true" do
    @grapher.file_names = [fixture(__FILE__, "h.cpp")]
    @grapher.directories = [fixture(__FILE__)]
    @grapher.process

    dep = @grapher.sources.first
    dep.dependencies.should == [
      fixture(__FILE__, "h.cpp"),
      fixture(__FILE__, "h.hpp")
    ]
  end

  it "includes a file in an \#elif that evaluates to true" do
    @grapher.file_names = [fixture(__FILE__, "i.cpp")]
    @grapher.directories = [fixture(__FILE__)]
    @grapher.process

    dep = @grapher.sources.first
    dep.dependencies.should == [
      fixture(__FILE__, "i.cpp"),
      fixture(__FILE__, "i.hpp")
    ]
  end

  it "does not include a file in an \#ifdef that evaluates to false" do
    @grapher.file_names = [fixture(__FILE__, "f.cpp")]
    @grapher.directories = [fixture(__FILE__)]
    @grapher.system_defines = { }
    @grapher.process

    dep = @grapher.sources.first
    dep.dependencies.should == [
      fixture(__FILE__, "f.cpp")
    ]
  end

  it "does not include a file in an \#ifndef that evaluates to false" do
    @grapher.file_names = [fixture(__FILE__, "g.cpp")]
    @grapher.directories = [fixture(__FILE__)]
    @grapher.system_defines = { "E" => "1" }
    @grapher.process

    dep = @grapher.sources.first
    dep.dependencies.should == [
      fixture(__FILE__, "g.cpp")
    ]
  end

  it "does not include a file in an \#if that evaluates to false" do
    @grapher.file_names = [fixture(__FILE__, "j.cpp")]
    @grapher.directories = [fixture(__FILE__)]
    @grapher.process

    dep = @grapher.sources.first
    dep.dependencies.should == [
      fixture(__FILE__, "j.cpp")
    ]
  end

  it "does not include a file in an \#elif that evaluates to false" do
    @grapher.file_names = [fixture(__FILE__, "k.cpp")]
    @grapher.directories = [fixture(__FILE__)]
    @grapher.process

    dep = @grapher.sources.first
    dep.dependencies.should == [
      fixture(__FILE__, "k.cpp")
    ]
  end

  it "only includes one entry for a file that occurs multiple times" do
    @grapher.file_names = [fixture(__FILE__, "l.cpp")]
    @grapher.directories = [fixture(__FILE__)]
    @grapher.process

    dep = @grapher.sources.first
    dep.dependencies.should == [
      fixture(__FILE__, "c.hpp"),
      fixture(__FILE__, "d.hpp"),
      fixture(__FILE__, "l.cpp")
    ]
  end

  it "raises an Errno::ENOENT if an included file is not found" do
    @grapher.file_names = [fixture(__FILE__, "q.cpp")]
    @grapher.directories = [fixture(__FILE__)]

    lambda { @grapher.process }.should raise_error(Errno::ENOENT)
  end
end
