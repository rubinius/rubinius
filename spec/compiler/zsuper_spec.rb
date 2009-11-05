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

  relates <<-ruby do
      def x(a)
        super
      end
    ruby

    parse do
      [:defn, :x, [:args, :a], [:scope, [:block, [:zsuper]]]]
    end

    compile do |g|
      in_method :x do |d|
        d.push_local 0
        d.push_block
        d.send_super :x, 1
      end
    end
  end

  relates <<-ruby do
      def x(&block)
        super
      end
    ruby

    parse do
      [:defn, :x, [:args, :"&block"], [:scope, [:block, [:zsuper]]]]
    end

    compile do |g|
      in_method :x do |d|
        proc_lbl = d.new_label

        d.push_block
        d.dup
        d.is_nil
        d.git proc_lbl

        d.push_cpath_top
        d.find_const :Proc
        d.swap
        d.send :__from_block__, 1

        proc_lbl.set!
        d.set_local 0
        d.pop

        d.push_block
        d.send_super :x, 0
      end
    end
  end

  relates <<-ruby do
      def x(a, *as)
        super
      end
    ruby

    parse do
      [:defn, :x, [:args, :a, :"*as"], [:scope, [:block, [:zsuper]]]]
    end

    compile do |g|
      in_method :x do |d|
        d.push_local 0
        d.push_local 1
        d.cast_array
        d.push_block
        d.send_super :x, 1, true
      end
    end
  end
end
