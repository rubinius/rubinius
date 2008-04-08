#
# Totally minimal (hopefully) drop-in replacement for test/unit
#

# NOTE: this list might be slightly out of date - I'm trying to automate...
#
# Minimal methods needed to run full miniunit + assert:
#
# Array: each, grep, join, size, <<, empty?, shift, first, dup
# Class: new, inherited
# Enumerable: find, reject
# Exception: backtrace, message
# File: dirname
# Fixnum: +, ==
# Hash: keys
# Kernel: at_exit
# Module: ===, public_instance_methods
# NilClass: ==
# Object: __send__, class, inspect
# String: sub, %, index, ===
# Regexp: =~, new, ===

# Only needed for specific assertions:
#
# Array.last
# Enumerable.include?
# Float: abs
# Object: ==, inspect, equal?, kind_of?, nil?, respond_to?
# Numeric: to_f, <, -

$TESTING_MINIUNIT ||= false

at_exit {
  exit_code = MiniTest::Unit.new.run(ARGV)
  exit exit_code if exit_code
} unless $TESTING_MINIUNIT

module MiniTest
  class Assertion < Exception; end

  file = if __FILE__ =~ /^[^\.]/ then # OMG ruby 1.9 is so lame (rubinius too)
           require 'pathname'
           pwd = Pathname.new(Dir.pwd)
           pn = Pathname.new(File.expand_path(__FILE__))
           pn = File.join(".", pn.relative_path_from(pwd)) unless pn.relative?
           pn.to_s
         else
           __FILE__
         end

  MINIUNIT_DIR = File.dirname(File.dirname(file))

  ##
  # Filters a backtrace to include only relevant calls.

  def self.filter_backtrace(bt)
    return ["No backtrace"] unless bt

    new_bt = []
    bt.each do |line|
      break if line.index(MINIUNIT_DIR) == 0
      new_bt << line
    end

    new_bt = bt.reject { |line| line.index(MINIUNIT_DIR) == 0 } if
      new_bt.empty?
    new_bt = bt.dup if new_bt.empty?

    new_bt
  end

  class Unit
    VERSION = "1.1.0"

    attr_reader :report

    @@out = $stdout

    def self.output= stream
      @@out = stream
    end

    def puke(klass, meth, e)
      if MiniTest::Assertion === e then
        @results[:failures] += 1

        loc = e.backtrace.find { |s| s !~ /in .(assert|flunk|raise)/ }
        loc.sub!(/:in .*$/, '')

        @report << "Failure:\n#{meth}(#{klass}) [#{loc}]:\n#{e.message}\n"
        'F'
      else
        @results[:errors] += 1
        bt = MiniTest::filter_backtrace(e.backtrace).join("\n    ")
        e = "Error:\n#{meth}(#{klass}):\n#{e.class}: #{e.message}\n    #{bt}\n"
        @report << e
        'E'
      end
    end

    def initialize
      @report = []
      @results = {:errors => 0, :failures => 0}
    end

    ##
    # Top level driver, controls all output and filtering.

    def run args
      filter = if args.first =~ /^(-n|--name)$/ then
                 args.shift
                 arg = args.shift
                 arg =~ /\/(.*)\// ? Regexp.new($1) : arg
               else
                 /./ # anything - ^test_ already filtered by #tests
               end

      @@out.puts "Loaded suite #{$0.sub(/\.rb$/, '')}\nStarted"

      start = Time.now
      run_test_suites filter

      @@out.puts
      @@out.puts "Finished in #{'%.6f' % (Time.now - start)} seconds."

      @report.each_with_index do |msg, i|
        @@out.puts "\n%3d) %s" % [i + 1, msg]
      end

      @@out.puts

      format = "%d tests, %d assertions, %d failures, %d errors"
      @@out.puts format % [@test_count, @assertion_count, failures, errors]

      return failures + errors if @test_count > 0 # or return nil...
    end

    def run_test_suites filter = /^test/
      @test_count, @assertion_count = 0, 0
      TestCase.test_suites.each do |suite|
        inst = suite.new
        inst._assertions = 0
        suite.tests.grep(filter).each do |test|
          @@out.puts "\n#{test}: " if $DEBUG
          result = '.'
          begin
            inst.setup
            inst.__send__ test
          rescue Exception => e
            result = puke(suite, test, e)
          ensure
            begin
              inst.teardown
            rescue Exception => e
              result = puke(suite, test, e)
            end
          end
          @@out.print result
          @@out.puts if $DEBUG
          @test_count += 1
        end
        @assertion_count += inst._assertions
      end
      [@test_count, @assertion_count]
    end

    def failures
      @results[:failures]
    end

    def errors
      @results[:errors]
    end

    class TestCase
      def self.reset
        @@test_suites = {}
      end

      reset

      def self.inherited klass
        @@test_suites[klass] = true
      end

      def self.test_suites
        @@test_suites.keys.sort_by { |ts| ts.name }
      end

      def self.tests
        public_instance_methods(true).grep(/^test/).sort.map { |m|
          m.to_s
        }
      end

      attr_accessor :_assertions
      def initialize
        self._assertions = 0
      end

      def setup; end
      def teardown; end

      def assert test, msg = "failed assertion (no message given)."
        self._assertions += 1
        raise MiniTest::Assertion, msg unless test
      end

      def deny test, msg = "failed deny (no message given)."
        assert !test, msg
      end

      def assert_block msg = "assert_block failed."
        assert yield, msg
      end

      def assert_equal exp, act, msg = ""
        msg += '.' unless msg.empty?
        assert exp == act, "#{msg}\n<#{exp.inspect}> expected, not\n<#{act.inspect}>.".strip
      end

      def assert_in_delta(exp, act, delta,
                          msg="Expected #{exp} to be within #{delta} of #{act}")
        assert delta.to_f > (exp.to_f - act.to_f).abs, msg
      end

      def assert_instance_of cls, obj, msg = "Expected #{obj.inspect} to be an instance of #{cls}"
        assert cls === obj, msg
      end

      def assert_kind_of cls, obj, msg = "Expected #{obj.inspect} to be a kind of #{cls}"
        assert obj.kind_of?(cls), msg
      end

      def assert_match exp, act, msg = "Expected #{act.inspect} to match #{exp.inspect}"
        assert act =~ exp, msg
      end

      def assert_nil obj, msg = "Expected #{obj.inspect} to be nil"
        assert obj.nil?, msg
      end

      def assert_no_match exp, act, msg = "Expected #{act.inspect} to not match #{exp.inspect}"
        assert act !~ exp, msg
      end

      def assert_not_equal exp, act, msg = "Expected #{act.inspect} to not be equal to #{exp.inspect}"
        assert exp != act, msg
       end

      def assert_not_nil obj, msg = "Expected #{obj.inspect} to not be nil"
        assert ! obj.nil?, msg
      end

      def assert_not_same exp, act, msg = "Expected #{act.inspect} to not be the same as #{exp.inspect}"
        assert ! exp.equal?(act), msg
      end

      def assert_nothing_raised _ = :ignored
        self._assertions += 1
        yield
      rescue => e
        raise MiniTest::Assertion, exception_details(e, "Exception raised:")
      end

      alias :assert_nothing_thrown :assert_nothing_raised

      def assert_operator o1, op, o2, msg = "<#{o1.inspect}> expected to be\n#{op.inspect}\n<#{o2.inspect}>."
        assert o1.__send__(op, o2), msg
      end

      def exception_details e, msg
        "#{msg}\nClass: <#{e.class}>\nMessage: <#{e.message.inspect}>\n---Backtrace---\n#{MiniTest::filter_backtrace(e.backtrace).join("\n")}\n---------------"
      end

      def assert_raises *exp
        msg = String === exp.last ? exp.pop : nil
        exp = exp.first if exp.size == 1
        begin
          yield
          raise MiniTest::Assertion, "<#{exp.inspect}> exception expected but none was thrown."
        rescue Exception => e
          assert((Array === exp ? exp.include?(e) : exp === e),
                 exception_details(e,
                                   "<#{exp.inspect}> exception expected, not"))
          return e
        end
      end
      alias :assert_raise :assert_raises

      def assert_respond_to obj, meth, msg = "Expected #{obj.inspect} of type #{obj.class} to respond_to? #{meth.inspect}"
        assert obj.respond_to?(meth), msg
      end

      def assert_same exp, act, msg = "Expected #{act.inspect} to be the same as #{exp.inspect}."
        assert exp.equal?(act), msg
      end

      def assert_send send, msg = nil
        recv, meth, *args = send
        assert(recv.__send__(meth, *args),
               msg || "<#{recv.inspect}> expected to respond to\n<#{meth.inspect}> with a true value.")
      end

      def assert_throws sym, msg = "<#{sym.inspect}> should have been thrown"
        caught = true
        catch(sym) do
          begin
            yield
          rescue ArgumentError => e     # 1.9 exception
            msg += ", not <#{e.message.split(/ /).last}>"
          rescue NameError => e         # 1.8 exception
            msg += ", not <#{e.name.inspect}>"
          end
          caught = false
        end
        assert caught, msg + "."
      end

      def flunk msg = "Flunked"
        assert false, msg
      end
    end # class TestCase
  end # class Unit
end # module MiniTest

Test = MiniTest unless defined? Test
