require File.expand_path('../../spec_helper', __FILE__)

describe "An Ensure node" do
  relates <<-ruby do
      begin
        # do nothing
      rescue
        # do nothing
      ensure
        # do nothing
      end
    ruby

    compile do |g|
      top    = g.new_label
      dunno  = g.new_label
      bottom = g.new_label

      g.setup_unwind dunno

      top.set!

      g.save_exception

      g.push_modifiers
      g.push :nil
      g.pop_modifiers
      g.pop_unwind
      g.goto bottom

      dunno.set!

      g.push_exception_state

      g.push :nil
      g.pop

      g.restore_exception_state
      g.reraise

      bottom.set!

      g.push :nil
      g.pop
    end
  end

  relates <<-ruby do
      begin
        (1 + 1)
      rescue SyntaxError => e1
        2
      rescue Exception => e2
        3
      else
        4
      ensure
        5
      end
    ruby

    compile do |g|
      g.for_ensure do |eb|
        eb.body do
          g.for_rescue do |rb|
            rb.body do
              g.push 1
              g.push 1
              g.send :+, 1, false
            end

            rb.condition :SyntaxError do
              g.push_current_exception
              g.set_local 0
              g.pop
              g.push 2
            end

            rb.condition :Exception do
              g.push_current_exception
              g.set_local 1
              g.pop
              g.push 3
            end

            rb.els do
              g.pop         # TODO: should this be built in?
              g.push 4
            end
          end
        end

        eb.handler do
          g.push 5
          g.pop
        end
      end
    end
  end

  relates <<-ruby do
      begin
        a
      rescue
        # do nothing
      ensure
        # do nothing
      end
    ruby

    compile do |g|
      g.for_ensure do |eb|
        eb.body do
          g.for_rescue do |rb|
            rb.body do
              g.push :self
              g.send :a, 0, true
            end

            rb.condition :StandardError do
              g.push :nil
            end
          end
        end

        eb.handler do
          g.push :nil
          g.pop
        end
      end
    end
  end

  relates <<-ruby do
      begin
        14
        return 2
      ensure
        13
      end
    ruby

    compile do |g|
      top = g.new_label
      exc_lbl = g.new_label
      noexc_lbl = g.new_label

      g.setup_unwind exc_lbl

      top.set!

      saved = save_exception()

      g.push 14
      g.pop
      g.push 2
      g.ensure_return

      g.pop_unwind
      g.goto noexc_lbl

      exc_lbl.set!
      g.push_exception_state

      g.push 13
      g.pop

      g.restore_exception_state
      g.reraise

      noexc_lbl.set!

      g.push 13
      g.pop
    end
  end

  relates <<-ruby do
      begin
        begin
          14
          return 2
        ensure
          13
        end
      ensure
        15
      end
    ruby

    # TODO
  end

  relates <<-ruby do
      begin
        14
        return 2
      ensure
        begin
          15
          return 3
        ensure
          16
        end
      end
    ruby

  # TODO
  end
end
