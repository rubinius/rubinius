require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe 'Method#to_proc' do
  it 'initializes a block' do
    MethodSpecs::Methods.new.method(:zero).to_proc.block.should_not == nil
  end
end
