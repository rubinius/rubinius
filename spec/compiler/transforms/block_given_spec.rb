require File.expand_path('../../../spec_helper', __FILE__)

describe "A Call node using BlockGiven transform" do
  block_given = lambda do |g|
    in_method :m do |d|
      no_blk = d.new_label
      done = d.new_label

      d.push_has_block
      d.gif no_blk

      d.push 1
      d.goto done

      no_blk.set!
      d.push :nil

      done.set!
    end
  end

  relates <<-ruby do
      def m
        1 if block_given?
      end
    ruby

    compile do |g|
      in_method :m do |d|
        no_blk = d.new_label
        done = d.new_label

        d.push :self
        d.send :block_given?, 0, true
        d.gif no_blk

        d.push 1
        d.goto done

        no_blk.set!
        d.push :nil

        done.set!
      end
    end

    compile(:block_given, &block_given)
  end

  relates <<-ruby do
      def m
        1 if iterator?
      end
    ruby

    compile do |g|
      in_method :m do |d|
        no_blk = d.new_label
        done = d.new_label

        d.push :self
        d.send :iterator?, 0, true
        d.gif no_blk

        d.push 1
        d.goto done

        no_blk.set!
        d.push :nil

        done.set!
      end
    end

    compile(:block_given, &block_given)
  end
end
