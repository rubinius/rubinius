require File.dirname(__FILE__) + '/../../../spec_helper'

has_tty? do # needed for CI until we figure out a better way
require 'readline'
require File.dirname(__FILE__) + '/shared/size'

describe "Readline::HISTORY.size" do
  it_behaves_like :readline_history_size, :size
end
end
