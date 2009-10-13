require File.dirname(__FILE__) + '/../../spec_helper'

describe "A Call node using SendLoop transform" do
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
end
