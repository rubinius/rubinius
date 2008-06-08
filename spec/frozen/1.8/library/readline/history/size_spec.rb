require File.dirname(__FILE__) + '/../../../spec_helper'
require 'readline'
require File.dirname(__FILE__) + '/shared/size'

describe "Readline::HISTORY.size" do
  it_behaves_like :readline_history_size, :size
end
