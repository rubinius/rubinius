require File.expand_path('../../spec_helper', __FILE__)

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

        d.push_proc
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
