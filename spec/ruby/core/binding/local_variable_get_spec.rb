require File.expand_path('../../../spec_helper', __FILE__)

describe 'Binding#local_variable_get' do
  it 'gets a local variable defined before the Binding' do
    number = 10

    binding.local_variable_get(:number).should == 10
  end

  it 'gets a local variable defined in the Binding' do
    bind = binding

    bind.local_variable_set(:number, 10)
    bind.local_variable_get(:number).should == 10
  end

  it 'gets a local variable defined using eval()' do
    bind = binding

    bind.eval('number = 10')

    bind.local_variable_get(:number).should == 10
  end

  it 'gets a local variable defined in a parent scope' do
    number = 10

    proc { binding.local_variable_get(:number) }.call.should == 10
  end

  it 'raises NameError for an undefined local variable' do
    proc { binding.local_variable_get(:cats) }.should raise_error(NameError)
  end
end
