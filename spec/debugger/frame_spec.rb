require File.expand_path('../spec_helper', __FILE__)

describe "Rubinius::Debugger::Frame" do
  before do
    compiled_method = def my_method; end

    variable_scope = Rubinius::VariableScope.new
    variable_scope.set_eval_local(:a, 3)
    variable_scope.instance_eval { @method = compiled_method }

    @location = Rubinius::Location.new
    @location.instance_eval do
      @variables = variable_scope
      @method = compiled_method
      @static_scope = compiled_method.scope
      @ip = 0
    end

    @debugger = Rubinius::Debugger.new
    @frame = Rubinius::Debugger::Frame.new(@debugger, 1, @location)
  end

  describe "#run" do
    it 'runs the code within the frame binding' do
      @frame.run('a + 3').should == 6
    end
  end

  describe "#describe" do
    it 'shows the location of the compiled method' do
      @frame.describe.should =~ /NilClass#my_method/
    end
  end

  describe "#method" do
    it "is delegated to location" do
      @frame.method.should == @location.method
    end
  end

  describe "#ip" do
    it "is delegated to location" do
      @frame.ip.should == @location.ip
    end
  end

  describe "#variables" do
    it "is delegated to location" do
      @frame.variables.dynamic_locals[:a].should == 3
    end
  end
end
