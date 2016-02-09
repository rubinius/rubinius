require File.expand_path('../../../spec_helper', __FILE__)

describe 'Binding#local_variable_set' do
  it 'sets a new local variable' do
    bind = binding

    bind.local_variable_set(:number, 10)
    bind.local_variable_get(:number).should == 10
  end

  it 'sets a local variable using a String as the variable name' do
    bind = binding

    bind.local_variable_set('number', 10)
    bind.local_variable_get('number').should == 10
  end

  it 'sets a local variable using an object responding to #to_str as the variable name' do
    bind = binding
    name = mock(:obj)

    name.stub!(:to_str).and_return('number')

    bind.local_variable_set(name, 10)
    bind.local_variable_get(name).should == 10
  end

  it 'scopes new local variables to the receiving Binding' do
    bind = binding

    bind.local_variable_set(:number, 10)

    proc { number }.should raise_error(NameError)
  end

  it 'overwrites an existing local variable defined before a Binding' do
    number = 10
    bind   = binding

    bind.local_variable_set(:number, 20)

    number.should == 20
  end

  it 'overwrites a local variable defined using eval()' do
    bind = binding

    bind.eval('number = 10')

    bind.local_variable_set(:number, 20)

    bind.local_variable_get(:number).should == 20
  end
end
