#
# Totally minimal (hopefully) drop-in replacement for test/unit
#

# AK: historically from miniunit afaiu, some minor changes made by myself

# TODO: document minimal core methods needed for this to work

module Test
  class Assertion < Exception; attr_accessor :line; end

  class Unit
    def self.autotest
      Test::Unit::TestCase::Tests.each do |klass|
        inst = klass.new
        klass.instance_methods(false).each do |meth|
          next unless meth.to_s =~ /^test/
          begin
            inst.setup
            inst.send meth.intern
            inst.teardown
            puts "Test Passed"
          rescue Exception => e
            puts "Test failed"
            break if ENV["SILENCE_TESTS"]
            file, line = file_and_line_number(e)
            code = file ? load_line(file, line) : nil
            display_result(klass, meth, e, code)
          end
        end
      end
      puts "Autotest finished"
    end

    def self.file_and_line_number(exc)
      # './shotgun-tests/test_core.rb:438:in `test_splat'''
      msg = exc.backtrace.detect {|m| m =~ /in `test/}
      msg.split(':', 2) rescue [nil,nil]
    end

    def self.display_result(klass, meth, e, code)
      print "\n", (Test::Assertion === e ? "Failure: " : "Error: ")
      if code
      puts "\n#{klass}.#{meth}: #{e}\n      => #{code}\n"
      else
      puts "\n#{klass}.#{meth}: #{e}\n   " 
      end
      puts e.backtrace
    end

    def self.load_line(file, line)
      line_no = line.to_i

      return nil unless File.exists?(file)
      return nil if line_no <= 0
      code = nil
      File.open(file) do |f|
        line = f.gets while f.lineno < line_no
        code = line.strip
      end
      code
    end

    class TestCase

      Tests = []

      def self.inherited(sub)
        Tests << sub
      end

      def setup; end
      def teardown; end

      def assert(test, msg="failed assertion (no message given)")
        unless test
          exc = Test::Assertion.new
          exc.line = caller[0].split(':')[1]
          raise exc
        end
      end

      def assert_equal(exp, act, msg=nil)
        assert exp == act, msg || "Expected #{act.inspect} to be equal to #{exp.inspect}"
      end

      def assert_in_delta(exp, act, delta, msg=nil)
        assert((exp.to_f - act.to_f).abs <= delta.to_f, msg || "Expected #{exp} to be within #{delta} of #{act}")
      end

      def assert_instance_of(cls, obj, msg=nil)
        assert obj.instance_of?(cls), msg || "Expected #{obj} to be a #{cls}"
      end

      def assert_kind_of(cls, obj, msg=nil)
        assert obj.kind_of?(cls), msg || "Expected #{obj.inspect} to be a kind of #{cls}"
      end

      def assert_match(exp, act, msg=nil)
        assert act =~ exp, msg || "Expected #{act.inspect} to match #{exp.inspect}"
      end

      def assert_nil(obj, msg=nil)
        assert obj.nil?, msg || "Expected #{obj.inspect} to be nil"
      end

      def assert_not_equal(exp, act, msg=nil)
        assert exp != act, msg || "Expected #{act.inspect} to not be equal to #{exp.inspect}"
      end

      def assert_not_nil(obj, msg=nil)
        assert ! obj.nil?, msg || "Expected #{obj.inspect} to not be nil"
      end

      def assert_not_same(exp, act, msg=nil)
        assert ! exp.equal?(act), msg || "Expected #{act.inspect} to not be the same as #{exp.inspect}"
      end

      def assert_raises(exp, msg=nil)
        begin
          yield
          assert false, "Expected #{exp} to be raised"
        rescue Exception => e
          assert exp === e, msg || "Expected #{exp} to be raised, but got #{e.class}"
          return e
        end
      end
      alias :assert_raise :assert_raises

      def assert_same(exp, act, msg=nil)
        assert exp.equal?(act), msg || "Expected #{act.inspect} to be the same as #{exp.inspect}"
      end

      def assert_operator(o1, op, o2, msg="")
        assert o1.__send__(op, o2), msg || "Expected #{o1}.#{op}(#{o2}) to be true"
      end

      def assert_nothing_raised; yield; end

    end # class TestCase
  end # class Unit
end # module Test

at_exit { Test::Unit.autotest }
