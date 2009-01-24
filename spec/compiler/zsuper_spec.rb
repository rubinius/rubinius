require File.dirname(__FILE__) + '/../spec_helper'

describe "A Zsuper node" do
  relates <<-ruby do
      def x
        super
      end
    ruby

    parse do
      [:defn, :x, [:args], [:scope, [:block, [:zsuper]]]]
    end

    compile do |g|
      in_method :x do |d|
        d.push_block
        d.send_super :x, 0
      end
    end
  end
end
