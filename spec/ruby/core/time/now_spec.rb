require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/shared/now'

describe 'Time.now' do
  it_behaves_like(:time_now, :now)
end
