require File.expand_path('../../spec_helper', __FILE__)

describe "A Flip3 node" do
  relates <<-ruby do
      x = if ((i % 4) == 0)...((i % 3) == 0) then
        i
      else
        nil
      end
    ruby

    # flip3
  end
end
