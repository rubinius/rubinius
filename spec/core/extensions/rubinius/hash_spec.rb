require File.dirname(__FILE__) + '/../../../spec_helper'

only :rbx do
  # Parser chockes on this as of 1334
  # context 'Creating a Hash' do
  #  specify 'Hash[] is able to process key, val arguments followed by key => val arguments' do
  #    Hash[:a, 1, :b, 2, :c => 3].should == {:a => 1, :b => 2, :c => 3}
  #  end
  # end
end
