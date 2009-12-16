require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

require File.dirname(__FILE__) + '/shared/wakeup'

describe "Thread#run" do
  it_behaves_like :thread_wakeup, :run
end

