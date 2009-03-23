require File.dirname(__FILE__) + '/../spec_helper'

describe "A Postexe node" do
  relates "END { 1 }" do
    parse do
      [:iter, [:postexe], nil, [:lit, 1]]
    end

    compile do |g|
      g.push :self

      in_block_send :at_exit, :none do |d|
        d.push 1
      end
    end
  end
end
