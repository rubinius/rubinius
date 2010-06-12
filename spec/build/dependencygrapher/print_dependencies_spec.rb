require File.expand_path('../../../spec_helper', __FILE__)
require 'rakelib/dependency_grapher'

describe "DependencyGrapher#print_dependencies" do
  before :each do
    @stdout, $stdout = $stdout, IOStub.new

    @grapher = DependencyGrapher.new []
    @grapher.should_receive(:get_system_defines)
  end

  after :each do
    $stdout = @stdout
  end

  it "prints the dependencies for all object files" do
    @grapher.file_names = [fixture(__FILE__, "c.cpp"), fixture(__FILE__, "d.cpp")]
    @grapher.directories = [fixture(__FILE__)]
    @grapher.process

    @grapher.print_dependencies $stdout

    path = fixture(__FILE__)
    $stdout.should == <<EOS
#{path}/c.o: \\
  #{path}/c.cpp \\
  #{path}/c.hpp \\
  #{path}/d.hpp
#{path}/d.o: \\
  #{path}/d.cpp \\
  #{path}/d.hpp \\
  #{path}/e.hpp
EOS
  end
end
