require File.expand_path('../../spec_helper', __FILE__)

describe "A Postexe node" do
  relates "END { 1 }" do
    compile do |g|
      g.push :self

      in_block_send :at_exit, :none do |d|
        d.push 1
      end
    end
  end
end
