require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/methods', __FILE__)
require File.expand_path('../shared/now', __FILE__)

describe 'Time.new' do
  it_behaves_like(:time_now, :new)
end
