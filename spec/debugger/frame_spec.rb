require File.expand_path('../spec_helper', __FILE__)

describe "Rubinius::Debugger::Frame" do
  before do
    compiled_method = def ten
      a + 7
    end

    variable_scope = Rubinius::VariableScope.new
    variable_scope.set_eval_local(:a, 3)
    variable_scope.instance_eval { @method = compiled_method }

    location = Rubinius::Location.new
    location.instance_eval do
      @variables = variable_scope
      @method = compiled_method
      @static_scope = compiled_method.scope
    end

    @debugger = mock('debugger')
    @frame = Rubinius::Debugger::Frame.new(@debugger, 1, location)
  end

  describe "#run" do
    it 'runs the code within the frame binding' do
      @frame.run('a + 3').should == 6
    end
  end

end
