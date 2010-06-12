require File.expand_path('../../../spec_helper', __FILE__)

describe "A Call node using SendLoop transform" do
  relates <<-ruby do
      loop
    ruby

    compile :loop do |g|
      g.push :self
      g.send :loop, 0, true
    end
  end

  relates <<-ruby do
      a = 1
      loop do
        a += 1
        break if a > 10
      end
    ruby

    compile :loop do |g|
      top = g.new_label
      f = g.new_label
      nxt = g.new_label
      done = g.new_label

      g.push 1
      g.set_local 0
      g.pop

      g.push_modifiers
      top.set!

      g.push_local 0
      g.push 1
      g.send :+, 1, false
      g.set_local 0
      g.pop
      g.push_local 0
      g.push 10
      g.send :>, 1, false
      g.gif f
      g.push :nil
      g.goto done
      g.goto nxt

      f.set!
      g.push :nil

      nxt.set!
      g.pop
      g.check_interrupts
      g.goto top

      done.set!
      g.pop_modifiers
    end
  end

  relates <<-ruby do
      def f(&block)
        loop(&block)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        body = d.new_label
        call = d.new_label

        d.push_proc
        d.set_local 0
        d.pop
        d.push :self
        d.push_local 0
        d.dup
        d.is_nil
        d.git call
        d.push_cpath_top
        d.find_const :Proc
        d.swap
        d.send :__from_block__, 1

        call.set!
        d.send_with_block :loop, 0, true
      end
    end
  end
end
