require File.dirname(__FILE__) + '/../spec_helper'

describe "A Zsuper node" do
  relates <<-ruby do
      def x
        super
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.push_block
        d.zsuper :x
      end
    end
  end

  relates <<-ruby do
      def x(a)
        super
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.push_block
        d.zsuper :x
      end
    end
  end

  relates <<-ruby do
      def x(&block)
        super
      end
    ruby

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
        d.zsuper :x
      end
    end
  end

  relates <<-ruby do
      def x(a, *as)
        super
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.push_block
        d.zsuper :x
      end
    end
  end
end
