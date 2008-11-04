############################################################
# This file is imported from a different project.
# DO NOT make modifications in this repo.
# File a patch instead and assign it to Ryan Davis
############################################################

$TESTING = true

require 'test/unit/testcase'
require 'sexp_processor' # for deep_clone

# key:
# wwtt = what were they thinking?

# TODO: <ko1_> 1.8.7 support {|&b|} syntax

class Examples
  attr_reader :reader
  attr_writer :writer

  def a_method(x); x+1; end
  alias an_alias a_method

  define_method(:bmethod_noargs) do
    x + 1
  end

  define_method(:unsplatted) do |x|
    x + 1
  end

  define_method :splatted do |*args|
    y = args.first
    y + 42
  end

  define_method :dmethod_added, instance_method(:a_method) if
    RUBY_VERSION < "1.9"
end

class ParseTreeTestCase < Test::Unit::TestCase
  unless defined? Mini then
    undef_method :default_test rescue nil
    alias :refute_nil :assert_not_nil
  end

  attr_accessor :processor # to be defined by subclass

  def setup
    super
    @processor = nil
  end

  def after_process_hook klass, node, data, input_name, output_name
  end

  def before_process_hook klass, node, data, input_name, output_name
  end

  def self.add_test name, data, klass = self.name[4..-1]
    name = name.to_s
    klass = klass.to_s

    if testcases.has_key? name then
      if testcases[name].has_key? klass then
        warn "testcase #{klass}##{name} already has data"
      else
        testcases[name][klass] = data
      end
    else
      warn "testcase #{name} does not exist"
    end
  end

  def self.add_tests name, hash
    name = name.to_s

    hash.each do |klass, data|
      warn "testcase #{klass}##{name} already has data" if
        testcases[name].has_key? klass
      testcases[name][klass] = data
    end
  end

  def self.clone_same
    @@testcases.each do |node, data|
      data.each do |key, val|
        if val == :same then
          prev_key = self.previous(key)
          data[key] = data[prev_key].deep_clone
        end
      end
    end
  end

  def self.copy_test_case nonverbose, klass
    verbose = nonverbose + "_mri_verbose_flag"
    testcases[verbose][klass] = testcases[nonverbose][klass]
  end

  def self.generate_test klass, node, data, input_name, output_name
    klass.send(:define_method, "test_#{node}".to_sym) do
      flunk "Processor is nil" if processor.nil?

      assert data.has_key?(input_name), "Unknown input data"
      assert data.has_key?(output_name), "Missing test data"

      $missing[node] << output_name unless data.has_key? output_name

      input    = data[input_name].deep_clone
      expected = data[output_name].deep_clone

      case expected
      when :unsupported then
        assert_raises(UnsupportedNodeError) do
          processor.process(input)
        end
      else
        extra_expected = []
        extra_input = []

        _, expected, extra_expected = *expected if
          Array === expected and expected.first == :defx
        _, input, extra_input = *input if
          Array === input and input.first == :defx

        # OMG... I can't believe I have to do this this way.  these
        # hooks are here instead of refactoring this define_method
        # body into an assertion. It'll allow subclasses to hook in
        # and add behavior before or after the processor does it's
        # thing. If you go the body refactor route, some of the
        # RawParseTree test casese fail for completely bogus reasons.

        before_process_hook klass, node, data, input_name, output_name
        refute_nil data[input_name], "testcase does not exist?"
        @result = processor.process input
        assert_equal(expected, @result,
                     "failed on input: #{data[input_name].inspect}")
        after_process_hook klass, node, data, input_name, output_name

        extra_input.each do |extra|
          processor.process(extra)
        end
        extra = processor.extra_methods rescue []
        assert_equal extra_expected, extra
      end
    end
  end

  def self.generate_tests klass
    install_missing_reporter
    clone_same

    output_name = klass.name.to_s.sub(/^Test/, '')

    input_name = self.previous(output_name)

    @@testcases.each do |node, data|
      next if [:skip, :unsupported].include? data[input_name]
      next if data[output_name] == :skip

      generate_test klass, node, data, input_name, output_name
    end
  end

  def self.inherited klass
    super

    generate_tests klass unless klass.name =~ /TestCase/
  end

  def self.install_missing_reporter
    unless defined? $missing then
      $missing = Hash.new { |h,k| h[k] = [] }
      at_exit {
        at_exit {
          warn ""
          $missing.sort.each do |name, klasses|
            warn "add_tests(#{name.inspect},"
            klasses.map! { |klass| "          #{klass.inspect} => :same" }
            warn klasses.join("\n") + ")"
          end
          warn ""
        }
      }
    end
  end

  def self.previous(key, extra=0) # FIX: remove R2C code
    idx = @@testcase_order.index(key)

    raise "Unknown class #{key} in @@testcase_order" if idx.nil?

    case key
    when "RubyToRubyC" then
      idx -= 1
    end
    @@testcase_order[idx - 1 - extra]
  end

  def self.testcase_order; @@testcase_order; end
  def self.testcases; @@testcases; end

  def self.unsupported_tests *tests
    tests.flatten.each do |name|
      add_test name, :unsupported
    end
  end

  ############################################################
  # Shared TestCases:

  @@testcase_order = %w(Ruby RawParseTree ParseTree)

  @@testcases = Hash.new { |h,k| h[k] = {} }

  add_tests("alias",
            "Ruby"         => "class X\n  alias :y :x\nend",
            "RawParseTree" => [:class, :X, nil,
                               [:scope, [:alias, [:lit, :y], [:lit, :x]]]],
            "ParseTree"    => s(:class, :X, nil,
                                s(:scope, s(:alias, s(:lit, :y), s(:lit, :x)))),
            "Ruby2Ruby"    => "class X\n  alias_method :y, :x\nend")

  add_tests("alias_ugh",
            "Ruby"         => "class X\n  alias y x\nend",
            "RawParseTree" => [:class, :X, nil,
                               [:scope, [:alias, [:lit, :y], [:lit, :x]]]],
            "ParseTree"    => s(:class, :X, nil,
                                s(:scope, s(:alias, s(:lit, :y), s(:lit, :x)))),
            "Ruby2Ruby"    => "class X\n  alias_method :y, :x\nend")

  add_tests("and",
            "Ruby"         => "(a and b)",
            "RawParseTree" => [:and, [:vcall, :a], [:vcall, :b]],
            "ParseTree"    => s(:and,
                                s(:call, nil, :a, s(:arglist)),
                                s(:call, nil, :b, s(:arglist))))

  add_tests("argscat_inside",
            "Ruby"         => "a = [b, *c]",
            "RawParseTree" => [:lasgn, :a,
                               [:argscat,
                                [:array, [:vcall, :b]], [:vcall, :c]]],
            "ParseTree"    => s(:lasgn, :a,
                                s(:array,
                                  s(:call, nil, :b, s(:arglist)),
                                  s(:splat, s(:call, nil, :c, s(:arglist))))))

  add_tests("argscat_svalue",
            "Ruby"         => "a = b, c, *d",
            "RawParseTree" => [:lasgn, :a,
                               [:svalue,
                                [:argscat,
                                 [:array, [:vcall, :b], [:vcall, :c]],
                                 [:vcall, :d]]]],
            "ParseTree"    => s(:lasgn, :a,
                                s(:svalue,
                                  s(:array,
                                    s(:call, nil, :b, s(:arglist)),
                                    s(:call, nil, :c, s(:arglist)),
                                    s(:splat,
                                      s(:call, nil, :d, s(:arglist)))))))

  add_tests("argspush",
            "Ruby"         => "a[*b] = c",
            "RawParseTree" => [:attrasgn,
                               [:vcall, :a],
                               :[]=,
                               [:argspush,
                                [:splat,
                                 [:vcall, :b]],
                                [:vcall, :c]]],
            "ParseTree"    => s(:attrasgn,
                                s(:call, nil, :a, s(:arglist)),
                                :[]=,
                                s(:arglist,
                                  s(:splat,
                                    s(:call, nil, :b, s(:arglist))),
                                  s(:call, nil, :c, s(:arglist)))))

  add_tests("array",
            "Ruby"         => "[1, :b, \"c\"]",
            "RawParseTree" => [:array, [:lit, 1], [:lit, :b], [:str, "c"]],
            "ParseTree"    => s(:array, s(:lit, 1), s(:lit, :b), s(:str, "c")))

  add_tests("array_pct_W",
            "Ruby"         => "%W[a b c]",
            "RawParseTree" => [:array, [:str, "a"], [:str, "b"], [:str, "c"]],
            "ParseTree"    => s(:array,
                                s(:str, "a"), s(:str, "b"), s(:str, "c")),
            "Ruby2Ruby"    => "[\"a\", \"b\", \"c\"]")

  add_tests("array_pct_W_dstr",
            "Ruby"         => "%W[a #\{@b} c]",
            "RawParseTree" => [:array,
                               [:str, "a"],
                               [:dstr, "", [:evstr, [:ivar, :@b]]],
                               [:str, "c"]],
            "ParseTree"    => s(:array,
                                s(:str, "a"),
                                s(:dstr, "", s(:evstr, s(:ivar, :@b))),
                                s(:str, "c")),
            "Ruby2Ruby"    => "[\"a\", \"#\{@b}\", \"c\"]")

  add_tests("array_pct_w",
            "Ruby"         => "%w[a b c]",
            "RawParseTree" => [:array, [:str, "a"], [:str, "b"], [:str, "c"]],
            "ParseTree"    => s(:array,
                                s(:str, "a"), s(:str, "b"), s(:str, "c")),
            "Ruby2Ruby"    => "[\"a\", \"b\", \"c\"]")

  add_tests("array_pct_w_dstr",
            "Ruby"         => "%w[a #\{@b} c]",
            "RawParseTree" => [:array,
                               [:str, "a"],
                               [:str, "#\{@b}"],
                               [:str, "c"]],
            "ParseTree"    => s(:array,
                                s(:str, "a"),
                                s(:str, "#\{@b}"),
                                s(:str, "c")),
            "Ruby2Ruby"    => "[\"a\", \"\\\#{@b}\", \"c\"]") # TODO: huh?

  add_tests("attrasgn",
            "Ruby"         => "y = 0\n42.method = y\n",
            "RawParseTree" => [:block,
                               [:lasgn, :y, [:lit, 0]],
                               [:attrasgn, [:lit, 42], :method=,
                                [:array, [:lvar, :y]]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :y, s(:lit, 0)),
                                s(:attrasgn, s(:lit, 42), :method=,
                                  s(:arglist, s(:lvar, :y)))))

  add_tests("attrasgn_index_equals",
            "Ruby"         => "a[42] = 24",
            "RawParseTree" => [:attrasgn, [:vcall, :a], :[]=,
                               [:array, [:lit, 42], [:lit, 24]]],
            "ParseTree"    => s(:attrasgn,
                                s(:call, nil, :a, s(:arglist)),
                                :[]=,
                                s(:arglist, s(:lit, 42), s(:lit, 24))))

  add_tests("attrasgn_index_equals_space",
            "Ruby"         => "a = []; a [42] = 24",
            "RawParseTree" => [:block,
                               [:lasgn, :a, [:zarray]],
                               [:attrasgn, [:lvar, :a], :[]=,
                                [:array, [:lit, 42], [:lit, 24]]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :a, s(:array)),
                                s(:attrasgn, s(:lvar, :a), :[]=,
                                  s(:arglist, s(:lit, 42), s(:lit, 24)))),
            "Ruby2Ruby"    => "a = []\na[42] = 24\n")

  add_tests("attrset",
            "Ruby"         => [Examples, :writer=],
            "RawParseTree" => [:defn, :writer=, [:attrset, :@writer]],
            "ParseTree"    => s(:defn, :writer=,
                                s(:args, :arg),
                                s(:attrset, :@writer)),
            "Ruby2Ruby"    => "attr_writer :writer")

  add_tests("back_ref",
            "Ruby"         => "[$&, $`, $', $+]",
            "RawParseTree" => [:array,
                               [:back_ref, :&],
                               [:back_ref, :"`"],
                               [:back_ref, :"'"],
                               [:back_ref, :+]],
            "ParseTree"    => s(:array,
                                s(:back_ref, :&),
                                s(:back_ref, :"`"),
                                s(:back_ref, :"'"),
                                s(:back_ref, :+)))

  add_tests("begin",
            "Ruby"         => "begin\n  (1 + 1)\nend",
            "RawParseTree" => [:begin,
                               [:call, [:lit, 1], :+, [:array, [:lit, 1]]]],
            "ParseTree"    => s(:call, s(:lit, 1), :+,
                                s(:arglist, s(:lit, 1))),
            "Ruby2Ruby"    => "(1 + 1)")

  add_tests("begin_def",
            "Ruby"         => "def m\n  begin\n\n  end\nend",
            "RawParseTree" => [:defn, :m, [:scope, [:block, [:args], [:nil]]]],
            "ParseTree"    => s(:defn, :m, s(:args),
                                s(:scope, s(:block, s(:nil)))),
            "Ruby2Ruby"    => "def m\n  # do nothing\nend")

  add_tests("begin_rescue_ensure",
            "Ruby"         => "begin\n  a\nrescue\n  # do nothing\nensure\n  # do nothing\nend",
            "RawParseTree" => [:begin,
                               [:ensure,
                                [:rescue,
                                 [:vcall, :a],
                                 [:resbody, nil]],
                                [:nil]]],
            "ParseTree"    => s(:ensure,
                                s(:rescue,
                                  s(:call, nil, :a, s(:arglist)),
                                  s(:resbody, s(:array), nil)),
                                s(:nil)))

  add_tests("begin_rescue_ensure_all_empty",
            "Ruby"         => "begin\n  # do nothing\nrescue\n  # do nothing\nensure\n  # do nothing\nend",
            "RawParseTree" => [:begin,
                               [:ensure,
                                [:rescue,
                                 [:resbody, nil]],
                                [:nil]]],
            "ParseTree"    => s(:ensure,
                                s(:rescue,
                                  s(:resbody, s(:array), nil)),
                                s(:nil)))

  add_tests("begin_rescue_twice",
            "Ruby"         => "begin\n  a\nrescue => mes\n  # do nothing\nend\nbegin\n  b\nrescue => mes\n  # do nothing\nend\n",
            "RawParseTree" => [:block,
                               [:begin,
                                [:rescue,
                                 [:vcall, :a],
                                 [:resbody, nil,
                                  [:lasgn, :mes, [:gvar, :$!]]]]],
                               [:begin,
                                [:rescue,
                                 [:vcall, :b],
                                 [:resbody, nil,
                                  [:lasgn, :mes, [:gvar, :$!]]]]]],
            "ParseTree"    => s(:block,
                                s(:rescue,
                                  s(:call, nil, :a, s(:arglist)),
                                  s(:resbody,
                                    s(:array, s(:lasgn, :mes, s(:gvar, :$!))),
                                    nil)),
                                s(:rescue,
                                  s(:call, nil, :b, s(:arglist)),
                                  s(:resbody,
                                    s(:array,
                                      s(:lasgn, :mes, s(:gvar, :$!))),
                                    nil))))

  add_tests("begin_rescue_twice_mri_verbose_flag",
            "RawParseTree" => [:block,
                               [:rescue,                # no begin
                                [:vcall, :a],
                                [:resbody, nil,
                                 [:lasgn, :mes, [:gvar, :$!]]]],
                               [:begin,
                                [:rescue,
                                 [:vcall, :b],
                                 [:resbody, nil,
                                  [:lasgn, :mes, [:gvar, :$!]]]]]])

  copy_test_case "begin_rescue_twice", "Ruby"
  copy_test_case "begin_rescue_twice", "ParseTree"

  add_tests("block_attrasgn",
            "Ruby" => "def self.setup(ctx)\n  bind = allocate\n  bind.context = ctx\n  return bind\nend",
            "RawParseTree" => [:defs, [:self], :setup,
                               [:scope,
                                [:block,
                                 [:args, :ctx],
                                 [:lasgn, :bind, [:vcall, :allocate]],
                                 [:attrasgn, [:lvar, :bind], :context=,
                                  [:array, [:lvar, :ctx]]],
                                 [:return, [:lvar, :bind]]]]],
            "ParseTree" => s(:defs, s(:self), :setup,
                             s(:args, :ctx),
                             s(:scope,
                               s(:block,
                                 s(:lasgn, :bind,
                                   s(:call, nil, :allocate, s(:arglist))),
                                 s(:attrasgn, s(:lvar, :bind), :context=,
                                   s(:arglist, s(:lvar, :ctx))),
                                 s(:return, s(:lvar, :bind))))))


  add_tests("block_lasgn",
            "Ruby"         => "x = (y = 1\n(y + 2))",
            "RawParseTree" => [:lasgn, :x,
                               [:block,
                                [:lasgn, :y, [:lit, 1]],
                                [:call, [:lvar, :y], :+, [:array, [:lit, 2]]]]],
            "ParseTree"    => s(:lasgn, :x,
                                s(:block,
                                  s(:lasgn, :y, s(:lit, 1)),
                                  s(:call, s(:lvar, :y), :+,
                                    s(:arglist, s(:lit, 2))))))

  add_tests("block_mystery_block",
            "Ruby"         => "a(b) do\n  if b then\n    true\n  else\n    c = false\n    d { |x| c = true }\n    c\n  end\nend",
            "RawParseTree" => [:iter,
                               [:fcall, :a, [:array, [:vcall, :b]]],
                               nil,
                               [:if,
                                [:vcall, :b],
                                [:true],
                                [:block,
                                 [:dasgn_curr, :c, [:false]],
                                 [:iter,
                                  [:fcall, :d],
                                  [:dasgn_curr, :x],
                                  [:dasgn, :c, [:true]]],
                                 [:dvar, :c]]]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :a,
                                  s(:arglist, s(:call, nil, :b, s(:arglist)))),
                                nil,
                                s(:if,
                                  s(:call, nil, :b, s(:arglist)),
                                  s(:true),
                                  s(:block,
                                    s(:lasgn, :c, s(:false)),
                                    s(:iter,
                                      s(:call, nil, :d, s(:arglist)),
                                      s(:lasgn, :x),
                                      s(:lasgn, :c, s(:true))),
                                    s(:lvar, :c)))))

  add_tests("block_pass_args_and_splat",
            "Ruby"         => "def blah(*args, &block)\n  other(42, *args, &block)\nend",
            "RawParseTree" => [:defn, :blah,
                               [:scope,
                                [:block,
                                 [:args, :"*args"],
                                 [:block_arg, :block],
                                 [:block_pass,
                                  [:lvar, :block],
                                  [:fcall, :other,
                                   [:argscat,
                                    [:array, [:lit, 42]], [:lvar, :args]]]]]]],
            "ParseTree"    => s(:defn, :blah,
                                s(:args, :"*args", :"&block"),
                                s(:scope,
                                  s(:block,
                                    s(:call, nil, :other,
                                      s(:arglist,
                                        s(:lit, 42),
                                        s(:splat, s(:lvar, :args)),
                                        s(:block_pass, s(:lvar, :block))))))))

  add_tests("block_pass_call_0",
            "Ruby"         => "a.b(&c)",
            "RawParseTree" => [:block_pass,
                               [:vcall, :c], [:call, [:vcall, :a], :b]],
            "ParseTree"    => s(:call,
                                s(:call, nil, :a, s(:arglist)),
                                :b,
                                s(:arglist,
                                  s(:block_pass,
                                    s(:call, nil, :c, s(:arglist))))))

  add_tests("block_pass_call_1",
            "Ruby"         => "a.b(4, &c)",
            "RawParseTree" => [:block_pass,
                               [:vcall, :c],
                               [:call, [:vcall, :a], :b, [:array, [:lit, 4]]]],
            "ParseTree"    => s(:call,
                                s(:call, nil, :a, s(:arglist)),
                                :b,
                                s(:arglist,
                                  s(:lit, 4),
                                  s(:block_pass,
                                    s(:call, nil, :c, s(:arglist))))))

  add_tests("block_pass_call_n",
            "Ruby"         => "a.b(1, 2, 3, &c)",
            "RawParseTree" => [:block_pass,
                               [:vcall, :c],
                               [:call, [:vcall, :a], :b,
                                [:array, [:lit, 1], [:lit, 2], [:lit, 3]]]],
            "ParseTree"    => s(:call,
                                s(:call, nil, :a, s(:arglist)),
                                :b,
                                s(:arglist,
                                  s(:lit, 1), s(:lit, 2), s(:lit, 3),
                                  s(:block_pass,
                                    s(:call, nil, :c, s(:arglist))))))

  add_tests("block_pass_fcall_0",
            "Ruby"         => "a(&b)",
            "RawParseTree" => [:block_pass, [:vcall, :b], [:fcall, :a]],
            "ParseTree"    => s(:call, nil, :a,
                                s(:arglist,
                                  s(:block_pass,
                                    s(:call, nil, :b, s(:arglist))))))

  add_tests("block_pass_fcall_1",
            "Ruby"         => "a(4, &b)",
            "RawParseTree" => [:block_pass,
                               [:vcall, :b],
                               [:fcall, :a, [:array, [:lit, 4]]]],
            "ParseTree"    => s(:call, nil, :a,
                                s(:arglist,
                                  s(:lit, 4),
                                  s(:block_pass,
                                    s(:call, nil, :b, s(:arglist))))))

  add_tests("block_pass_fcall_n",
            "Ruby"         => "a(1, 2, 3, &b)",
            "RawParseTree" => [:block_pass,
                               [:vcall, :b],
                               [:fcall, :a,
                                [:array, [:lit, 1], [:lit, 2], [:lit, 3]]]],
            "ParseTree"    => s(:call, nil, :a,
                                s(:arglist,
                                  s(:lit, 1), s(:lit, 2), s(:lit, 3),
                                  s(:block_pass,
                                    s(:call, nil, :b, s(:arglist))))))

  add_tests("block_pass_omgwtf",
            "Ruby"         => "define_attr_method(:x, :sequence_name, &Proc.new { |*args| nil })",
            "RawParseTree" => [:block_pass,
                               [:iter,
                                [:call, [:const, :Proc], :new],
                                [:masgn, nil, [:dasgn_curr, :args], nil],
                                [:nil]],
                               [:fcall, :define_attr_method,
                                [:array, [:lit, :x], [:lit, :sequence_name]]]],
            "ParseTree"    => s(:call, nil, :define_attr_method,
                                s(:arglist,
                                  s(:lit, :x),
                                  s(:lit, :sequence_name),
                                  s(:block_pass,
                                    s(:iter,
                                      s(:call, s(:const, :Proc), :new,
                                        s(:arglist)),
                                      s(:masgn,
                                        s(:array, s(:splat, s(:lasgn, :args)))),
                                      s(:nil))))))

  add_tests("block_pass_splat",
            "Ruby"         => "def blah(*args, &block)\n  other(*args, &block)\nend",
            "RawParseTree" => [:defn, :blah,
                               [:scope,
                                [:block,
                                 [:args, :"*args"],
                                 [:block_arg, :block],
                                 [:block_pass,
                                  [:lvar, :block],
                                  [:fcall, :other,
                                   [:splat, [:lvar, :args]]]]]]],
            "ParseTree"    => s(:defn, :blah,
                                s(:args, :"*args", :"&block"),
                                s(:scope,
                                  s(:block,
                                    s(:call, nil, :other,
                                      s(:arglist,
                                        s(:splat, s(:lvar, :args)),
                                        s(:block_pass, s(:lvar, :block))))))))

  add_tests("block_pass_thingy",
            "Ruby"         => "r.read_body(dest, &block)",
            "RawParseTree" => [:block_pass,
                               [:vcall, :block],
                               [:call, [:vcall, :r], :read_body,
                                [:array, [:vcall, :dest]]]],
            "ParseTree"    => s(:call,
                                s(:call, nil, :r, s(:arglist)),
                                :read_body,
                                s(:arglist,
                                  s(:call, nil, :dest, s(:arglist)),
                                  s(:block_pass,
                                    s(:call, nil, :block, s(:arglist))))))

  add_tests("block_stmt_after",
            "Ruby"         => "def f\n  begin\n    b\n  rescue\n    c\n  end\n\n  d\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                                 [:args],
                                 [:begin,
                                  [:rescue,
                                   [:vcall, :b],
                                   [:resbody, nil, [:vcall, :c]]]],
                                 [:vcall, :d]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args),
                                s(:scope,
                                  s(:block,
                                    s(:rescue,
                                      s(:call, nil, :b, s(:arglist)),
                                      s(:resbody,
                                        s(:array),
                                        s(:call, nil, :c, s(:arglist)))),
                                    s(:call, nil, :d, s(:arglist))))),
            "Ruby2Ruby"    => "def f\n  b rescue c\n  d\nend")

  add_tests("block_stmt_after_mri_verbose_flag",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                                 [:args],
                                 [:rescue,              # no begin
                                  [:vcall, :b],
                                  [:resbody, nil, [:vcall, :c]]],
                                 [:vcall, :d]]]])

  copy_test_case "block_stmt_after", "Ruby"
  copy_test_case "block_stmt_after", "ParseTree"
  copy_test_case "block_stmt_after", "Ruby2Ruby"

  add_tests("block_stmt_before",
            "Ruby"         => "def f\n  a\n  begin\n    b\n  rescue\n    c\n  end\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                                 [:args],
                                 [:vcall, :a],
                                 [:begin,
                                  [:rescue, [:vcall, :b],
                                   [:resbody, nil, [:vcall, :c]]]]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args),
                                s(:scope,
                                  s(:block,
                                    s(:call, nil, :a, s(:arglist)),
                                    s(:rescue, s(:call, nil, :b, s(:arglist)),
                                      s(:resbody,
                                        s(:array),
                                        s(:call, nil, :c, s(:arglist))))))),
            "Ruby2Ruby"    => "def f\n  a\n  b rescue c\nend")

  # oddly... this one doesn't HAVE any differences when verbose... new?
  copy_test_case "block_stmt_before", "Ruby"
  copy_test_case "block_stmt_before", "ParseTree"
  copy_test_case "block_stmt_before", "RawParseTree"
  copy_test_case "block_stmt_before", "Ruby2Ruby"

  add_tests("block_stmt_both",
            "Ruby"         => "def f\n  a\n  begin\n    b\n  rescue\n    c\n  end\n  d\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                                 [:args],
                                 [:vcall, :a],
                                 [:begin,
                                  [:rescue,
                                   [:vcall, :b],
                                   [:resbody,
                                    nil,
                                    [:vcall, :c]]]],
                                 [:vcall, :d]]]],
            "ParseTree"    => s(:defn, :f, s(:args),
                                s(:scope,
                                  s(:block,
                                    s(:call, nil, :a, s(:arglist)),
                                    s(:rescue,
                                      s(:call, nil, :b, s(:arglist)),
                                      s(:resbody,
                                        s(:array),
                                        s(:call, nil, :c, s(:arglist)))),
                                    s(:call, nil, :d, s(:arglist))))),
            "Ruby2Ruby"    => "def f\n  a\n  b rescue c\n  d\nend")

  add_tests("block_stmt_both_mri_verbose_flag",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                                 [:args],
                                 [:vcall, :a],
                                 [:rescue,              # no begin
                                  [:vcall, :b],
                                  [:resbody,
                                   nil,
                                   [:vcall, :c]]],
                                 [:vcall, :d]]]])

  copy_test_case "block_stmt_both", "Ruby"
  copy_test_case "block_stmt_both", "ParseTree"
  copy_test_case "block_stmt_both", "Ruby2Ruby"

  add_tests("bmethod",
            "Ruby"         => [Examples, :unsplatted],
            "RawParseTree" => [:defn, :unsplatted,
                               [:bmethod,
                                [:dasgn_curr, :x],
                                [:call, [:dvar, :x], :+, [:array, [:lit, 1]]]]],
            "ParseTree"    => s(:defn, :unsplatted,
                                s(:args, :x),
                                s(:scope,
                                  s(:block,
                                    s(:call,
                                      s(:lvar, :x),
                                      :+,
                                      s(:arglist, s(:lit, 1)))))),
            "Ruby2Ruby"    => "def unsplatted(x)\n  (x + 1)\nend")

  add_tests("bmethod_noargs",
            "Ruby"         => [Examples, :bmethod_noargs],
            "RawParseTree" => [:defn, :bmethod_noargs,
                               [:bmethod,
                                nil,
                                [:call,
                                 [:vcall, :x], :"+", [:array, [:lit, 1]]]]],
            "ParseTree"    => s(:defn, :bmethod_noargs,
                                s(:args),
                                s(:scope,
                                  s(:block,
                                    s(:call,
                                      s(:call, nil, :x, s(:arglist)),
                                      :"+",
                                      s(:arglist, s(:lit, 1)))))),
            "Ruby2Ruby"    => "def bmethod_noargs\n  (x + 1)\nend")

  add_tests("bmethod_splat",
            "Ruby"         => [Examples, :splatted],
            "RawParseTree" => [:defn, :splatted,
                               [:bmethod,
                                [:masgn, nil, [:dasgn_curr, :args], nil],
                                [:block,
                                 [:dasgn_curr, :y,
                                  [:call, [:dvar, :args], :first]],
                                 [:call, [:dvar, :y], :+,
                                  [:array, [:lit, 42]]]]]],
            "ParseTree"    => s(:defn, :splatted,
                                s(:args, :"*args"),
                                s(:scope,
                                  s(:block,
                                    s(:lasgn, :y,
                                      s(:call, s(:lvar, :args), :first,
                                        s(:arglist))),
                                    s(:call, s(:lvar, :y), :+,
                                      s(:arglist, s(:lit, 42)))))),
            "Ruby2Ruby"    => "def splatted(*args)\n  y = args.first\n  (y + 42)\nend")

  add_tests("break",
            "Ruby"         => "loop { break if true }",
            "RawParseTree" => [:iter,
                               [:fcall, :loop], nil,
                               [:if, [:true], [:break], nil]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :loop, s(:arglist)), nil,
                                s(:if, s(:true), s(:break), nil)))

  add_tests("break_arg",
            "Ruby"         => "loop { break 42 if true }",
            "RawParseTree" => [:iter,
                               [:fcall, :loop], nil,
                               [:if, [:true], [:break, [:lit, 42]], nil]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :loop, s(:arglist)), nil,
                                s(:if, s(:true), s(:break, s(:lit, 42)), nil)))

  add_tests("call",
            "Ruby"         => "self.method",
            "RawParseTree" => [:call, [:self], :method],
            "ParseTree"    => s(:call, s(:self), :method, s(:arglist)))

  add_tests("call_arglist",
            "Ruby"         => "o.puts(42)",
            "RawParseTree" => [:call, [:vcall, :o], :puts,
                               [:array, [:lit, 42]]],
            "ParseTree"    => s(:call, s(:call, nil, :o, s(:arglist)), :puts,
                                s(:arglist, s(:lit, 42))))

  add_tests("call_arglist_hash",
            "Ruby"         => "o.m(:a => 1, :b => 2)",
            "RawParseTree" => [:call,
                               [:vcall, :o], :m,
                               [:array,
                                [:hash,
                                 [:lit, :a], [:lit, 1],
                                 [:lit, :b], [:lit, 2]]]],
            "ParseTree"    => s(:call,
                                s(:call, nil, :o, s(:arglist)), :m,
                                s(:arglist,
                                  s(:hash,
                                    s(:lit, :a), s(:lit, 1),
                                    s(:lit, :b), s(:lit, 2)))))

  add_tests("call_arglist_norm_hash",
            "Ruby"         => "o.m(42, :a => 1, :b => 2)",
            "RawParseTree" => [:call,
                               [:vcall, :o], :m,
                               [:array,
                                [:lit, 42],
                                [:hash,
                                 [:lit, :a], [:lit, 1],
                                 [:lit, :b], [:lit, 2]]]],
            "ParseTree"    => s(:call,
                                s(:call, nil, :o, s(:arglist)), :m,
                                s(:arglist,
                                  s(:lit, 42),
                                  s(:hash,
                                    s(:lit, :a), s(:lit, 1),
                                    s(:lit, :b), s(:lit, 2)))))

  add_tests("call_arglist_norm_hash_splat",
            "Ruby"         => "o.m(42, :a => 1, :b => 2, *c)",
            "RawParseTree" => [:call,
                               [:vcall, :o], :m,
                               [:argscat,
                                [:array,
                                 [:lit, 42],
                                 [:hash,
                                  [:lit, :a], [:lit, 1],
                                  [:lit, :b], [:lit, 2]]],
                                [:vcall, :c]]],
            "ParseTree"    => s(:call,
                                s(:call, nil, :o, s(:arglist)), :m,
                                s(:arglist,
                                  s(:lit, 42),
                                  s(:hash,
                                    s(:lit, :a), s(:lit, 1),
                                    s(:lit, :b), s(:lit, 2)),
                                  s(:splat, s(:call, nil, :c, s(:arglist))))))

  add_tests("call_arglist_space",
            "Ruby"         => "a (1,2,3)",
            "RawParseTree" => [:fcall, :a,
                               [:array, [:lit, 1], [:lit, 2], [:lit, 3]]],
            "ParseTree"    => s(:call, nil, :a,
                                s(:arglist,
                                  s(:lit, 1), s(:lit, 2), s(:lit, 3))),
            "Ruby2Ruby"    => "a(1, 2, 3)")

  add_tests("call_command",
            "Ruby"         => "1.b(c)",
            "RawParseTree" => [:call, [:lit, 1], :b, [:array, [:vcall, :c]]],
            "ParseTree"    => s(:call,
                                s(:lit, 1),
                                :b,
                                s(:arglist, s(:call, nil, :c, s(:arglist)))))

  add_tests("call_expr",
            "Ruby"         => "(v = (1 + 1)).zero?",
            "RawParseTree" => [:call,
                               [:lasgn, :v,
                                [:call, [:lit, 1], :+, [:array, [:lit, 1]]]],
                               :zero?],
            "ParseTree"    => s(:call,
                                s(:lasgn, :v,
                                  s(:call, s(:lit, 1), :+,
                                    s(:arglist, s(:lit, 1)))),
                                :zero?, s(:arglist)))

  add_tests("call_index",
            "Ruby"         => "a = []\na[42]\n",
            "RawParseTree" => [:block,
                               [:lasgn, :a, [:zarray]],
                               [:call, [:lvar, :a], :[], [:array, [:lit, 42]]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :a, s(:array)),
                                s(:call, s(:lvar, :a), :[],
                                  s(:arglist, s(:lit, 42)))))

  add_tests("call_index_no_args",
            "Ruby"         => "a[]",
            "RawParseTree" => [:call, [:vcall, :a], :[]],
            "ParseTree"    => s(:call, s(:call, nil, :a, s(:arglist)),
                                :[], s(:arglist)))

  add_tests("call_index_space",
            "Ruby"         => "a = []\na [42]\n",
            "RawParseTree" => [:block,
                               [:lasgn, :a, [:zarray]],
                               [:call, [:lvar, :a], :[], [:array, [:lit, 42]]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :a, s(:array)),
                                s(:call, s(:lvar, :a), :[],
                                  s(:arglist, s(:lit, 42)))),
            "Ruby2Ruby"    => "a = []\na[42]\n")

  add_tests("call_unary_neg",
            "Ruby"         => "-2**31",
            "RawParseTree" => [:call,
                               [:call, [:lit, 2], :**, [:array, [:lit, 31]]],
                               :-@],
            "ParseTree"    => s(:call,
                                s(:call,
                                  s(:lit, 2),
                                  :**,
                                  s(:arglist, s(:lit, 31))),
                                :-@, s(:arglist)),
            "Ruby2Ruby"    => "-(2 ** 31)")

  add_tests("case",
            "Ruby"         => "var = 2\nresult = \"\"\ncase var\nwhen 1 then\n  puts(\"something\")\n  result = \"red\"\nwhen 2, 3 then\n  result = \"yellow\"\nwhen 4 then\n  # do nothing\nelse\n  result = \"green\"\nend\ncase result\nwhen \"red\" then\n  var = 1\nwhen \"yellow\" then\n  var = 2\nwhen \"green\" then\n  var = 3\nelse\n  # do nothing\nend\n",
            "RawParseTree" => [:block,
                               [:lasgn, :var, [:lit, 2]],
                               [:lasgn, :result, [:str, ""]],
                               [:case,
                                [:lvar, :var],
                                [:when,
                                 [:array, [:lit, 1]],
                                 [:block,
                                  [:fcall, :puts,
                                   [:array, [:str, "something"]]],
                                  [:lasgn, :result, [:str, "red"]]]],
                                [:when,
                                 [:array, [:lit, 2], [:lit, 3]],
                                 [:lasgn, :result, [:str, "yellow"]]],
                                [:when, [:array, [:lit, 4]], nil],
                                [:lasgn, :result, [:str, "green"]]],
                               [:case,
                                [:lvar, :result],
                                [:when, [:array, [:str, "red"]],
                                 [:lasgn, :var, [:lit, 1]]],
                                [:when, [:array, [:str, "yellow"]],
                                 [:lasgn, :var, [:lit, 2]]],
                                [:when, [:array, [:str, "green"]],
                                 [:lasgn, :var, [:lit, 3]]],
                                nil]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :var, s(:lit, 2)),
                                s(:lasgn, :result, s(:str, "")),
                                s(:case,
                                  s(:lvar, :var),
                                  s(:when,
                                    s(:array, s(:lit, 1)),
                                    s(:block,
                                      s(:call, nil, :puts,
                                        s(:arglist, s(:str, "something"))),
                                      s(:lasgn, :result, s(:str, "red")))),
                                  s(:when,
                                    s(:array, s(:lit, 2), s(:lit, 3)),
                                    s(:lasgn, :result, s(:str, "yellow"))),
                                  s(:when, s(:array, s(:lit, 4)), nil),
                                  s(:lasgn, :result, s(:str, "green"))),
                                s(:case,
                                  s(:lvar, :result),
                                  s(:when, s(:array, s(:str, "red")),
                                    s(:lasgn, :var, s(:lit, 1))),
                                  s(:when, s(:array, s(:str, "yellow")),
                                    s(:lasgn, :var, s(:lit, 2))),
                                  s(:when, s(:array, s(:str, "green")),
                                    s(:lasgn, :var, s(:lit, 3))),
                                  nil)))

  add_tests("case_nested",
            "Ruby"         => "var1 = 1\nvar2 = 2\nresult = nil\ncase var1\nwhen 1 then\n  case var2\n  when 1 then\n    result = 1\n  when 2 then\n    result = 2\n  else\n    result = 3\n  end\nwhen 2 then\n  case var2\n  when 1 then\n    result = 4\n  when 2 then\n    result = 5\n  else\n    result = 6\n  end\nelse\n  result = 7\nend\n",
            "RawParseTree" => [:block,
                               [:lasgn, :var1, [:lit, 1]],
                               [:lasgn, :var2, [:lit, 2]],
                               [:lasgn, :result, [:nil]],
                               [:case,
                                [:lvar, :var1],
                                [:when, [:array, [:lit, 1]],
                                 [:case,
                                  [:lvar, :var2],
                                  [:when, [:array, [:lit, 1]],
                                   [:lasgn, :result, [:lit, 1]]],
                                  [:when, [:array, [:lit, 2]],
                                   [:lasgn, :result, [:lit, 2]]],
                                  [:lasgn, :result, [:lit, 3]]]],
                                [:when, [:array, [:lit, 2]],
                                 [:case,
                                  [:lvar, :var2],
                                  [:when, [:array, [:lit, 1]],
                                   [:lasgn, :result, [:lit, 4]]],
                                  [:when, [:array, [:lit, 2]],
                                   [:lasgn, :result, [:lit, 5]]],
                                  [:lasgn, :result, [:lit, 6]]]],
                                [:lasgn, :result, [:lit, 7]]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :var1, s(:lit, 1)),
                                s(:lasgn, :var2, s(:lit, 2)),
                                s(:lasgn, :result, s(:nil)),
                                s(:case,
                                  s(:lvar, :var1),
                                  s(:when, s(:array, s(:lit, 1)),
                                    s(:case,
                                      s(:lvar, :var2),
                                      s(:when, s(:array, s(:lit, 1)),
                                        s(:lasgn, :result, s(:lit, 1))),
                                      s(:when, s(:array, s(:lit, 2)),
                                        s(:lasgn, :result, s(:lit, 2))),
                                      s(:lasgn, :result, s(:lit, 3)))),
                                  s(:when, s(:array, s(:lit, 2)),
                                    s(:case,
                                      s(:lvar, :var2),
                                      s(:when, s(:array, s(:lit, 1)),
                                        s(:lasgn, :result, s(:lit, 4))),
                                      s(:when, s(:array, s(:lit, 2)),
                                        s(:lasgn, :result, s(:lit, 5))),
                                      s(:lasgn, :result, s(:lit, 6)))),
                                  s(:lasgn, :result, s(:lit, 7)))))

  add_tests("case_nested_inner_no_expr",
            "Ruby"         => "case a\nwhen b then\n  case\n  when (d and e) then\n    f\n  else\n    # do nothing\n  end\nelse\n  # do nothing\nend",
            "RawParseTree" => [:case, [:vcall, :a],
                               [:when, [:array, [:vcall, :b]],
                                [:case, nil,
                                 [:when,
                                  [:array, [:and, [:vcall, :d], [:vcall, :e]]],
                                  [:vcall, :f]],
                                 nil]],
                               nil],
            "ParseTree"    => s(:case, s(:call, nil, :a, s(:arglist)),
                                s(:when,
                                  s(:array, s(:call, nil, :b, s(:arglist))),
                                  s(:case, nil,
                                    s(:when,
                                      s(:array,
                                        s(:and,
                                          s(:call, nil, :d, s(:arglist)),
                                          s(:call, nil, :e, s(:arglist)))),
                                      s(:call, nil, :f, s(:arglist))),
                                    nil)),
                                nil))

  add_tests("case_no_expr",
            "Ruby"         => "case\nwhen (a == 1) then\n  :a\nwhen (a == 2) then\n  :b\nelse\n  :c\nend",
            "RawParseTree" => [:case, nil,
                               [:when,
                                [:array,
                                 [:call, [:vcall, :a], :==,
                                  [:array, [:lit, 1]]]],
                                [:lit, :a]],
                               [:when,
                                [:array,
                                 [:call, [:vcall, :a], :==,
                                  [:array, [:lit, 2]]]],
                                [:lit, :b]],
                               [:lit, :c]],
            "ParseTree"    => s(:case, nil,
                                s(:when,
                                  s(:array,
                                    s(:call,
                                      s(:call, nil, :a, s(:arglist)),
                                      :==,
                                      s(:arglist, s(:lit, 1)))),
                                  s(:lit, :a)),
                                s(:when,
                                  s(:array,
                                    s(:call,
                                      s(:call, nil, :a, s(:arglist)),
                                      :==,
                                      s(:arglist, s(:lit, 2)))),
                                  s(:lit, :b)),
                                s(:lit, :c)))

  add_tests("case_splat",
            "Ruby"         => "case a\nwhen :b, *c then\n  d\nelse\n  e\nend",
            "RawParseTree" => [:case, [:vcall, :a],
                               [:when,
                                [:array,
                                 [:lit, :b], [:when, [:vcall, :c], nil]], # wtf?
                                [:vcall, :d]],
                               [:vcall, :e]],
            "ParseTree"    => s(:case, s(:call, nil, :a, s(:arglist)),
                                s(:when,
                                  s(:array,
                                    s(:lit, :b),
                                    s(:when,
                                      s(:call, nil, :c, s(:arglist)),
                                      nil)), # wtf?
                                  s(:call, nil, :d, s(:arglist))),
                                s(:call, nil, :e, s(:arglist))))

  add_tests("cdecl",
            "Ruby"         => "X = 42",
            "RawParseTree" => [:cdecl, :X, [:lit, 42]],
            "ParseTree"    => s(:cdecl, :X, s(:lit, 42)))

  add_tests("class_plain",
            "Ruby"         => "class X\n  puts((1 + 1))\n  def blah\n    puts(\"hello\")\n  end\nend",
            "RawParseTree" => [:class,
                               :X,
                               nil,
                               [:scope,
                                [:block,
                                 [:fcall, :puts,
                                  [:array,
                                   [:call, [:lit, 1], :+,
                                    [:array, [:lit, 1]]]]],
                                 [:defn, :blah,
                                  [:scope,
                                   [:block,
                                    [:args],
                                    [:fcall, :puts,
                                     [:array, [:str, "hello"]]]]]]]]],
            "ParseTree"    => s(:class,
                                :X,
                                nil,
                                s(:scope,
                                  s(:block,
                                    s(:call, nil, :puts,
                                      s(:arglist,
                                        s(:call, s(:lit, 1), :+,
                                          s(:arglist, s(:lit, 1))))),
                                    s(:defn, :blah,
                                      s(:args),
                                      s(:scope,
                                        s(:block,
                                          s(:call, nil, :puts,
                                            s(:arglist,
                                              s(:str, "hello"))))))))))

  add_tests("class_scoped",
            "Ruby"         => "class X::Y\n  c\nend",
            "RawParseTree" => [:class, [:colon2, [:const, :X], :Y], nil,
                               [:scope, [:vcall, :c]]],
            "ParseTree"    => s(:class, s(:colon2, s(:const, :X), :Y), nil,
                                s(:scope, s(:call, nil, :c, s(:arglist)))))

  add_tests("class_scoped3",
            "Ruby"         => "class ::Y\n  c\nend",
            "RawParseTree" => [:class, [:colon3, :Y], nil,
                               [:scope, [:vcall, :c]]],
            "ParseTree"    => s(:class, s(:colon3, :Y), nil,
                                s(:scope, s(:call, nil, :c, s(:arglist)))))

  add_tests("class_super_array",
            "Ruby"         => "class X < Array\nend",
            "RawParseTree" => [:class,
                               :X,
                               [:const, :Array],
                               [:scope]],
            "ParseTree"    => s(:class,
                                :X,
                                s(:const, :Array),
                                s(:scope)))

  add_tests("class_super_expr",
            "Ruby"         => "class X < expr\nend",
            "RawParseTree" => [:class,
                               :X,
                               [:vcall, :expr],
                               [:scope]],
            "ParseTree"    => s(:class,
                                :X,
                                s(:call, nil, :expr, s(:arglist)),
                                s(:scope)))

  add_tests("class_super_object",
            "Ruby"         => "class X < Object\nend",
            "RawParseTree" => [:class,
                               :X,
                               [:const, :Object],
                               [:scope]],
            "ParseTree"    => s(:class,
                                :X,
                                s(:const, :Object),
                                s(:scope)))

  add_tests("colon2",
            "Ruby"         => "X::Y",
            "RawParseTree" => [:colon2, [:const, :X], :Y],
            "ParseTree"    => s(:colon2, s(:const, :X), :Y))

  add_tests("colon3",
            "Ruby"         => "::X",
            "RawParseTree" => [:colon3, :X],
            "ParseTree"    => s(:colon3, :X))

  add_tests("const",
            "Ruby"         => "X",
            "RawParseTree" => [:const, :X],
            "ParseTree"    => s(:const, :X))

  add_tests("constX",
            "Ruby"         => "X = 1",
            "RawParseTree" => [:cdecl, :X, [:lit, 1]],
            "ParseTree"    => s(:cdecl, :X, s(:lit, 1)))

  add_tests("constY",
            "Ruby"         => "::X = 1",
            "RawParseTree" => [:cdecl, [:colon3, :X], [:lit, 1]],
            "ParseTree"    => s(:cdecl, s(:colon3, :X), s(:lit, 1)))

  add_tests("constZ",
            "Ruby"         => "X::Y = 1",
            "RawParseTree" => [:cdecl, [:colon2, [:const, :X], :Y], [:lit, 1]],
            "ParseTree"    => s(:cdecl,
                                s(:colon2, s(:const, :X), :Y),
                                s(:lit, 1)))

  add_tests("cvar",
            "Ruby"         => "@@x",
            "RawParseTree" => [:cvar, :@@x],
            "ParseTree"    => s(:cvar, :@@x))

  add_tests("cvasgn",
            "Ruby"         => "def x\n  @@blah = 1\nend",
            "RawParseTree" => [:defn, :x,
                               [:scope,
                                [:block, [:args],
                                 [:cvasgn, :@@blah, [:lit, 1]]]]],
            "ParseTree"    => s(:defn, :x,
                                s(:args),
                                s(:scope,
                                  s(:block,
                                    s(:cvasgn, :@@blah, s(:lit, 1))))))

  add_tests("cvasgn_cls_method",
            "Ruby"         => "def self.quiet_mode=(boolean)\n  @@quiet_mode = boolean\nend",
            "RawParseTree" => [:defs, [:self], :quiet_mode=,
                               [:scope,
                                [:block,
                                 [:args, :boolean],
                                 [:cvasgn, :@@quiet_mode, [:lvar, :boolean]]]]],
            "ParseTree"    => s(:defs, s(:self), :quiet_mode=,
                                s(:args, :boolean),
                                s(:scope,
                                  s(:block,
                                    s(:cvasgn, :@@quiet_mode,
                                      s(:lvar, :boolean))))))

  add_tests("cvdecl",
            "Ruby"         => "class X\n  @@blah = 1\nend",
            "RawParseTree" => [:class, :X, nil,
                               [:scope, [:cvdecl, :@@blah, [:lit, 1]]]],
            "ParseTree"    => s(:class, :X, nil,
                                s(:scope, s(:cvdecl, :@@blah, s(:lit, 1)))))

  add_tests("dasgn_0",
            "Ruby"         => "a.each { |x| b.each { |y| x = (x + 1) } if true }",
            "RawParseTree" => [:iter,
                               [:call, [:vcall, :a], :each],
                               [:dasgn_curr, :x],
                               [:if, [:true],
                                [:iter,
                                 [:call, [:vcall, :b], :each],
                                 [:dasgn_curr, :y],
                                 [:dasgn, :x,
                                  [:call, [:dvar, :x], :+,
                                   [:array, [:lit, 1]]]]],
                                nil]],
            "ParseTree"    => s(:iter,
                                s(:call, s(:call, nil, :a, s(:arglist)), :each,
                                  s(:arglist)),
                                s(:lasgn, :x),
                                s(:if, s(:true),
                                  s(:iter,
                                    s(:call, s(:call, nil, :b, s(:arglist)),
                                      :each,
                                      s(:arglist)),
                                    s(:lasgn, :y),
                                    s(:lasgn, :x,
                                      s(:call, s(:lvar, :x), :+,
                                        s(:arglist, s(:lit, 1))))),
                                  nil)))

  add_tests("dasgn_1",
            "Ruby"         => "a.each { |x| b.each { |y| c = (c + 1) } if true }",
            "RawParseTree" => [:iter,
                               [:call, [:vcall, :a], :each],
                               [:dasgn_curr, :x],
                               [:if, [:true],
                                [:iter,
                                 [:call, [:vcall, :b], :each],
                                 [:dasgn_curr, :y],
                                 [:dasgn_curr, :c,
                                  [:call, [:dvar, :c], :+,
                                   [:array, [:lit, 1]]]]],
                                nil]],
            "ParseTree"    => s(:iter,
                                s(:call, s(:call, nil, :a, s(:arglist)), :each,
                                  s(:arglist)),
                                s(:lasgn, :x),
                                s(:if, s(:true),
                                  s(:iter,
                                    s(:call, s(:call, nil, :b, s(:arglist)),
                                      :each,
                                      s(:arglist)),
                                    s(:lasgn, :y),
                                    s(:lasgn, :c,
                                      s(:call, s(:lvar, :c), :+,
                                        s(:arglist, s(:lit, 1))))),
                                  nil)))

  add_tests("dasgn_2",
            "Ruby"         => "a.each do |x|\n  if true then\n    c = 0\n    b.each { |y| c = (c + 1) }\n  end\nend", # FIX: hate that extra newline!
            "RawParseTree" => [:iter,
                               [:call, [:vcall, :a], :each],
                               [:dasgn_curr, :x],
                               [:if, [:true],
                                [:block,
                                 [:dasgn_curr, :c, [:lit, 0]],
                                 [:iter,
                                  [:call, [:vcall, :b], :each],
                                  [:dasgn_curr, :y],
                                  [:dasgn, :c,
                                   [:call, [:dvar, :c], :+,
                                    [:array, [:lit, 1]]]]]],
                                nil]],
            "ParseTree"    => s(:iter,
                                s(:call, s(:call, nil, :a, s(:arglist)), :each,
                                  s(:arglist)),
                                s(:lasgn, :x),
                                s(:if, s(:true),
                                  s(:block,
                                    s(:lasgn, :c, s(:lit, 0)),
                                    s(:iter,
                                      s(:call, s(:call, nil, :b, s(:arglist)),
                                        :each,
                                        s(:arglist)),
                                      s(:lasgn, :y),
                                      s(:lasgn, :c,
                                        s(:call, s(:lvar, :c), :+,
                                          s(:arglist, s(:lit, 1)))))),
                                  nil)))

  add_tests("dasgn_curr",
            "Ruby"         => "data.each do |x, y|\n  a = 1\n  b = a\n  b = a = x\nend",
            "RawParseTree" => [:iter,
                               [:call, [:vcall, :data], :each],
                               [:masgn,
                                [:array, [:dasgn_curr, :x], [:dasgn_curr, :y]],
                                nil, nil],
                               [:block,
                                [:dasgn_curr, :a, [:lit, 1]],
                                [:dasgn_curr, :b, [:dvar, :a]],
                                [:dasgn_curr, :b,
                                 [:dasgn_curr, :a, [:dvar, :x]]]]],
            "ParseTree"    => s(:iter,
                                s(:call, s(:call, nil, :data,
                                           s(:arglist)), :each, s(:arglist)),
                                s(:masgn,
                                  s(:array, s(:lasgn, :x), s(:lasgn, :y))),
                                s(:block,
                                  s(:lasgn, :a, s(:lit, 1)),
                                  s(:lasgn, :b, s(:lvar, :a)),
                                  s(:lasgn, :b, s(:lasgn, :a, s(:lvar, :x))))))

  add_tests("dasgn_icky",
            "Ruby"         => "a do\n  v = nil\n  assert_block(full_message) do\n    begin\n      yield\n    rescue Exception => v\n      break\n    end\n  end\nend",
            "RawParseTree" => [:iter,
                               [:fcall, :a],
                               nil,
                               [:block,
                                [:dasgn_curr, :v, [:nil]],
                                [:iter,
                                 [:fcall, :assert_block,
                                  [:array, [:vcall, :full_message]]],
                                 nil,
                                 [:begin,
                                  [:rescue,
                                   [:yield],
                                   [:resbody,
                                    [:array, [:const, :Exception]],
                                    [:block,
                                     [:dasgn, :v,
                                      [:gvar, :$!]], [:break]]]]]]]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :a, s(:arglist)),
                                nil,
                                s(:block,
                                  s(:lasgn, :v, s(:nil)),
                                  s(:iter,
                                    s(:call, nil, :assert_block,
                                      s(:arglist,
                                        s(:call, nil, :full_message,
                                          s(:arglist)))),
                                    nil,
                                    s(:rescue,
                                      s(:yield),
                                      s(:resbody,
                                        s(:array,
                                          s(:const, :Exception),
                                          s(:lasgn, :v, s(:gvar, :$!))),
                                        s(:break)))))))

  add_tests("dasgn_mixed",
            "Ruby"         => "t = 0\nns.each { |n| t += n }\n",
            "RawParseTree" => [:block,
                               [:lasgn, :t, [:lit, 0]],
                               [:iter,
                                [:call, [:vcall, :ns], :each],
                                [:dasgn_curr, :n],
                                [:lasgn, :t,
                                 [:call, [:lvar, :t], :+,
                                  [:array, [:dvar, :n]]]]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :t, s(:lit, 0)),
                                s(:iter,
                                  s(:call, s(:call, nil, :ns,
                                             s(:arglist)), :each, s(:arglist)),
                                  s(:lasgn, :n),
                                  s(:lasgn, :t,
                                    s(:call, s(:lvar, :t), :+,
                                      s(:arglist, s(:lvar, :n)))))),
            "Ruby2Ruby"    => "t = 0\nns.each { |n| t = (t + n) }\n")

  add_tests("defined",
            "Ruby"         => "defined? $x",
            "RawParseTree" => [:defined, [:gvar, :$x]],
            "ParseTree"    => s(:defined, s(:gvar, :$x)))

  # TODO: make all the defn_args* p their arglist
  add_tests("defn_args_block",
            "Ruby"         => "def f(&block)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                                 [:args],
                                 [:block_arg, :block],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args, :"&block"),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_mand",
            "Ruby"         => "def f(mand)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                               [:args, :mand],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args, :mand),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_mand_block",
            "Ruby"         => "def f(mand, &block)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                                 [:args, :mand],
                                 [:block_arg, :block],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args, :mand, :"&block"),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_mand_opt",
            "Ruby"         => "def f(mand, opt = 42)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                               [:args, :mand, :opt,
                                [:block,
                                 [:lasgn, :opt, [:lit, 42]]]],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args, :mand, :opt,
                                  s(:block,
                                    s(:lasgn, :opt, s(:lit, 42)))),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_mand_opt_block",
            "Ruby"         => "def f(mand, opt = 42, &block)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                                 [:args, :mand, :opt,
                                  [:block,
                                   [:lasgn, :opt, [:lit, 42]]]],
                                 [:block_arg, :block],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args, :mand, :opt, :"&block",
                                  s(:block,
                                    s(:lasgn, :opt, s(:lit, 42)))),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_mand_opt_splat",
            "Ruby"         => "def f(mand, opt = 42, *rest)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                               [:args, :mand, :opt, :"*rest",
                                [:block,
                                 [:lasgn, :opt, [:lit, 42]]]],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args, :mand, :opt, :"*rest",
                                  s(:block,
                                    s(:lasgn, :opt, s(:lit, 42)))),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_mand_opt_splat_block",
            "Ruby"         => "def f(mand, opt = 42, *rest, &block)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                                 [:args, :mand, :opt, :"*rest",
                                  [:block,
                                   [:lasgn, :opt, [:lit, 42]]]],
                                 [:block_arg, :block],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args, :mand, :opt, :"*rest", :"&block",
                                  s(:block,
                                    s(:lasgn, :opt, s(:lit, 42)))),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_mand_opt_splat_no_name",
            "Ruby"         => "def x(a, b = 42, *)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :x,
                               [:scope,
                                [:block,
                                 [:args, :a, :b, :"*",
                                  [:block, [:lasgn, :b, [:lit, 42]]]],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :x,
                                s(:args, :a, :b, :"*",
                                  s(:block, s(:lasgn, :b, s(:lit, 42)))),
                                s(:scope,
                                  s(:block,
                                    s(:nil)))))

  add_tests("defn_args_mand_splat",
            "Ruby"         => "def f(mand, *rest)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                               [:args, :mand, :"*rest"],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args, :mand, :"*rest"),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_mand_splat_block",
            "Ruby"         => "def f(mand, *rest, &block)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                                 [:args, :mand, :"*rest"],
                                 [:block_arg, :block],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args, :mand, :"*rest", :"&block"),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_mand_splat_no_name",
            "Ruby"         => "def x(a, *args)\n  p(a, args)\nend",
            "RawParseTree" => [:defn, :x,
                               [:scope,
                                [:block,
                                 [:args, :a, :"*args"],
                                 [:fcall, :p,
                                  [:array, [:lvar, :a], [:lvar, :args]]]]]],
            "ParseTree"    => s(:defn, :x,
                                s(:args, :a, :"*args"),
                                s(:scope,
                                  s(:block,
                                    s(:call, nil, :p,
                                      s(:arglist, s(:lvar, :a), s(:lvar, :args)))))))

  add_tests("defn_args_none",
            "Ruby"         => "def empty\n  # do nothing\nend",
            "RawParseTree" => [:defn, :empty,
                               [:scope, [:block, [:args], [:nil]]]],
            "ParseTree"    => s(:defn, :empty,
                                s(:args),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_opt",
            "Ruby"         => "def f(opt = 42)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                               [:args, :opt,
                                [:block,
                                 [:lasgn, :opt, [:lit, 42]]]],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args, :opt,
                                  s(:block,
                                    s(:lasgn, :opt, s(:lit, 42)))),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_opt_block",
            "Ruby"         => "def f(opt = 42, &block)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                                 [:args, :opt,
                                  [:block,
                                   [:lasgn, :opt, [:lit, 42]]]],
                                 [:block_arg, :block],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args, :opt, :"&block",
                                  s(:block,
                                    s(:lasgn, :opt, s(:lit, 42)))),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_opt_splat",
            "Ruby"         => "def f(opt = 42, *rest)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                                 [:args, :opt, :"*rest",
                                  [:block,
                                   [:lasgn, :opt, [:lit, 42]]]],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args, :opt, :"*rest",
                                  s(:block,
                                    s(:lasgn, :opt, s(:lit, 42)))),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_opt_splat_block",
            "Ruby"         => "def f(opt = 42, *rest, &block)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                                 [:args, :opt, :"*rest",
                                  [:block,
                                   [:lasgn, :opt, [:lit, 42]]]],
                                 [:block_arg, :block],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args, :opt, :"*rest", :"&block",
                                  s(:block,
                                    s(:lasgn, :opt, s(:lit, 42)))),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_opt_splat_no_name",
            "Ruby"         => "def x(b = 42, *)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :x,
                               [:scope,
                                [:block,
                                 [:args, :b, :"*",
                                  [:block, [:lasgn, :b, [:lit, 42]]]],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :x,
                                s(:args, :b, :"*",
                                  s(:block, s(:lasgn, :b, s(:lit, 42)))),
                                s(:scope,
                                  s(:block,
                                    s(:nil)))))

  add_tests("defn_args_splat",
            "Ruby"         => "def f(*rest)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :f,
                               [:scope,
                                [:block,
                                 [:args, :"*rest"],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :f,
                                s(:args, :"*rest"),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_args_splat_no_name",
            "Ruby"         => "def x(*)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :x,
                               [:scope,
                                [:block,
                                 [:args, :"*"],
                                 [:nil]]]],
            "ParseTree"    => s(:defn, :x,
                                s(:args, :"*"),
                                s(:scope,
                                  s(:block,
                                    s(:nil)))))

  add_tests("defn_or",
            "Ruby"         => "def |(o)\n  # do nothing\nend",
            "RawParseTree" => [:defn, :|,
                               [:scope, [:block, [:args, :o], [:nil]]]],
            "ParseTree"    => s(:defn, :|,
                                s(:args, :o),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_rescue",
            "Ruby"         => "def eql?(resource)\n  (self.uuid == resource.uuid)\nrescue\n  false\nend",
            "RawParseTree" => [:defn, :eql?,
                               [:scope,
                                [:block,
                                 [:args, :resource],
                                 [:rescue,
                                  [:call,
                                   [:call, [:self], :uuid],
                                   :==,
                                   [:array,
                                    [:call, [:lvar, :resource], :uuid]]],
                                  [:resbody, nil, [:false]]]]]],
            "ParseTree"    => s(:defn, :eql?,
                                s(:args, :resource),
                                s(:scope,
                                  s(:block,
                                    s(:rescue,
                                      s(:call,
                                        s(:call, s(:self), :uuid, s(:arglist)),
                                        :==,
                                        s(:arglist,
                                          s(:call, s(:lvar, :resource),
                                            :uuid, s(:arglist)))),
                                      s(:resbody, s(:array), s(:false)))))),
            "Ruby2Ruby"    => "def eql?(resource)\n  (self.uuid == resource.uuid) rescue false\nend")

  add_tests("defn_rescue_mri_verbose_flag",
            "Ruby"         => "def eql?(resource)\n  (self.uuid == resource.uuid)\nrescue\n  false\nend",
            "RawParseTree" => [:defn, :eql?,
                               [:scope,
                                [:block,
                                 [:args, :resource],
                                 [:rescue,
                                  [:call,
                                   [:call, [:self], :uuid],
                                   :==,
                                   [:array,
                                    [:call, [:lvar, :resource], :uuid]]],
                                  [:resbody, nil, [:false]]]]]],
            "ParseTree"    => s(:defn, :eql?,
                                s(:args, :resource),
                                s(:scope,
                                  s(:block,
                                    s(:rescue,
                                      s(:call,
                                        s(:call, s(:self), :uuid, s(:arglist)),
                                        :==,
                                        s(:arglist,
                                          s(:call, s(:lvar, :resource),
                                            :uuid, s(:arglist)))),
                                      s(:resbody, s(:array), s(:false)))))),
            "Ruby2Ruby"    => "def eql?(resource)\n  (self.uuid == resource.uuid) rescue false\nend")

  add_tests("defn_something_eh",
            "Ruby"         => "def something?\n  # do nothing\nend",
            "RawParseTree" => [:defn, :something?,
                               [:scope, [:block, [:args], [:nil]]]],
            "ParseTree"    => s(:defn, :something?,
                                s(:args),
                                s(:scope, s(:block, s(:nil)))))

  add_tests("defn_splat_no_name",
            "Ruby"         => "def x(a, *)\n  p(a)\nend",
            "RawParseTree" => [:defn, :x,
                               [:scope,
                                [:block,
                                 [:args, :a, :"*"],
                                 [:fcall, :p,
                                  [:array, [:lvar, :a]]]]]],
            "ParseTree"    => s(:defn, :x,
                                s(:args, :a, :"*"),
                                s(:scope,
                                  s(:block,
                                    s(:call, nil, :p,
                                      s(:arglist, s(:lvar, :a)))))))

  add_tests("defn_zarray",
            "Ruby"         => "def zarray\n  a = []\n  return a\nend",
            "RawParseTree" => [:defn, :zarray,
                               [:scope,
                                [:block, [:args],
                                 [:lasgn, :a, [:zarray]],
                                 [:return, [:lvar, :a]]]]],
            "ParseTree"    => s(:defn, :zarray,
                                s(:args),
                                s(:scope,
                                  s(:block,
                                    s(:lasgn, :a, s(:array)),
                                    s(:return, s(:lvar, :a))))))

  add_tests("defs",
            "Ruby"         => "def self.x(y)\n  (y + 1)\nend",
            "RawParseTree" => [:defs, [:self], :x,
                               [:scope,
                                [:block,
                                 [:args, :y],
                                 [:call, [:lvar, :y], :+,
                                  [:array, [:lit, 1]]]]]],
            "ParseTree"    => s(:defs, s(:self), :x,
                                s(:args, :y),
                                s(:scope,
                                  s(:block,
                                    s(:call, s(:lvar, :y), :+,
                                      s(:arglist, s(:lit, 1)))))))

  add_tests("defs_empty",
            "Ruby"         => "def self.empty\n  # do nothing\nend",
            "RawParseTree" => [:defs, [:self], :empty,
                               [:scope, [:args]]],
            "ParseTree"    => s(:defs, s(:self), :empty,
                                s(:args),
                                s(:scope, s(:block))))

  add_tests("defs_empty_args",
            "Ruby"         => "def self.empty(*)\n  # do nothing\nend",
            "RawParseTree" => [:defs, [:self], :empty,
                               [:scope, [:args, :*]]],
            "ParseTree"    => s(:defs, s(:self), :empty,
                                s(:args, :*),
                                s(:scope, s(:block))))

  add_tests("defs_expr_wtf",
            "Ruby"         => "def (a.b).empty(*)\n  # do nothing\nend",
            "RawParseTree" => [:defs,
                               [:call, [:vcall, :a], :b],
                               :empty,
                               [:scope, [:args, :*]]],
            "ParseTree"    => s(:defs,
                                s(:call,
                                  s(:call, nil, :a, s(:arglist)),
                                  :b, s(:arglist)),
                                :empty,
                                s(:args, :*),
                                s(:scope, s(:block))))

  add_tests("dmethod",
            "Ruby"         => [Examples, :dmethod_added],
            "RawParseTree" => [:defn, :dmethod_added,
                               [:dmethod,
                                :a_method,
                                [:scope,
                                 [:block,
                                  [:args, :x],
                                  [:call, [:lvar, :x], :+,
                                   [:array, [:lit, 1]]]]]]],
            "ParseTree"    => s(:defn, :dmethod_added,
                                s(:args, :x),
                                s(:scope,
                                  s(:block,
                                    s(:call, s(:lvar, :x), :+,
                                      s(:arglist, s(:lit, 1)))))),
            "Ruby2Ruby"    => "def dmethod_added(x)\n  (x + 1)\nend")

  add_tests("dot2",
            "Ruby"         => "(a..b)",
            "RawParseTree" => [:dot2, [:vcall, :a], [:vcall, :b]],
            "ParseTree"    => s(:dot2,
                                s(:call, nil, :a, s(:arglist)),
                                s(:call, nil, :b, s(:arglist))))

  add_tests("dot3",
            "Ruby"         => "(a...b)",
            "RawParseTree" => [:dot3, [:vcall, :a], [:vcall, :b]],
            "ParseTree"    => s(:dot3,
                                s(:call, nil, :a, s(:arglist)),
                                s(:call, nil, :b, s(:arglist))))

  add_tests("dregx",
            "Ruby"         => "/x#\{(1 + 1)}y/",
            "RawParseTree" => [:dregx, "x",
                               [:evstr,
                                [:call, [:lit, 1], :+, [:array, [:lit, 1]]]],
                               [:str, "y"]],
            "ParseTree"    => s(:dregx, "x",
                                s(:evstr,
                                  s(:call, s(:lit, 1), :+,
                                    s(:arglist, s(:lit, 1)))),
                                s(:str, "y")))

  add_tests("dregx_interp",
            "Ruby"         => "/#\{@rakefile}/",
            "RawParseTree" => [:dregx, '', [:evstr, [:ivar, :@rakefile]]],
            "ParseTree"    => s(:dregx, '', s(:evstr, s(:ivar, :@rakefile))))

  add_tests("dregx_interp_empty",
            "Ruby"         => "/a#\{}b/",
            "RawParseTree" => [:dregx, 'a', [:evstr], [:str, "b"]],
            "ParseTree"    => s(:dregx, 'a', s(:evstr), s(:str, "b")))

  add_tests("dregx_n",
            "Ruby"         => '/#{1}/n',
            "RawParseTree" => [:dregx, '', [:evstr, [:lit, 1]], /x/n.options],
            "ParseTree"    => s(:dregx, '',
                                s(:evstr, s(:lit, 1)), /x/n.options),
            "Ruby2Ruby"    => "/#\{1}/") # HACK - need to support regexp flag

  add_tests("dregx_once",
            "Ruby"         => "/x#\{(1 + 1)}y/o",
            "RawParseTree" => [:dregx_once, "x",
                               [:evstr,
                                [:call, [:lit, 1], :+, [:array, [:lit, 1]]]],
                               [:str, "y"]],
            "ParseTree"    => s(:dregx_once, "x",
                                s(:evstr,
                                  s(:call, s(:lit, 1), :+,
                                    s(:arglist, s(:lit, 1)))),
                                s(:str, "y")))

  add_tests("dregx_once_n_interp",
            "Ruby"         => "/#\{IAC}#\{SB}/no",
            "RawParseTree" => [:dregx_once, '',
                               [:evstr, [:const, :IAC]],
                               [:evstr, [:const, :SB]], /x/n.options],
            "ParseTree"    => s(:dregx_once, '',
                                s(:evstr, s(:const, :IAC)),
                                s(:evstr, s(:const, :SB)), /x/n.options),
            "Ruby2Ruby"    => "/#\{IAC}#\{SB}/o") # HACK

  add_tests("dstr",
            "Ruby"         => "argl = 1\n\"x#\{argl}y\"\n",
            "RawParseTree" => [:block,
                               [:lasgn, :argl, [:lit, 1]],
                               [:dstr, "x", [:evstr, [:lvar, :argl]],
                                [:str, "y"]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :argl, s(:lit, 1)),
                                s(:dstr, "x", s(:evstr, s(:lvar, :argl)),
                                  s(:str, "y"))))

  add_tests("dstr_2",
            "Ruby"         => "argl = 1\n\"x#\{(\"%.2f\" % 3.14159)}y\"\n",
            "RawParseTree" =>   [:block,
                                 [:lasgn, :argl, [:lit, 1]],
                                 [:dstr,
                                  "x",
                                  [:evstr,
                                   [:call, [:str, "%.2f"], :%,
                                    [:array, [:lit, 3.14159]]]],
                                  [:str, "y"]]],
            "ParseTree"    =>   s(:block,
                                  s(:lasgn, :argl, s(:lit, 1)),
                                  s(:dstr,
                                    "x",
                                    s(:evstr,
                                      s(:call, s(:str, "%.2f"), :%,
                                        s(:arglist, s(:lit, 3.14159)))),
                                    s(:str, "y"))))

  add_tests("dstr_3",
            "Ruby"         => "max = 2\nargl = 1\n\"x#\{(\"%.#\{max}f\" % 3.14159)}y\"\n",
            "RawParseTree" =>   [:block,
                                 [:lasgn, :max, [:lit, 2]],
                                 [:lasgn, :argl, [:lit, 1]],
                                 [:dstr, "x",
                                  [:evstr,
                                   [:call,
                                    [:dstr, "%.",
                                     [:evstr, [:lvar, :max]],
                                     [:str, "f"]],
                                    :%,
                                    [:array, [:lit, 3.14159]]]],
                                  [:str, "y"]]],
            "ParseTree"    =>   s(:block,
                                  s(:lasgn, :max, s(:lit, 2)),
                                  s(:lasgn, :argl, s(:lit, 1)),
                                  s(:dstr, "x",
                                    s(:evstr,
                                      s(:call,
                                        s(:dstr, "%.",
                                          s(:evstr, s(:lvar, :max)),
                                          s(:str, "f")),
                                        :%,
                                        s(:arglist, s(:lit, 3.14159)))),
                                    s(:str, "y"))))

  add_tests("dstr_concat",
            "Ruby"         => '"#{22}aa" "cd#{44}" "55" "#{66}"',
            "RawParseTree" => [:dstr,
                               "",
                               [:evstr, [:lit, 22]],
                               [:str, "aa"],
                               [:str, "cd"],
                               [:evstr, [:lit, 44]],
                               [:str, "55"],
                               [:evstr, [:lit, 66]]],
            "ParseTree"    => s(:dstr,
                                "",
                                s(:evstr, s(:lit, 22)),
                                s(:str, "aa"),
                                s(:str, "cd"),
                                s(:evstr, s(:lit, 44)),
                                s(:str, "55"),
                                s(:evstr, s(:lit, 66))),
            "Ruby2Ruby"    => '"#{22}aacd#{44}55#{66}"')

  add_tests("dstr_gross",
            "Ruby"         => '"a #$global b #@ivar c #@@cvar d"',
            "RawParseTree" => [:dstr, "a ",
                               [:evstr, [:gvar, :$global]],
                               [:str, " b "],
                               [:evstr, [:ivar, :@ivar]],
                               [:str, " c "],
                               [:evstr, [:cvar, :@@cvar]],
                               [:str, " d"]],
            "ParseTree"    => s(:dstr, "a ",
                                s(:evstr, s(:gvar, :$global)),
                                s(:str, " b "),
                                s(:evstr, s(:ivar, :@ivar)),
                                s(:str, " c "),
                                s(:evstr, s(:cvar, :@@cvar)),
                                s(:str, " d")),
            "Ruby2Ruby" => '"a #{$global} b #{@ivar} c #{@@cvar} d"')

  add_tests("dstr_heredoc_expand",
            "Ruby"         => "<<EOM\n  blah\n#\{1 + 1}blah\nEOM\n",
            "RawParseTree" => [:dstr, "  blah\n",
                               [:evstr, [:call, [:lit, 1], :+,
                                         [:array, [:lit, 1]]]],
                               [:str, "blah\n"]],
            "ParseTree"    => s(:dstr, "  blah\n",
                                s(:evstr, s(:call, s(:lit, 1), :+,
                                            s(:arglist, s(:lit, 1)))),
                                s(:str, "blah\n")),
            "Ruby2Ruby"    => "\"  blah\\n#\{(1 + 1)}blah\\n\"")

  add_tests("dstr_heredoc_windoze_sucks",
            "Ruby"         => "<<-EOF\r\ndef test_#\{action}_valid_feed\r\n  EOF\r\n",
            "RawParseTree" => [:dstr,
                               'def test_',
                               [:evstr, [:vcall, :action]],
                               [:str, "_valid_feed\n"]],
            "ParseTree"    => s(:dstr,
                                'def test_',
                                s(:evstr, s(:call, nil, :action, s(:arglist))),
                                s(:str, "_valid_feed\n")),
            "Ruby2Ruby"    => "\"def test_#\{action}_valid_feed\\n\"")

  add_tests("dstr_heredoc_yet_again",
            "Ruby"         => "<<-EOF\ns1 '#\{RUBY_PLATFORM}' s2\n#\{__FILE__}\n        EOF\n",
            "RawParseTree" => [:dstr, "s1 '",
                               [:evstr, [:const, :RUBY_PLATFORM]],
                               [:str, "' s2\n"],
                               [:str, "(string)"],
                               [:str, "\n"]],
            "ParseTree"    => s(:dstr, "s1 '",
                                s(:evstr, s(:const, :RUBY_PLATFORM)),
                                s(:str, "' s2\n"),
                                s(:str, "(string)"),
                                s(:str, "\n")),
            "Ruby2Ruby"    => "\"s1 '#\{RUBY_PLATFORM}' s2\\n(string)\\n\"")

  add_tests("dstr_nest",
            "Ruby"         => "%Q[before [#\{nest}] after]",
            "RawParseTree" => [:dstr, "before [",
                               [:evstr, [:vcall, :nest]], [:str, "] after"]],
            "ParseTree"    => s(:dstr, "before [",
                                s(:evstr, s(:call, nil, :nest, s(:arglist))),
                                s(:str, "] after")),
            "Ruby2Ruby"    => "\"before [#\{nest}] after\"")

  add_tests("dstr_str_lit_start",
            "Ruby"         => '"#{"blah"}#{__FILE__}:#{__LINE__}: warning: #{$!.message} (#{$!.class})"',
            "RawParseTree" => [:dstr,
                               "blah(string):",
                               [:evstr, [:lit, 1]],
                               [:str, ": warning: "],
                               [:evstr, [:call, [:gvar, :$!], :message]],
                               [:str, " ("],
                               [:evstr, [:call, [:gvar, :$!], :class]],
                               [:str, ")"]],
            "ParseTree"    => s(:dstr,
                                "blah(string):",
                                s(:evstr, s(:lit, 1)),
                                s(:str, ": warning: "),
                                s(:evstr, s(:call, s(:gvar, :$!), :message,
                                            s(:arglist))),
                                s(:str, " ("),
                                s(:evstr, s(:call, s(:gvar, :$!), :class,
                                            s(:arglist))),
                                s(:str, ")")),
            "Ruby2Ruby"    => '"blah(string):#{1}: warning: #{$!.message} (#{$!.class})"')

  add_tests("dstr_the_revenge",
            "Ruby"         => '"before #{from} middle #{to} (#{__FILE__}:#{__LINE__})"',
            "RawParseTree" => [:dstr,
                               "before ",
                               [:evstr, [:vcall, :from]],
                               [:str, " middle "],
                               [:evstr, [:vcall, :to]],
                               [:str, " ("],
                               [:str, "(string)"],
                               [:str, ":"],
                               [:evstr, [:lit, 1]],
                               [:str, ")"]],
            "ParseTree"    => s(:dstr,
                                "before ",
                                s(:evstr, s(:call, nil, :from, s(:arglist))),
                                s(:str, " middle "),
                                s(:evstr, s(:call, nil, :to, s(:arglist))),
                                s(:str, " ("),
                                s(:str, "(string)"),
                                s(:str, ":"),
                                s(:evstr, s(:lit, 1)),
                                s(:str, ")")),
            "Ruby2Ruby"    => '"before #{from} middle #{to} ((string):#{1})"')

  add_tests("dsym",
            "Ruby"         => ":\"x#\{(1 + 1)}y\"",
            "RawParseTree" => [:dsym, "x",
                               [:evstr, [:call, [:lit, 1], :+,
                                         [:array, [:lit, 1]]]], [:str, "y"]],
            "ParseTree"    => s(:dsym, "x",
                                s(:evstr, s(:call, s(:lit, 1), :+,
                                            s(:arglist, s(:lit, 1)))), s(:str, "y")))

  add_tests("dxstr",
            "Ruby"         => "t = 5\n`touch #\{t}`\n",
            "RawParseTree" => [:block,
                               [:lasgn, :t, [:lit, 5]],
                               [:dxstr, 'touch ', [:evstr, [:lvar, :t]]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :t, s(:lit, 5)),
                                s(:dxstr, 'touch ', s(:evstr, s(:lvar, :t)))))

  add_tests("ensure",
            "Ruby"         => "begin\n  (1 + 1)\nrescue SyntaxError => e1\n  2\nrescue Exception => e2\n  3\nelse\n  4\nensure\n  5\nend",
            "RawParseTree" => [:begin,
                               [:ensure,
                                [:rescue,
                                 [:call, [:lit, 1], :+, [:array, [:lit, 1]]],
                                 [:resbody,
                                  [:array, [:const, :SyntaxError]],
                                  [:block,
                                   [:lasgn, :e1, [:gvar, :$!]], [:lit, 2]],
                                  [:resbody,
                                   [:array, [:const, :Exception]],
                                   [:block,
                                    [:lasgn, :e2, [:gvar, :$!]], [:lit, 3]]]],
                                 [:lit, 4]],
                                [:lit, 5]]],
            "ParseTree"    => s(:ensure,
                                s(:rescue,
                                  s(:call, s(:lit, 1), :+,
                                    s(:arglist, s(:lit, 1))),
                                  s(:resbody,
                                    s(:array,
                                      s(:const, :SyntaxError),
                                      s(:lasgn, :e1, s(:gvar, :$!))),
                                    s(:lit, 2)),
                                  s(:resbody,
                                    s(:array,
                                      s(:const, :Exception),
                                      s(:lasgn, :e2, s(:gvar, :$!))),
                                    s(:lit, 3)),
                                  s(:lit, 4)),
                                s(:lit, 5)))

  add_tests("false",
            "Ruby"         => "false",
            "RawParseTree" => [:false],
            "ParseTree"    => s(:false))

  add_tests("fbody",
            "Ruby"         => [Examples, :an_alias],
            "RawParseTree" => [:defn, :an_alias,
                               [:fbody,
                                [:scope,
                                 [:block,
                                  [:args, :x],
                                  [:call, [:lvar, :x], :+,
                                   [:array, [:lit, 1]]]]]]],
            "ParseTree"    => s(:defn, :an_alias,
                                s(:args, :x),
                                s(:scope,
                                  s(:block,
                                    s(:call, s(:lvar, :x), :+,
                                      s(:arglist, s(:lit, 1)))))),
            "Ruby2Ruby"    => "def an_alias(x)\n  (x + 1)\nend")

  add_tests("fcall_arglist",
            "Ruby"         => "m(42)",
            "RawParseTree" => [:fcall, :m, [:array, [:lit, 42]]],
            "ParseTree"    => s(:call, nil, :m, s(:arglist, s(:lit, 42))))

  add_tests("fcall_arglist_hash",
            "Ruby"         => "m(:a => 1, :b => 2)",
            "RawParseTree" => [:fcall, :m,
                               [:array,
                                [:hash,
                                 [:lit, :a], [:lit, 1],
                                 [:lit, :b], [:lit, 2]]]],
            "ParseTree"    => s(:call, nil, :m,
                                s(:arglist,
                                  s(:hash,
                                    s(:lit, :a), s(:lit, 1),
                                    s(:lit, :b), s(:lit, 2)))))

  add_tests("fcall_arglist_norm_hash",
            "Ruby"         => "m(42, :a => 1, :b => 2)",
            "RawParseTree" => [:fcall, :m,
                               [:array,
                                [:lit, 42],
                                [:hash,
                                 [:lit, :a], [:lit, 1],
                                 [:lit, :b], [:lit, 2]]]],
            "ParseTree"    => s(:call, nil, :m,
                                s(:arglist,
                                  s(:lit, 42),
                                  s(:hash,
                                    s(:lit, :a), s(:lit, 1),
                                    s(:lit, :b), s(:lit, 2)))))

  add_tests("fcall_arglist_norm_hash_splat",
            "Ruby"         => "m(42, :a => 1, :b => 2, *c)",
            "RawParseTree" => [:fcall, :m,
                               [:argscat,
                                [:array,
                                 [:lit, 42],
                                 [:hash,
                                  [:lit, :a], [:lit, 1],
                                  [:lit, :b], [:lit, 2]]],
                                [:vcall, :c]]],
            "ParseTree"    => s(:call, nil, :m,
                                s(:arglist,
                                  s(:lit, 42),
                                  s(:hash,
                                    s(:lit, :a), s(:lit, 1),
                                    s(:lit, :b), s(:lit, 2)),
                                  s(:splat, s(:call, nil, :c, s(:arglist))))))

  add_tests("fcall_block",
            "Ruby"         => "a(:b) { :c }",
            "RawParseTree" => [:iter,
                               [:fcall, :a, [:array, [:lit, :b]]], nil,
                               [:lit, :c]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :a,
                                  s(:arglist, s(:lit, :b))), nil,
                                s(:lit, :c)))

  add_tests("fcall_index_space",
            "Ruby"         => "a [42]",
            "RawParseTree" => [:fcall, :a, [:array, [:array, [:lit, 42]]]],
            "ParseTree"    => s(:call, nil, :a,
                                s(:arglist, s(:array, s(:lit, 42)))),
            "Ruby2Ruby"    => "a([42])")

  add_tests("fcall_keyword",
            "Ruby"         => "42 if block_given?",
            "RawParseTree" => [:if, [:fcall, :block_given?], [:lit, 42], nil],
            "ParseTree"    => s(:if,
                                s(:call, nil, :block_given?, s(:arglist)),
                                s(:lit, 42), nil))

  add_tests("flip2",
            "Ruby"         => "x = if ((i % 4) == 0)..((i % 3) == 0) then\n  i\nelse\n  nil\nend",
            "RawParseTree" => [:lasgn,
                               :x,
                               [:if,
                                [:flip2,
                                 [:call,
                                  [:call, [:vcall, :i], :%,
                                   [:array, [:lit, 4]]],
                                  :==,
                                  [:array, [:lit, 0]]],
                                 [:call,
                                  [:call, [:vcall, :i], :%,
                                   [:array, [:lit, 3]]],
                                  :==,
                                  [:array, [:lit, 0]]]],
                                [:vcall, :i],
                                [:nil]]],
            "ParseTree"    => s(:lasgn,
                                :x,
                                s(:if,
                                  s(:flip2,
                                    s(:call,
                                      s(:call, s(:call, nil, :i, s(:arglist)),
                                        :%,
                                        s(:arglist, s(:lit, 4))),
                                      :==,
                                      s(:arglist, s(:lit, 0))),
                                    s(:call,
                                      s(:call, s(:call, nil, :i, s(:arglist)),
                                        :%,
                                        s(:arglist, s(:lit, 3))),
                                      :==,
                                      s(:arglist, s(:lit, 0)))),
                                  s(:call, nil, :i, s(:arglist)),
                                  s(:nil))))

  add_tests("flip2_method",
            "Ruby"         => "if 1..2.a?(b) then\n  nil\nend",
            "RawParseTree" => [:if,
                               [:flip2,
                                [:lit, 1],
                                [:call, [:lit, 2], :a?,
                                 [:array, [:vcall, :b]]]],
                               [:nil],
                               nil],
            "ParseTree"    => s(:if,
                                s(:flip2,
                                  s(:lit, 1),
                                  s(:call, s(:lit, 2), :a?,
                                    s(:arglist,
                                      s(:call, nil, :b, s(:arglist))))),
                                s(:nil),
                                nil))

  add_tests("flip3",
            "Ruby"         => "x = if ((i % 4) == 0)...((i % 3) == 0) then\n  i\nelse\n  nil\nend",
            "RawParseTree" => [:lasgn,
                               :x,
                               [:if,
                                [:flip3,
                                 [:call,
                                  [:call, [:vcall, :i], :%,
                                   [:array, [:lit, 4]]],
                                  :==,
                                  [:array, [:lit, 0]]],
                                 [:call,
                                  [:call, [:vcall, :i], :%,
                                   [:array, [:lit, 3]]],
                                  :==,
                                  [:array, [:lit, 0]]]],
                                [:vcall, :i],
                                [:nil]]],
            "ParseTree"    => s(:lasgn,
                                :x,
                                s(:if,
                                  s(:flip3,
                                    s(:call,
                                      s(:call, s(:call, nil, :i, s(:arglist)),
                                        :%,
                                        s(:arglist, s(:lit, 4))),
                                      :==,
                                      s(:arglist, s(:lit, 0))),
                                    s(:call,
                                      s(:call, s(:call, nil, :i, s(:arglist)),
                                        :%,
                                        s(:arglist, s(:lit, 3))),
                                      :==,
                                      s(:arglist, s(:lit, 0)))),
                                  s(:call, nil, :i, s(:arglist)),
                                  s(:nil))))

  add_tests("for",
            "Ruby"         => "for o in ary do\n  puts(o)\nend",
            "RawParseTree" => [:for,
                               [:vcall, :ary],
                               [:lasgn, :o],
                               [:fcall, :puts, [:array, [:lvar, :o]]]],
            "ParseTree"    => s(:for,
                                s(:call, nil, :ary, s(:arglist)),
                                s(:lasgn, :o),
                                s(:call, nil, :puts,
                                  s(:arglist, s(:lvar, :o)))))

  add_tests("for_no_body",
            "Ruby"         => "for i in (0..max) do\n  # do nothing\nend",
            "RawParseTree" => [:for,
                               [:dot2, [:lit, 0], [:vcall, :max]],
                               [:lasgn, :i]],
            "ParseTree"    => s(:for,
                                s(:dot2,
                                  s(:lit, 0),
                                  s(:call, nil, :max, s(:arglist))),
                                s(:lasgn, :i)))

  add_tests("gasgn",
            "Ruby"         => "$x = 42",
            "RawParseTree" => [:gasgn, :$x, [:lit, 42]],
            "ParseTree"    => s(:gasgn, :$x, s(:lit, 42)))

  add_tests("global",
            "Ruby"         => "$stderr",
            "RawParseTree" =>  [:gvar, :$stderr],
            "ParseTree"    =>  s(:gvar, :$stderr))

  add_tests("gvar",
            "Ruby"         => "$x",
            "RawParseTree" => [:gvar, :$x],
            "ParseTree"    => s(:gvar, :$x))

  add_tests("gvar_underscore",
            "Ruby"         => "$_",
            "RawParseTree" => [:gvar, :$_],
            "ParseTree"    => s(:gvar, :$_))

  add_tests("gvar_underscore_blah",
            "Ruby"         => "$__blah",
            "RawParseTree" => [:gvar, :$__blah],
            "ParseTree"    => s(:gvar, :$__blah))

  add_tests("hash",
            "Ruby"         => "{ 1 => 2, 3 => 4 }",
            "RawParseTree" => [:hash,
                               [:lit, 1], [:lit, 2],
                               [:lit, 3], [:lit, 4]],
            "ParseTree"    => s(:hash,
                                s(:lit, 1), s(:lit, 2),
                                s(:lit, 3), s(:lit, 4)))

  add_tests("hash_rescue",
            "Ruby"         => "{ 1 => (2 rescue 3) }",
            "RawParseTree" => [:hash,
                               [:lit, 1],
                               [:rescue,
                                [:lit, 2],
                                [:resbody, nil, [:lit, 3]]]],
            "ParseTree"    => s(:hash,
                                s(:lit, 1),
                                s(:rescue,
                                  s(:lit, 2),
                                  s(:resbody, s(:array), s(:lit, 3)))))

  add_tests("iasgn",
            "Ruby"         => "@a = 4",
            "RawParseTree" => [:iasgn, :@a, [:lit, 4]],
            "ParseTree"    => s(:iasgn, :@a, s(:lit, 4)))

  add_tests("if_block_condition",
            "Ruby"         => "if (x = 5\n(x + 1)) then\n  nil\nend",
            "RawParseTree" => [:if,
                               [:block,
                                [:lasgn, :x, [:lit, 5]],
                                [:call,
                                 [:lvar, :x],
                                 :+,
                                 [:array, [:lit, 1]]]],
                               [:nil],
                               nil],
            "ParseTree"    => s(:if,
                                s(:block,
                                  s(:lasgn, :x, s(:lit, 5)),
                                  s(:call,
                                    s(:lvar, :x),
                                    :+,
                                    s(:arglist, s(:lit, 1)))),
                                s(:nil),
                                nil))

  add_tests("if_lasgn_short",
            "Ruby"         => "if x = obj.x then\n  x.do_it\nend",
            "RawParseTree" => [:if,
                               [:lasgn, :x,
                                [:call, [:vcall, :obj], :x]],
                               [:call,
                                [:lvar, :x], :do_it],
                               nil],
            "ParseTree"    => s(:if,
                                s(:lasgn, :x,
                                  s(:call,
                                    s(:call, nil, :obj, s(:arglist)),
                                    :x, s(:arglist))),
                                s(:call, s(:lvar, :x), :do_it, s(:arglist)),
                                nil))

  add_tests("if_nested",
            "Ruby"         => "return if false unless true",
            "RawParseTree" => [:if, [:true], nil,
                               [:if, [:false], [:return], nil]],
            "ParseTree"    => s(:if, s(:true), nil,
                                s(:if, s(:false), s(:return), nil)))

  add_tests("if_post",
            "Ruby"         => "a if b",
            "RawParseTree" => [:if, [:vcall, :b], [:vcall, :a], nil],
            "ParseTree"    => s(:if, s(:call, nil, :b, s(:arglist)),
                                s(:call, nil, :a, s(:arglist)), nil))

  add_tests("if_post_not",
            "Ruby"         => "a if not b",
            "RawParseTree" => [:if, [:vcall, :b], nil, [:vcall, :a]],
            "ParseTree"    => s(:if, s(:call, nil, :b, s(:arglist)), nil,
                                s(:call, nil, :a, s(:arglist))),
            "Ruby2Ruby"    => "a unless b")

  add_tests("if_pre",
            "Ruby"         => "if b then a end",
            "RawParseTree" => [:if, [:vcall, :b], [:vcall, :a], nil],
            "ParseTree"    => s(:if, s(:call, nil, :b, s(:arglist)),
                                s(:call, nil, :a, s(:arglist)), nil),
            "Ruby2Ruby"    => "a if b")

  add_tests("if_pre_not",
            "Ruby"         => "if not b then a end",
            "RawParseTree" => [:if, [:vcall, :b], nil, [:vcall, :a]],
            "ParseTree"    => s(:if, s(:call, nil, :b, s(:arglist)), nil,
                                s(:call, nil, :a, s(:arglist))),
            "Ruby2Ruby"    => "a unless b")

  add_tests("iter_call_arglist_space",
            "Ruby" => "a (1) {|c|d}",
            "RawParseTree" => [:iter,
                               [:fcall, :a, [:array, [:lit, 1]]],
                               [:dasgn_curr, :c],
                               [:vcall, :d]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :a, s(:arglist, s(:lit, 1))),
                                s(:lasgn, :c),
                                s(:call, nil, :d, s(:arglist))),
            "Ruby2Ruby"    => "a(1) { |c| d }")

  add_tests("iter_dasgn_curr_dasgn_madness",
            "Ruby"         => "as.each { |a|\n  b += a.b(false) }",
            "RawParseTree" => [:iter,
                               [:call, [:vcall, :as], :each],
                               [:dasgn_curr, :a],
                               [:dasgn_curr,
                                :b,
                                [:call,
                                 [:dvar, :b],
                                 :+,
                                 [:array,
                                  [:call, [:dvar, :a], :b,
                                   [:array, [:false]]]]]]],
            "ParseTree"    => s(:iter,
                                s(:call,
                                  s(:call, nil, :as, s(:arglist)),
                                  :each, s(:arglist)),
                                s(:lasgn, :a),
                                s(:lasgn, :b,
                                  s(:call,
                                    s(:lvar, :b),
                                    :+,
                                    s(:arglist,
                                      s(:call, s(:lvar, :a), :b,
                                        s(:arglist, s(:false))))))),
            "Ruby2Ruby"    => "as.each { |a| b = (b + a.b(false)) }")

  add_tests("iter_downto",
            "Ruby"         => "3.downto(1) { |n| puts(n.to_s) }",
            "RawParseTree" => [:iter,
                               [:call, [:lit, 3], :downto, [:array, [:lit, 1]]],
                               [:dasgn_curr, :n],
                               [:fcall, :puts,
                                [:array, [:call, [:dvar, :n], :to_s]]]],
            "ParseTree"    => s(:iter,
                                s(:call, s(:lit, 3), :downto,
                                  s(:arglist, s(:lit, 1))),
                                s(:lasgn, :n),
                                s(:call, nil, :puts,
                                  s(:arglist,
                                    s(:call, s(:lvar, :n),
                                      :to_s, s(:arglist))))))

  add_tests("iter_each_lvar",
            "Ruby"         => "array = [1, 2, 3]\narray.each { |x| puts(x.to_s) }\n",
            "RawParseTree" => [:block,
                               [:lasgn, :array,
                                [:array, [:lit, 1], [:lit, 2], [:lit, 3]]],
                               [:iter,
                                [:call, [:lvar, :array], :each],
                                [:dasgn_curr, :x],
                                [:fcall, :puts,
                                 [:array, [:call, [:dvar, :x], :to_s]]]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :array,
                                  s(:array,
                                    s(:lit, 1), s(:lit, 2), s(:lit, 3))),
                                s(:iter,
                                  s(:call, s(:lvar, :array), :each,
                                    s(:arglist)),
                                  s(:lasgn, :x),
                                  s(:call, nil, :puts,
                                    s(:arglist, s(:call, s(:lvar, :x),
                                                  :to_s, s(:arglist)))))))

  add_tests("iter_each_nested",
            "Ruby"         => "array1 = [1, 2, 3]\narray2 = [4, 5, 6, 7]\narray1.each do |x|\n  array2.each do |y|\n    puts(x.to_s)\n    puts(y.to_s)\n  end\nend\n",
            "RawParseTree" => [:block,
                               [:lasgn, :array1,
                                [:array, [:lit, 1], [:lit, 2], [:lit, 3]]],
                               [:lasgn, :array2,
                                [:array,
                                 [:lit, 4], [:lit, 5], [:lit, 6], [:lit, 7]]],
                               [:iter,
                                [:call,
                                 [:lvar, :array1], :each],
                                [:dasgn_curr, :x],
                                [:iter,
                                 [:call,
                                  [:lvar, :array2], :each],
                                 [:dasgn_curr, :y],
                                 [:block,
                                  [:fcall, :puts,
                                   [:array, [:call, [:dvar, :x], :to_s]]],
                                  [:fcall, :puts,
                                   [:array, [:call, [:dvar, :y], :to_s]]]]]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :array1,
                                  s(:array,
                                    s(:lit, 1), s(:lit, 2), s(:lit, 3))),
                                s(:lasgn, :array2,
                                  s(:array,
                                    s(:lit, 4), s(:lit, 5),
                                    s(:lit, 6), s(:lit, 7))),
                                s(:iter,
                                  s(:call,
                                    s(:lvar, :array1), :each, s(:arglist)),
                                  s(:lasgn, :x),
                                  s(:iter,
                                    s(:call,
                                      s(:lvar, :array2), :each, s(:arglist)),
                                    s(:lasgn, :y),
                                    s(:block,
                                      s(:call, nil, :puts,
                                        s(:arglist,
                                          s(:call, s(:lvar, :x),
                                            :to_s, s(:arglist)))),
                                      s(:call, nil, :puts,
                                        s(:arglist,
                                          s(:call, s(:lvar, :y),
                                            :to_s, s(:arglist)))))))))

  add_tests("iter_loop_empty",
            "Ruby"         => "loop { }",
            "RawParseTree" => [:iter, [:fcall, :loop], nil],
            "ParseTree"    => s(:iter, s(:call, nil, :loop, s(:arglist)), nil))

  add_tests("iter_masgn_2",
            "Ruby"         => "a { |b, c| p(c) }",
            "RawParseTree" => [:iter,
                               [:fcall, :a],
                               [:masgn,
                                [:array, [:dasgn_curr, :b], [:dasgn_curr, :c]],
                                nil, nil],
                               [:fcall, :p, [:array, [:dvar, :c]]]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :a, s(:arglist)),
                                s(:masgn,
                                  s(:array, s(:lasgn, :b), s(:lasgn, :c))),
                                s(:call, nil, :p, s(:arglist, s(:lvar, :c)))))

  add_tests("iter_masgn_args_splat",
            "Ruby"         => "a { |b, c, *d| p(c) }",
            "RawParseTree" => [:iter,
                               [:fcall, :a],
                               [:masgn,
                                [:array, [:dasgn_curr, :b], [:dasgn_curr, :c]],
                                [:dasgn_curr, :d], nil],
                               [:fcall, :p, [:array, [:dvar, :c]]]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :a, s(:arglist)),
                                s(:masgn,
                                  s(:array,
                                    s(:lasgn, :b),
                                    s(:lasgn, :c),
                                    s(:splat, s(:lasgn, :d)))),
                                s(:call, nil, :p, s(:arglist, s(:lvar, :c)))))

  add_tests("iter_masgn_args_splat_no_name",
            "Ruby"         => "a { |b, c, *| p(c) }",
            "RawParseTree" => [:iter,
                               [:fcall, :a],
                               [:masgn,
                                [:array, [:dasgn_curr, :b], [:dasgn_curr, :c]],
                                [:splat], nil],
                               [:fcall, :p, [:array, [:dvar, :c]]]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :a, s(:arglist)),
                                s(:masgn,
                                  s(:array,
                                    s(:lasgn, :b),
                                    s(:lasgn, :c),
                                    s(:splat))),
                                s(:call, nil, :p, s(:arglist, s(:lvar, :c)))))

  add_tests("iter_masgn_splat",
            "Ruby"         => "a { |*c| p(c) }",
            "RawParseTree" => [:iter,
                               [:fcall, :a],
                               [:masgn, nil, [:dasgn_curr, :c], nil],
                               [:fcall, :p, [:array, [:dvar, :c]]]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :a, s(:arglist)),
                                s(:masgn, s(:array, s(:splat, s(:lasgn, :c)))),
                                s(:call, nil, :p, s(:arglist, s(:lvar, :c)))))

  add_tests("iter_masgn_splat_no_name",
            "Ruby"         => "a { |*| p(c) }",
            "RawParseTree" => [:iter,
                               [:fcall, :a],
                               [:masgn, nil, [:splat], nil],
                               [:fcall, :p, [:array, [:vcall, :c]]]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :a, s(:arglist)),
                                s(:masgn, s(:array, s(:splat))),
                                s(:call, nil, :p,
                                  s(:arglist, s(:call, nil, :c, s(:arglist))))))

  add_tests("iter_shadowed_var",
            "Ruby"         => "a do |x|\n  b do |x|\n    puts x\n  end\nend",
            "RawParseTree" => [:iter,
                               [:fcall, :a],
                               [:dasgn_curr, :x],
                               [:iter,
                                [:fcall, :b],
                                [:dasgn, :x],
                                [:fcall, :puts, [:array, [:dvar, :x]]]]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :a, s(:arglist)),
                                s(:lasgn, :x),
                                s(:iter,
                                  s(:call, nil, :b, s(:arglist)),
                                  s(:lasgn, :x),
                                  s(:call, nil, :puts,
                                    s(:arglist, s(:lvar, :x))))),
            "Ruby2Ruby"    => "a { |x| b { |x| puts(x) } }")

  add_tests("iter_upto",
            "Ruby"         => "1.upto(3) { |n| puts(n.to_s) }",
            "RawParseTree" => [:iter,
                               [:call, [:lit, 1], :upto, [:array, [:lit, 3]]],
                               [:dasgn_curr, :n],
                               [:fcall, :puts,
                                [:array, [:call, [:dvar, :n], :to_s]]]],
            "ParseTree"    => s(:iter,
                                s(:call, s(:lit, 1), :upto,
                                  s(:arglist, s(:lit, 3))),
                                s(:lasgn, :n),
                                s(:call, nil, :puts,
                                  s(:arglist,
                                    s(:call, s(:lvar, :n), :to_s,
                                      s(:arglist))))))

  add_tests("iter_while",
            "Ruby"         => "argl = 10\nwhile (argl >= 1) do\n  puts(\"hello\")\n  argl = (argl - 1)\nend\n",
            "RawParseTree" => [:block,
                               [:lasgn, :argl, [:lit, 10]],
                               [:while,
                                [:call, [:lvar, :argl], :">=",
                                 [:array, [:lit, 1]]],
                                [:block,
                                 [:fcall, :puts, [:array, [:str, "hello"]]],
                                 [:lasgn,
                                  :argl,
                                  [:call, [:lvar, :argl],
                                   :"-", [:array, [:lit, 1]]]]], true]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :argl, s(:lit, 10)),
                                s(:while,
                                  s(:call, s(:lvar, :argl), :">=",
                                    s(:arglist, s(:lit, 1))),
                                  s(:block,
                                    s(:call, nil, :puts,
                                      s(:arglist, s(:str, "hello"))),
                                    s(:lasgn,
                                      :argl,
                                      s(:call, s(:lvar, :argl), :"-",
                                        s(:arglist, s(:lit, 1))))), true)))

  add_tests("ivar",
            "Ruby"         => [Examples, :reader],
            "RawParseTree" => [:defn, :reader, [:ivar, :@reader]],
            "ParseTree"    => s(:defn, :reader, # FIX should be unified?
                                s(:args),
                                s(:ivar, :@reader)),
            "Ruby2Ruby"    => "attr_reader :reader")

  add_tests("lasgn_array",
            "Ruby"         => "var = [\"foo\", \"bar\"]",
            "RawParseTree" => [:lasgn, :var,
                               [:array,
                                [:str, "foo"],
                                [:str, "bar"]]],
            "ParseTree"    => s(:lasgn, :var,
                                s(:array,
                                  s(:str, "foo"),
                                  s(:str, "bar"))))

  add_tests("lasgn_call",
            "Ruby"         => "c = (2 + 3)",
            "RawParseTree" => [:lasgn, :c, [:call, [:lit, 2], :+,
                                            [:array, [:lit, 3]]]],
            "ParseTree"    => s(:lasgn, :c, s(:call, s(:lit, 2), :+,
                                              s(:arglist, s(:lit, 3)))))

  add_tests("lit_bool_false",
            "Ruby"         => "false",
            "RawParseTree" => [:false],
            "ParseTree"    => s(:false))

  add_tests("lit_bool_true",
            "Ruby"         => "true",
            "RawParseTree" => [:true],
            "ParseTree"    => s(:true))

  add_tests("lit_float",
            "Ruby"         => "1.1",
            "RawParseTree" => [:lit, 1.1],
            "ParseTree"    => s(:lit, 1.1))

  add_tests("lit_long",
            "Ruby"         => "1",
            "RawParseTree" => [:lit, 1],
            "ParseTree"    => s(:lit, 1))

  add_tests("lit_long_negative",
            "Ruby"         => "-1",
            "RawParseTree" => [:lit, -1],
            "ParseTree"    => s(:lit, -1))

  add_tests("lit_range2",
            "Ruby"         => "(1..10)",
            "RawParseTree" => [:lit, 1..10],
            "ParseTree"    => s(:lit, 1..10))

  add_tests("lit_range3",
            "Ruby"         => "(1...10)",
            "RawParseTree" => [:lit, 1...10],
            "ParseTree"    => s(:lit, 1...10))

# TODO: discuss and decide which lit we like
#   it "converts a regexp to an sexp" do
#     "/blah/".to_sexp.should == s(:regex, "blah", 0)
#     "/blah/i".to_sexp.should == s(:regex, "blah", 1)
#     "/blah/u".to_sexp.should == s(:regex, "blah", 64)
#   end

  add_tests("lit_regexp",
            "Ruby"         => "/x/",
            "RawParseTree" => [:lit, /x/],
            "ParseTree"    => s(:lit, /x/))

  add_tests("lit_regexp_i_wwtt",
            "Ruby"         => 'str.split(//i)',
            "RawParseTree" => [:call, [:vcall, :str], :split,
                               [:array, [:lit, //i]]],
            "ParseTree"    => s(:call, s(:call, nil, :str, s(:arglist)), :split,
                                s(:arglist, s(:lit, //i))))

  add_tests("lit_regexp_n",
            "Ruby"         => "/x/n", # HACK differs on 1.9 - this is easiest
            "RawParseTree" => [:lit, /x/n],
            "ParseTree"    => s(:lit, /x/n),
            "Ruby2Ruby"    => /x/n.inspect)

  add_tests("lit_regexp_once",
            "Ruby"         => "/x/o",
            "RawParseTree" => [:lit, /x/],
            "ParseTree"    => s(:lit, /x/),
            "Ruby2Ruby"    => "/x/")

  add_tests("lit_sym",
            "Ruby"         => ":x",
            "RawParseTree" => [:lit, :x],
            "ParseTree"    => s(:lit, :x))

  add_tests("lit_sym_splat",
            "Ruby"         => ":\"*args\"",
            "RawParseTree" => [:lit, :"*args"],
            "ParseTree"    => s(:lit, :"*args"))

  add_tests("lvar_def_boundary",
            "Ruby"         => "b = 42\ndef a\n  c do\n    begin\n      do_stuff\n    rescue RuntimeError => b\n      puts(b)\n    end\n  end\nend\n",
            "RawParseTree" => [:block,
                               [:lasgn, :b, [:lit, 42]],
                               [:defn, :a,
                                [:scope,
                                 [:block,
                                  [:args],
                                  [:iter,
                                   [:fcall, :c],
                                   nil,
                                   [:begin,
                                    [:rescue,
                                     [:vcall, :do_stuff],
                                     [:resbody,
                                      [:array, [:const, :RuntimeError]],
                                      [:block,
                                       [:dasgn_curr, :b, [:gvar, :$!]],
                                       [:fcall, :puts,
                                        [:array, [:dvar, :b]]]]]]]]]]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :b, s(:lit, 42)),
                                s(:defn, :a,
                                  s(:args),
                                  s(:scope,
                                    s(:block,
                                      s(:iter,
                                        s(:call, nil, :c, s(:arglist)),
                                        nil,
                                        s(:rescue,
                                          s(:call, nil, :do_stuff, s(:arglist)),
                                          s(:resbody,
                                            s(:array,
                                              s(:const, :RuntimeError),
                                              s(:lasgn, :b, s(:gvar, :$!))),
                                            s(:call, nil, :puts,
                                              s(:arglist,
                                                s(:lvar, :b)))))))))))

  add_tests("masgn",
            "Ruby"         => "a, b = c, d",
            "RawParseTree" => [:masgn,
                               [:array, [:lasgn, :a], [:lasgn, :b]], nil,
                               [:array,  [:vcall, :c], [:vcall, :d]]],
            "ParseTree"    => s(:masgn,
                                s(:array, s(:lasgn, :a), s(:lasgn, :b)),
                                s(:array, s(:call, nil, :c, s(:arglist)),
                                  s(:call, nil, :d, s(:arglist)))))

  add_tests("masgn_argscat",
            "Ruby"         => "a, b, *c = 1, 2, *[3, 4]",
            "RawParseTree" => [:masgn,
                               [:array, [:lasgn, :a], [:lasgn, :b]],
                               [:lasgn, :c],
                               [:argscat,
                                [:array, [:lit, 1], [:lit, 2]],
                                [:array, [:lit, 3], [:lit, 4]]]],
            "ParseTree"    => s(:masgn,
                                s(:array,
                                  s(:lasgn, :a),
                                  s(:lasgn, :b),
                                  s(:splat, s(:lasgn, :c))),
                                s(:array,
                                  s(:lit, 1), s(:lit, 2),
                                  s(:splat,
                                    s(:array, s(:lit, 3), s(:lit, 4))))))

  add_tests("masgn_attrasgn",
            "Ruby"         => "a, b.c = d, e",
            "RawParseTree" => [:masgn,
                               [:array, [:lasgn, :a],
                                [:attrasgn, [:vcall, :b], :c=]], nil,
                               [:array, [:vcall, :d], [:vcall, :e]]],
            "ParseTree"    => s(:masgn,
                                s(:array,
                                  s(:lasgn, :a),
                                  s(:attrasgn,
                                    s(:call, nil, :b, s(:arglist)),
                                    :c=, s(:arglist))),
                                s(:array,
                                  s(:call, nil, :d, s(:arglist)),
                                  s(:call, nil, :e, s(:arglist)))))

  add_tests("masgn_attrasgn_array_rhs",
            "Ruby"         => "a.b, a.c, _ = q",
            "RawParseTree" => [:masgn,
                                [:array,
                                  [:attrasgn, [:vcall, :a], :b=],
                                  [:attrasgn, [:vcall, :a], :c=],
                                  [:lasgn, :_]], nil,
                                [:to_ary, [:vcall, :q]]],
            "ParseTree"    => s(:masgn,
                                s(:array,
                                  s(:attrasgn,
                                    s(:call, nil, :a, s(:arglist)),
                                    :b=, s(:arglist)),
                                  s(:attrasgn,
                                    s(:call, nil, :a, s(:arglist)),
                                    :c=, s(:arglist)),
                                  s(:lasgn, :_)),
                                s(:to_ary,
                                  s(:call, nil, :q, s(:arglist)))))

  add_tests("masgn_attrasgn_idx",
            "Ruby"         => "a, i, j = [], 1, 2\na[i], a[j] = a[j], a[i]\n",
            "RawParseTree" => [:block,
                               [:masgn,
                                [:array,
                                 [:lasgn, :a], [:lasgn, :i], [:lasgn, :j]], nil,
                                [:array, [:zarray], [:lit, 1], [:lit, 2]]],
                               [:masgn,
                                [:array,
                                 [:attrasgn,
                                  [:lvar, :a], :[]=, [:array, [:lvar, :i]]],
                                 [:attrasgn,
                                  [:lvar, :a], :[]=, [:array, [:lvar, :j]]]],
                                nil,
                                [:array,
                                 [:call, [:lvar, :a], :[],
                                  [:array, [:lvar, :j]]],
                                 [:call, [:lvar, :a], :[],
                                  [:array, [:lvar, :i]]]]]],
            "ParseTree"    => s(:block,
                                s(:masgn,
                                  s(:array,
                                    s(:lasgn, :a),
                                    s(:lasgn, :i), s(:lasgn, :j)),
                                  s(:array, s(:array), s(:lit, 1), s(:lit, 2))),
                                s(:masgn,
                                  s(:array,
                                    s(:attrasgn, s(:lvar, :a), :[]=,
                                      s(:arglist, s(:lvar, :i))),
                                    s(:attrasgn, s(:lvar, :a), :[]=,
                                      s(:arglist, s(:lvar, :j)))),
                                  s(:array,
                                    s(:call, s(:lvar, :a), :[],
                                      s(:arglist, s(:lvar, :j))),
                                    s(:call, s(:lvar, :a), :[],
                                      s(:arglist, s(:lvar, :i)))))))

  add_tests("masgn_cdecl",
            "Ruby"         => "A, B, C = 1, 2, 3",
            "RawParseTree" => [:masgn,
                               [:array, [:cdecl, :A], [:cdecl, :B],
                                [:cdecl, :C]], nil,
                               [:array, [:lit, 1], [:lit, 2], [:lit, 3]]],
            "ParseTree"    => s(:masgn,
                               s(:array, s(:cdecl, :A), s(:cdecl, :B),
                                s(:cdecl, :C)),
                               s(:array, s(:lit, 1), s(:lit, 2), s(:lit, 3))))


  add_tests("masgn_iasgn",
            "Ruby"         => "a, @b = c, d",
            "RawParseTree" => [:masgn,
                               [:array, [:lasgn, :a], [:iasgn, :"@b"]], nil,
                               [:array,  [:vcall, :c], [:vcall, :d]]],
            "ParseTree"    => s(:masgn,
                                s(:array, s(:lasgn, :a), s(:iasgn, :"@b")),
                                s(:array,
                                  s(:call, nil, :c, s(:arglist)),
                                  s(:call, nil, :d, s(:arglist)))))

  add_tests("masgn_masgn",
            "Ruby"         => "a, (b, c) = [1, [2, 3]]",
            "RawParseTree" => [:masgn,
                               [:array,
                                [:lasgn, :a],
                                [:masgn,
                                 [:array,
                                  [:lasgn, :b],
                                  [:lasgn, :c]], nil, nil]],
                               nil,
                               [:to_ary,
                                [:array,
                                 [:lit, 1],
                                 [:array,
                                  [:lit, 2],
                                  [:lit, 3]]]]],
            "ParseTree"    => s(:masgn,
                                s(:array,
                                  s(:lasgn, :a),
                                  s(:masgn,
                                    s(:array,
                                      s(:lasgn, :b),
                                      s(:lasgn, :c)))),
                                s(:to_ary,
                                  s(:array,
                                    s(:lit, 1),
                                    s(:array,
                                      s(:lit, 2),
                                      s(:lit, 3))))))

  add_tests("masgn_splat",
            "Ruby"         => "a, b, *c = d, e, f, g",
            "RawParseTree" => [:masgn,
                               [:array, [:lasgn, :a], [:lasgn, :b]],
                               [:lasgn, :c],
                               [:array,
                                [:vcall, :d], [:vcall, :e],
                                [:vcall, :f], [:vcall, :g]]],
            "ParseTree"    => s(:masgn,
                                s(:array,
                                  s(:lasgn, :a),
                                  s(:lasgn, :b),
                                  s(:splat, s(:lasgn, :c))),
                                s(:array,
                                  s(:call, nil, :d, s(:arglist)),
                                  s(:call, nil, :e, s(:arglist)),
                                  s(:call, nil, :f, s(:arglist)),
                                  s(:call, nil, :g, s(:arglist)))))

  add_tests("masgn_splat_no_name_to_ary",
            "Ruby"         => "a, b, * = c",
            "RawParseTree" => [:masgn,
                               [:array, [:lasgn, :a], [:lasgn, :b]],
                               [:splat],
                               [:to_ary, [:vcall, :c]]],
            "ParseTree"    => s(:masgn,
                                s(:array,
                                  s(:lasgn, :a),
                                  s(:lasgn, :b),
                                  s(:splat)),
                                s(:to_ary, s(:call, nil, :c, s(:arglist)))))

  add_tests("masgn_splat_no_name_trailing",
            "Ruby"         => "a, b, = c",
            "RawParseTree" => [:masgn,
                               [:array, [:lasgn, :a], [:lasgn, :b]], nil,
                               [:to_ary, [:vcall, :c]]],
            "ParseTree"    => s(:masgn,
                                s(:array, s(:lasgn, :a), s(:lasgn, :b)),
                                s(:to_ary, s(:call, nil, :c, s(:arglist)))),
            "Ruby2Ruby"    => "a, b = c") # TODO: check this is right

  add_tests("masgn_splat_to_ary",
            "Ruby"         => "a, b, *c = d",
            "RawParseTree" => [:masgn,
                               [:array, [:lasgn, :a], [:lasgn, :b]],
                               [:lasgn, :c],
                               [:to_ary, [:vcall, :d]]],
            "ParseTree"    => s(:masgn,
                                s(:array,
                                  s(:lasgn, :a),
                                  s(:lasgn, :b),
                                  s(:splat, s(:lasgn, :c))),
                                s(:to_ary, s(:call, nil, :d, s(:arglist)))))

  add_tests("masgn_splat_to_ary2",
            "Ruby"         => "a, b, *c = d.e(\"f\")",
            "RawParseTree" => [:masgn,
                               [:array, [:lasgn, :a], [:lasgn, :b]],
                               [:lasgn, :c],
                               [:to_ary,
                                [:call, [:vcall, :d], :e,
                                 [:array, [:str, 'f']]]]],
            "ParseTree"    => s(:masgn,
                                s(:array,
                                  s(:lasgn, :a),
                                  s(:lasgn, :b),
                                  s(:splat, s(:lasgn, :c))),
                                s(:to_ary,
                                  s(:call,
                                    s(:call, nil, :d, s(:arglist)),
                                    :e,
                                    s(:arglist, s(:str, 'f'))))))

  add_tests("match",
            "Ruby"         => "1 if /x/",
            "RawParseTree" => [:if, [:match, [:lit, /x/]], [:lit, 1], nil],
            "ParseTree"    => s(:if, s(:match, s(:lit, /x/)), s(:lit, 1), nil))

  add_tests("match2",
            "Ruby"         => "/x/ =~ \"blah\"",
            "RawParseTree" => [:match2, [:lit, /x/], [:str, "blah"]],
            "ParseTree"    => s(:match2, s(:lit, /x/), s(:str, "blah")))

  add_tests("match3",
            "Ruby"         => "\"blah\" =~ /x/",
            "RawParseTree" => [:match3, [:lit, /x/], [:str, "blah"]],
            "ParseTree"    => s(:match3, s(:lit, /x/), s(:str, "blah")))

  add_tests("module",
            "Ruby"         => "module X\n  def y\n    # do nothing\n  end\nend",
            "RawParseTree" => [:module, :X,
                               [:scope,
                                [:defn, :y,
                                 [:scope, [:block, [:args], [:nil]]]]]],
            "ParseTree"    => s(:module, :X,
                                s(:scope,
                                  s(:defn, :y,
                                    s(:args),
                                    s(:scope, s(:block, s(:nil)))))))

  add_tests("module_scoped",
            "Ruby"         => "module X::Y\n  c\nend",
            "RawParseTree" => [:module, [:colon2, [:const, :X], :Y],
                               [:scope, [:vcall, :c]]],
            "ParseTree"    => s(:module, s(:colon2, s(:const, :X), :Y),
                                s(:scope, s(:call, nil, :c, s(:arglist)))))

  add_tests("module_scoped3",
            "Ruby"         => "module ::Y\n  c\nend",
            "RawParseTree" => [:module, [:colon3, :Y], [:scope, [:vcall, :c]]],
            "ParseTree"    => s(:module,
                                s(:colon3, :Y),
                                s(:scope, s(:call, nil, :c, s(:arglist)))))

  add_tests("next",
            "Ruby"         => "loop { next if false }",
            "RawParseTree" => [:iter,
                               [:fcall, :loop],
                               nil,
                               [:if, [:false], [:next], nil]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :loop, s(:arglist)),
                                nil,
                                s(:if, s(:false), s(:next), nil)))

  add_tests("next_arg",
            "Ruby"         => "loop { next 42 if false }",
            "RawParseTree" => [:iter,
                               [:fcall, :loop],
                               nil,
                               [:if, [:false], [:next, [:lit, 42]], nil]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :loop, s(:arglist)),
                                nil,
                                s(:if, s(:false), s(:next, s(:lit, 42)), nil)))

  add_tests("not",
            "Ruby"         => "(not true)",
            "RawParseTree" => [:not, [:true]],
            "ParseTree"    => s(:not, s(:true)))

  add_tests("nth_ref",
            "Ruby"         => "$1",
            "RawParseTree" => [:nth_ref, 1],
            "ParseTree"    => s(:nth_ref, 1))

  add_tests("op_asgn1",
            "Ruby"         => "b = []\nb[1] ||= 10\nb[2] &&= 11\nb[3] += 12\n",
            "RawParseTree" => [:block,
                               [:lasgn, :b, [:zarray]],
                               [:op_asgn1, [:lvar, :b],
                                [:array, [:lit, 1]], :"||", [:lit, 10]],
                               [:op_asgn1, [:lvar, :b],
                                [:array, [:lit, 2]], :"&&", [:lit, 11]],
                               [:op_asgn1, [:lvar, :b],
                                [:array, [:lit, 3]], :+, [:lit, 12]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :b, s(:array)),
                                s(:op_asgn1, s(:lvar, :b),
                                  s(:arglist, s(:lit, 1)), :"||", s(:lit, 10)),
                                s(:op_asgn1, s(:lvar, :b),
                                  s(:arglist, s(:lit, 2)), :"&&", s(:lit, 11)),
                                s(:op_asgn1, s(:lvar, :b),
                                  s(:arglist, s(:lit, 3)), :+, s(:lit, 12))))

  add_tests("op_asgn1_ivar",
            "Ruby"         => "@b = []\n@b[1] ||= 10\n@b[2] &&= 11\n@b[3] += 12\n",
            "RawParseTree" => [:block,
                               [:iasgn, :@b, [:zarray]],
                               [:op_asgn1, [:ivar, :@b],
                                [:array, [:lit, 1]], :"||", [:lit, 10]],
                               [:op_asgn1, [:ivar, :@b],
                                [:array, [:lit, 2]], :"&&", [:lit, 11]],
                               [:op_asgn1, [:ivar, :@b],
                                [:array, [:lit, 3]], :+, [:lit, 12]]],
            "ParseTree"    => s(:block,
                                s(:iasgn, :@b, s(:array)),
                                s(:op_asgn1, s(:ivar, :@b),
                                  s(:arglist, s(:lit, 1)), :"||", s(:lit, 10)),
                                s(:op_asgn1, s(:ivar, :@b),
                                  s(:arglist, s(:lit, 2)), :"&&", s(:lit, 11)),
                                s(:op_asgn1, s(:ivar, :@b),
                                  s(:arglist, s(:lit, 3)), :+, s(:lit, 12))))

  add_tests("op_asgn2",
            "Ruby"         => "s = Struct.new(:var)\nc = s.new(nil)\nc.var ||= 20\nc.var &&= 21\nc.var += 22\nc.d.e.f ||= 42\n",
            "RawParseTree" => [:block,
                               [:lasgn, :s,
                                [:call, [:const, :Struct],
                                 :new, [:array, [:lit, :var]]]],
                               [:lasgn, :c,
                                [:call, [:lvar, :s], :new, [:array, [:nil]]]],

                               [:op_asgn2, [:lvar, :c], :var=, :"||",
                                [:lit, 20]],
                               [:op_asgn2, [:lvar, :c], :var=, :"&&",
                                [:lit, 21]],
                               [:op_asgn2, [:lvar, :c], :var=, :+, [:lit, 22]],

                               [:op_asgn2,
                                [:call,
                                 [:call, [:lvar, :c], :d], :e], :f=, :"||",
                                [:lit, 42]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :s,
                                  s(:call, s(:const, :Struct),
                                    :new, s(:arglist, s(:lit, :var)))),
                                s(:lasgn, :c,
                                  s(:call, s(:lvar, :s),
                                    :new, s(:arglist, s(:nil)))),
                                s(:op_asgn2, s(:lvar, :c),
                                  :var=, :"||", s(:lit, 20)),
                                s(:op_asgn2, s(:lvar, :c),
                                  :var=, :"&&", s(:lit, 21)),
                                s(:op_asgn2, s(:lvar, :c),
                                  :var=, :+, s(:lit, 22)),
                                s(:op_asgn2,
                                  s(:call,
                                    s(:call, s(:lvar, :c), :d, s(:arglist)),
                                    :e, s(:arglist)),
                                  :f=, :"||", s(:lit, 42))))

  add_tests("op_asgn2_self",
            "Ruby"         => "self.Bag ||= Bag.new",
            "RawParseTree" => [:op_asgn2, [:self], :"Bag=", :"||",
                               [:call, [:const, :Bag], :new]],
            "ParseTree"    => s(:op_asgn2, s(:self), :"Bag=", :"||",
                                s(:call, s(:const, :Bag), :new, s(:arglist))))

  add_tests("op_asgn_and",
            "Ruby"         => "a = 0\na &&= 2\n",
            "RawParseTree" => [:block,
                               [:lasgn, :a, [:lit, 0]],
                               [:op_asgn_and,
                                [:lvar, :a], [:lasgn, :a, [:lit, 2]]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :a, s(:lit, 0)),
                                s(:op_asgn_and,
                                  s(:lvar, :a), s(:lasgn, :a, s(:lit, 2)))))

  add_tests("op_asgn_and_ivar2",
            "Ruby"         => "@fetcher &&= new(Gem.configuration[:http_proxy])",
            "RawParseTree" => [:op_asgn_and,
                               [:ivar, :@fetcher],
                               [:iasgn,
                                :@fetcher,
                                [:fcall,
                                 :new,
                                 [:array,
                                  [:call,
                                   [:call, [:const, :Gem], :configuration],
                                   :[],
                                   [:array, [:lit, :http_proxy]]]]]]],
            "ParseTree"    => s(:op_asgn_and,
                                s(:ivar, :@fetcher),
                                s(:iasgn,
                                  :@fetcher,
                                  s(:call, nil,
                                    :new,
                                    s(:arglist,
                                      s(:call,
                                        s(:call, s(:const, :Gem),
                                          :configuration,
                                          s(:arglist)),
                                        :[],
                                        s(:arglist, s(:lit, :http_proxy))))))))

  add_tests("op_asgn_or",
            "Ruby"         => "a = 0\na ||= 1\n",
            "RawParseTree" => [:block,
                               [:lasgn, :a, [:lit, 0]],
                               [:op_asgn_or,
                                [:lvar, :a], [:lasgn, :a, [:lit, 1]]]],
            "ParseTree"    => s(:block,
                                s(:lasgn, :a, s(:lit, 0)),
                                s(:op_asgn_or,
                                  s(:lvar, :a), s(:lasgn, :a, s(:lit, 1)))))

  add_tests("op_asgn_or_block",
            "Ruby"         => "a ||= begin\n        b\n      rescue\n        c\n      end",
            "RawParseTree" => [:op_asgn_or,
                               [:lvar, :a],
                               [:lasgn, :a,
                                [:rescue,
                                 [:vcall, :b],
                                 [:resbody, nil, [:vcall, :c]]]]],
            "ParseTree"    => s(:op_asgn_or,
                                s(:lvar, :a),
                                s(:lasgn, :a,
                                  s(:rescue,
                                    s(:call, nil, :b, s(:arglist)),
                                    s(:resbody, s(:array),
                                      s(:call, nil, :c, s(:arglist)))))),
            "Ruby2Ruby"    => "a ||= b rescue c")

  add_tests("op_asgn_or_ivar",
            "Ruby"         => "@v ||= {  }",
            "RawParseTree" => [:op_asgn_or,
                               [:ivar, :@v],
                               [:iasgn, :@v, [:hash]]],
            "ParseTree"    => s(:op_asgn_or,
                                s(:ivar, :@v),
                                s(:iasgn, :@v, s(:hash))))

  add_tests("op_asgn_or_ivar2",
            "Ruby"         => "@fetcher ||= new(Gem.configuration[:http_proxy])",
            "RawParseTree" => [:op_asgn_or,
                               [:ivar, :@fetcher],
                               [:iasgn,
                                :@fetcher,
                                [:fcall,
                                 :new,
                                 [:array,
                                  [:call,
                                   [:call, [:const, :Gem], :configuration],
                                   :[],
                                   [:array, [:lit, :http_proxy]]]]]]],
            "ParseTree"    => s(:op_asgn_or,
                                s(:ivar, :@fetcher),
                                s(:iasgn,
                                  :@fetcher,
                                  s(:call, nil, :new,
                                    s(:arglist,
                                      s(:call,
                                        s(:call, s(:const, :Gem),
                                          :configuration,
                                          s(:arglist)),
                                        :[],
                                        s(:arglist, s(:lit, :http_proxy))))))))

  add_tests("or",
            "Ruby"         => "(a or b)",
            "RawParseTree" => [:or, [:vcall, :a], [:vcall, :b]],
            "ParseTree"    => s(:or,
                                s(:call, nil, :a, s(:arglist)),
                                s(:call, nil, :b, s(:arglist))))

  add_tests("or_big",
            "Ruby"         => "((a or b) or (c and d))",
            "RawParseTree" => [:or,
                               [:or,  [:vcall, :a], [:vcall, :b]],
                               [:and, [:vcall, :c], [:vcall, :d]]],
            "ParseTree"    => s(:or,
                                s(:or,
                                  s(:call, nil, :a, s(:arglist)),
                                  s(:call, nil, :b, s(:arglist))),
                                s(:and,
                                  s(:call, nil, :c, s(:arglist)),
                                  s(:call, nil, :d, s(:arglist)))))

  add_tests("or_big2",
            "Ruby"         => "((a || b) || (c && d))",
            "RawParseTree" => [:or,
                               [:or,  [:vcall, :a], [:vcall, :b]],
                               [:and, [:vcall, :c], [:vcall, :d]]],
            "ParseTree"    => s(:or,
                                s(:or,
                                  s(:call, nil, :a, s(:arglist)),
                                  s(:call, nil, :b, s(:arglist))),
                                s(:and,
                                  s(:call, nil, :c, s(:arglist)),
                                  s(:call, nil, :d, s(:arglist)))),
            "Ruby2Ruby"    => "((a or b) or (c and d))")

  add_tests("parse_floats_as_args",
            "Ruby"         => "def x(a=0.0,b=0.0)\n  a+b\nend",
            "RawParseTree" => [:defn, :x,
                               [:scope,
                                [:block,
                                 [:args, :a, :b,
                                  [:block,
                                   [:lasgn, :a, [:lit, 0.0]],
                                   [:lasgn, :b, [:lit, 0.0]]]],
                                 [:call, [:lvar, :a], :+,
                                  [:array, [:lvar, :b]]]]]],
            "ParseTree"    => s(:defn, :x,
                                s(:args, :a, :b,
                                  s(:block,
                                    s(:lasgn, :a, s(:lit, 0.0)),
                                    s(:lasgn, :b, s(:lit, 0.0)))),
                                s(:scope,
                                  s(:block,
                                    s(:call, s(:lvar, :a), :+,
                                      s(:arglist, s(:lvar, :b)))))),
            "Ruby2Ruby"    => "def x(a = 0.0, b = 0.0)\n  (a + b)\nend")

  add_tests("postexe",
            "Ruby"         => "END { 1 }",
            "RawParseTree" => [:iter, [:postexe], nil, [:lit, 1]],
            "ParseTree"    => s(:iter, s(:postexe), nil, s(:lit, 1)))

  add_tests("proc_args_0",
            "Ruby"         => "proc { || (x + 1) }",
            "RawParseTree" => [:iter,
                               [:fcall, :proc],
                               0,
                               [:call, [:vcall, :x], :+, [:array, [:lit, 1]]]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :proc, s(:arglist)),
                                0,
                                s(:call,
                                  s(:call, nil, :x, s(:arglist)),
                                  :+,
                                  s(:arglist, s(:lit, 1)))))

  add_tests("proc_args_1",
            "Ruby"         => "proc { |x| (x + 1) }",
            "RawParseTree" => [:iter,
                               [:fcall, :proc],
                               [:dasgn_curr, :x],
                               [:call, [:dvar, :x], :+, [:array, [:lit, 1]]]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :proc, s(:arglist)),
                                s(:lasgn, :x),
                                s(:call, s(:lvar, :x), :+,
                                  s(:arglist, s(:lit, 1)))))

  add_tests("proc_args_2",
            "Ruby"         => "proc { |x, y| (x + y) }",
            "RawParseTree" => [:iter,
                               [:fcall, :proc],
                               [:masgn, [:array,
                                         [:dasgn_curr, :x],
                                         [:dasgn_curr, :y]], nil, nil],
                               [:call, [:dvar, :x], :+, [:array, [:dvar, :y]]]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :proc, s(:arglist)),
                                s(:masgn,
                                  s(:array,
                                    s(:lasgn, :x),
                                    s(:lasgn, :y))),
                                s(:call, s(:lvar, :x), :+,
                                  s(:arglist, s(:lvar, :y)))))

  add_tests("proc_args_no",
            "Ruby"         => "proc { (x + 1) }",
            "RawParseTree" => [:iter,
                               [:fcall, :proc],
                               nil,
                               [:call, [:vcall, :x], :+, [:array, [:lit, 1]]]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :proc, s(:arglist)),
                                nil,
                                s(:call, s(:call, nil, :x, s(:arglist)),
                                  :+, s(:arglist, s(:lit, 1)))))

  add_tests("redo",
            "Ruby"         => "loop { redo if false }",
            "RawParseTree" => [:iter,
                               [:fcall, :loop], nil,
                               [:if, [:false], [:redo], nil]],
            "ParseTree"    => s(:iter,
                                s(:call, nil, :loop, s(:arglist)),
                                nil,
                                s(:if, s(:false), s(:redo), nil)))

  # TODO: need a resbody w/ multiple classes and a splat

  add_tests("rescue",
            "Ruby"         => "blah rescue nil",
            "RawParseTree" => [:rescue,
                               [:vcall, :blah], [:resbody, nil, [:nil]]],
            "ParseTree"    => s(:rescue,
                                s(:call, nil, :blah, s(:arglist)),
                                s(:resbody, s(:array), s(:nil))))

  add_tests("rescue_block_body",
            "Ruby"         => "begin\n  a\nrescue => e\n  c\n  d\nend",
            "RawParseTree" => [:begin,
                               [:rescue,
                                [:vcall, :a],
                                [:resbody, nil,
                                 [:block,
                                  [:lasgn, :e, [:gvar, :$!]],
                                  [:vcall, :c],
                                  [:vcall, :d]]]]],
            "ParseTree"    => s(:rescue,
                                s(:call, nil, :a, s(:arglist)),
                                s(:resbody,
                                  s(:array, s(:lasgn, :e, s(:gvar, :$!))),
                                  s(:block,
                                    s(:call, nil, :c, s(:arglist)),
                                    s(:call, nil, :d, s(:arglist))))))

  add_tests("rescue_block_body_3",
            "Ruby"         => "begin\n  a\nrescue A\n  b\nrescue B\n  c\nrescue C\n  d\nend",
            "RawParseTree" => [:begin,
                               [:rescue,
                                [:vcall, :a],
                                [:resbody, [:array, [:const, :A]],
                                 [:vcall, :b],
                                 [:resbody, [:array, [:const, :B]],
                                  [:vcall, :c],
                                  [:resbody, [:array, [:const, :C]],
                                   [:vcall, :d]]]]]],
            "ParseTree"    => s(:rescue,
                                s(:call, nil, :a, s(:arglist)),
                                s(:resbody, s(:array, s(:const, :A)),
                                  s(:call, nil, :b, s(:arglist))),
                                s(:resbody, s(:array, s(:const, :B)),
                                  s(:call, nil, :c, s(:arglist))),
                                s(:resbody, s(:array, s(:const, :C)),
                                  s(:call, nil, :d, s(:arglist)))))

  add_tests("rescue_block_body_ivar",
            "Ruby"         => "begin\n  a\nrescue => @e\n  c\n  d\nend",
            "RawParseTree" => [:begin,
                               [:rescue,
                                [:vcall, :a],
                                [:resbody, nil,
                                 [:block,
                                  [:iasgn, :@e, [:gvar, :$!]],
                                  [:vcall, :c],
                                  [:vcall, :d]]]]],
            "ParseTree"    => s(:rescue,
                                s(:call, nil, :a, s(:arglist)),
                                s(:resbody,
                                  s(:array, s(:iasgn, :@e, s(:gvar, :$!))),
                                  s(:block,
                                    s(:call, nil, :c, s(:arglist)),
                                    s(:call, nil, :d, s(:arglist))))))

  add_tests("rescue_block_nada",
            "Ruby"         => "begin\n  blah\nrescue\n  # do nothing\nend",
            "RawParseTree" => [:begin,
                               [:rescue, [:vcall, :blah], [:resbody, nil]]],
            "ParseTree"    => s(:rescue,
                                s(:call, nil, :blah, s(:arglist)),
                                s(:resbody, s(:array), nil)))

  add_tests("rescue_exceptions",
            "Ruby"         => "begin\n  blah\nrescue RuntimeError => r\n  # do nothing\nend",
            "RawParseTree" => [:begin,
                               [:rescue,
                                [:vcall, :blah],
                                [:resbody,
                                 [:array, [:const, :RuntimeError]],
                                 [:lasgn, :r, [:gvar, :$!]]]]],
            "ParseTree"    => s(:rescue,
                                s(:call, nil, :blah, s(:arglist)),
                                s(:resbody,
                                  s(:array,
                                    s(:const, :RuntimeError),
                                    s(:lasgn, :r, s(:gvar, :$!))),
                                  nil)))


  add_tests("rescue_iasgn_var_empty",
            "Ruby"         => "begin\n  1\nrescue => @e\n  # do nothing\nend",
            "RawParseTree" => [:begin,
                               [:rescue,
                                [:lit, 1],
                                [:resbody, nil, [:iasgn, :@e, [:gvar, :$!]]]]],
            "ParseTree"    => s(:rescue,
                                s(:lit, 1),
                                s(:resbody,
                                  s(:array, s(:iasgn, :@e, s(:gvar, :$!))),
                                  nil)))

  add_tests("rescue_lasgn",
            "Ruby"         => "begin\n  1\nrescue\n  var = 2\nend",
            "RawParseTree" => [:begin,
                               [:rescue,
                                [:lit, 1],
                                [:resbody, nil, [:lasgn, :var, [:lit, 2]]]]],
            "ParseTree"    => s(:rescue,
                                s(:lit, 1),
                                s(:resbody,
                                  s(:array),
                                  s(:lasgn, :var, s(:lit, 2)))),
            "Ruby2Ruby"    => "1 rescue var = 2")

  add_tests("rescue_lasgn_var",
            "Ruby"         => "begin\n  1\nrescue => e\n  var = 2\nend",
            "RawParseTree" => [:begin,
                               [:rescue,
                                [:lit, 1],
                                [:resbody, nil,
                                 [:block,
                                  [:lasgn, :e, [:gvar, :$!]],
                                  [:lasgn, :var, [:lit, 2]]]]]],
            "ParseTree"    => s(:rescue,
                                s(:lit, 1),
                                s(:resbody,
                                  s(:array, s(:lasgn, :e, s(:gvar, :$!))),
                                  s(:lasgn, :var, s(:lit, 2)))))

  add_tests("rescue_lasgn_var_empty",
            "Ruby"         => "begin\n  1\nrescue => e\n  # do nothing\nend",
            "RawParseTree" => [:begin,
                               [:rescue,
                                [:lit, 1],
                                [:resbody, nil, [:lasgn, :e, [:gvar, :$!]]]]],
            "ParseTree"    => s(:rescue,
                                s(:lit, 1),
                                s(:resbody,
                                  s(:array, s(:lasgn, :e, s(:gvar, :$!))),
                                  nil)))

  add_tests("retry",
            "Ruby"         => "retry",
            "RawParseTree" => [:retry],
            "ParseTree"    => s(:retry))

  add_tests("return_0",
            "Ruby"         => "return",
            "RawParseTree" => [:return],
            "ParseTree"    => s(:return))

  add_tests("return_1",
            "Ruby"         => "return 1",
            "RawParseTree" => [:return, [:lit, 1]],
            "ParseTree"    => s(:return, s(:lit, 1)))

  add_tests("return_1_splatted",
            "Ruby"         => "return *1",
            "RawParseTree" => [:return, [:svalue, [:splat, [:lit, 1]]]],
            "ParseTree"    => s(:return, s(:svalue, s(:splat, s(:lit, 1)))))

  add_tests("return_n",
            "Ruby"         => "return 1, 2, 3",
            "RawParseTree" => [:return, [:array,
                                         [:lit, 1], [:lit, 2], [:lit, 3]]],
            "ParseTree"    => s(:return, s(:array,
                                           s(:lit, 1), s(:lit, 2), s(:lit, 3))),
            "Ruby2Ruby"    => "return [1, 2, 3]")

  add_tests("sclass",
            "Ruby"         => "class << self\n  42\nend",
            "RawParseTree" => [:sclass, [:self], [:scope, [:lit, 42]]],
            "ParseTree"    => s(:sclass, s(:self), s(:scope, s(:lit, 42))))

  add_tests("sclass_trailing_class",
            "Ruby"         => "class A\n  class << self\n    a\n  end\n  class B\n  end\nend",
            "RawParseTree" => [:class, :A, nil,
                               [:scope,
                                [:block,
                                 [:sclass, [:self], [:scope, [:vcall, :a]]],
                                 [:class, :B, nil, [:scope]]]]],
            "ParseTree"    => s(:class, :A, nil,
                                s(:scope,
                                  s(:block,
                                    s(:sclass, s(:self),
                                      s(:scope,
                                        s(:call, nil, :a, s(:arglist)))),
                                    s(:class, :B, nil, s(:scope))))))

  add_tests("splat",
            "Ruby"         => "def x(*b)\n  a(*b)\nend",
            "RawParseTree" => [:defn, :x,
                               [:scope,
                                [:block,
                                 [:args, :"*b"],
                                 [:fcall, :a, [:splat, [:lvar, :b]]]]]],
            "ParseTree"    => s(:defn, :x,
                                s(:args, :"*b"),
                                s(:scope,
                                  s(:block,
                                    s(:call, nil, :a,
                                      s(:arglist, s(:splat, s(:lvar, :b))))))))

  add_tests("splat_array",
            "Ruby"         => "[*[1]]",
            "RawParseTree" => [:splat, [:array, [:lit, 1]]],
            "ParseTree"    => s(:array, s(:splat, s(:array, s(:lit, 1)))))

  add_tests("splat_break",
            "Ruby"         => "break *[1]",
            "RawParseTree" => [:break, [:svalue, [:splat, [:array, [:lit, 1]]]]],
            "ParseTree"    => s(:break, s(:svalue, s(:splat, s(:array, s(:lit, 1))))))

  add_tests("splat_break_array",
            "Ruby"         => "break [*[1]]",
            "RawParseTree" => [:break, [:splat, [:array, [:lit, 1]]]],
            "ParseTree"    => s(:break, s(:array, s(:splat, s(:array, s(:lit, 1))))))

  add_tests("splat_fcall",
            "Ruby"         => "meth(*[1])",
            "RawParseTree" => [:fcall, :meth,
                               [:splat, [:array, [:lit, 1]]]],
            "ParseTree"    => s(:call, nil, :meth,
                             s(:arglist, s(:splat, s(:array, s(:lit, 1))))))

  add_tests("splat_fcall_array",
            "Ruby"         => "meth([*[1]])",
            "RawParseTree" => [:fcall, :meth,
                               [:array, [:splat, [:array, [:lit, 1]]]]],
            "ParseTree"    => s(:call, nil, :meth,
                             s(:arglist,
                               s(:array, s(:splat, s(:array, s(:lit, 1)))))))

  add_tests("splat_lasgn",
            "Ruby"         => "x = *[1]",
            "RawParseTree" => [:lasgn, :x, [:svalue, [:splat, [:array, [:lit, 1]]]]],
            "ParseTree"    => s(:lasgn, :x, s(:svalue, s(:splat, s(:array, s(:lit, 1))))))

  add_tests("splat_lasgn_array",
            "Ruby"         => "x = [*[1]]",
            "RawParseTree" => [:lasgn, :x, [:splat, [:array, [:lit, 1]]]],
            "ParseTree"    => s(:lasgn, :x, s(:array, s(:splat, s(:array, s(:lit, 1))))))

  add_tests("splat_lit_1",
            "Ruby"         => "[*1]",
            "RawParseTree" => [:splat, [:lit, 1]], # UGH - damn MRI
            "ParseTree"    => s(:array, s(:splat, s(:lit, 1))))

  add_tests("splat_lit_n",
            "Ruby"         => "[1, *2]",
            "RawParseTree" => [:argscat, [:array, [:lit, 1]], [:lit, 2]],
            "ParseTree"    => s(:array, s(:lit, 1), s(:splat, s(:lit, 2))))

  add_tests("splat_next",
            "Ruby"         => "next *[1]",
            "RawParseTree" => [:next, [:svalue, [:splat, [:array, [:lit, 1]]]]],
            "ParseTree"    => s(:next, s(:svalue, s(:splat, s(:array, s(:lit, 1))))))

  add_tests("splat_next_array",
            "Ruby"         => "next [*[1]]",
            "RawParseTree" => [:next, [:splat, [:array, [:lit, 1]]]],
            "ParseTree"    => s(:next, s(:array, s(:splat, s(:array, s(:lit, 1))))))

  add_tests("splat_return",
            "Ruby"         => "return *[1]",
            "RawParseTree" => [:return, [:svalue, [:splat, [:array, [:lit, 1]]]]],
            "ParseTree"    => s(:return, s(:svalue, s(:splat, s(:array, s(:lit, 1))))))

  add_tests("splat_return_array",
            "Ruby"         => "return [*[1]]",
            "RawParseTree" => [:return, [:splat, [:array, [:lit, 1]]]],
            "ParseTree"    => s(:return, s(:array, s(:splat, s(:array, s(:lit, 1))))))

  add_tests("splat_super",
            "Ruby"         => "super(*[1])",
            "RawParseTree" => [:super, [:splat, [:array, [:lit, 1]]]],
            "ParseTree"    => s(:super, s(:splat, s(:array, s(:lit, 1)))))

  add_tests("splat_super_array",
            "Ruby"         => "super([*[1]])",
            "RawParseTree" => [:super, [:array, [:splat, [:array, [:lit, 1]]]]],
            "ParseTree"    => s(:super, s(:array, s(:splat, s(:array, s(:lit, 1))))))

  add_tests("splat_yield",
            "Ruby"         => "yield(*[1])",
            "RawParseTree" => [:yield, [:splat, [:array, [:lit, 1]]]],
            "ParseTree"    => s(:yield, s(:splat, s(:array, s(:lit, 1)))))

  add_tests("splat_yield_array",
            "Ruby"         => "yield([*[1]])",
            "RawParseTree" => [:yield, [:splat, [:array, [:lit, 1]]], true],
            "ParseTree"    => s(:yield, s(:array, s(:splat, s(:array, s(:lit, 1))))))

  add_tests("str",
            "Ruby"         => '"x"',
            "RawParseTree" => [:str, "x"],
            "ParseTree"    => s(:str, "x"))

  add_tests("str_concat_newline", # FIX? make prettier? possible?
            "Ruby"         => '"before" \\
  " after"',
            "RawParseTree" => [:str, "before after"],
            "ParseTree"    => s(:str, "before after"),
            "Ruby2Ruby"    => '"before after"')

  add_tests("str_concat_space",
            "Ruby"         => '"before" " after"',
            "RawParseTree" => [:str, "before after"],
            "ParseTree"    => s(:str, "before after"),
            "Ruby2Ruby"    => '"before after"')

  add_tests("str_heredoc",
            "Ruby"         => "<<'EOM'\n  blah\nblah\nEOM",
            "RawParseTree" => [:str, "  blah\nblah\n"],
            "ParseTree"    => s(:str, "  blah\nblah\n"),
            "Ruby2Ruby"    => "\"  blah\\nblah\\n\"")

  add_tests("str_heredoc_call",
            "Ruby"         => "<<'EOM'.strip\n  blah\nblah\nEOM",
            "RawParseTree" => [:call, [:str, "  blah\nblah\n"], :strip],
            "ParseTree"    => s(:call, s(:str, "  blah\nblah\n"),
                                :strip, s(:arglist)),
            "Ruby2Ruby"    => "\"  blah\\nblah\\n\".strip")

  add_tests("str_heredoc_double",
            "Ruby"         => "a += <<-H1 + b + <<-H2\n  first\nH1\n  second\nH2",
            "RawParseTree" => [:lasgn, :a,
                               [:call,
                                [:lvar, :a],
                                :+,
                                [:array,
                                 [:call,
                                  [:call, [:str, "  first\n"], :+,
                                   [:array, [:vcall, :b]]],
                                  :+,
                                  [:array, [:str, "  second\n"]]]]]],
            "ParseTree"    => s(:lasgn, :a,
                                s(:call,
                                  s(:lvar, :a),
                                  :+,
                                  s(:arglist,
                                    s(:call,
                                      s(:call, s(:str, "  first\n"), :+,
                                        s(:arglist,
                                          s(:call, nil, :b, s(:arglist)))),
                                      :+,
                                      s(:arglist, s(:str, "  second\n")))))),
            "Ruby2Ruby"    => "a = (a + ((\"  first\\n\" + b) + \"  second\\n\"))")

  add_tests("str_heredoc_indent",
            "Ruby"         => "<<-EOM\n  blah\nblah\n\n  EOM",
            "RawParseTree" => [:str, "  blah\nblah\n\n"],
            "ParseTree"    => s(:str, "  blah\nblah\n\n"),
            "Ruby2Ruby"    => "\"  blah\\nblah\\n\\n\"")

  add_tests("str_interp_file",
            "Ruby"         => '"file = #{__FILE__}\n"',
            "RawParseTree" => [:str, "file = (string)\n"],
            "ParseTree"    => s(:str, "file = (string)\n"),
            "Ruby2Ruby"    => '"file = (string)\\n"')

  add_tests("structure_extra_block_for_dvar_scoping",
            "Ruby"         => "a.b do |c, d|\n  unless e.f(c) then\n    g = false\n    d.h { |x, i| g = true }\n  end\nend",
            "RawParseTree" => [:iter,
                               [:call, [:vcall, :a], :b],
                               [:masgn, [:array,
                                         [:dasgn_curr, :c],
                                         [:dasgn_curr, :d]], nil, nil],
                               [:if,
                                [:call, [:vcall, :e], :f,
                                 [:array, [:dvar, :c]]],
                                nil,
                                [:block,
                                 [:dasgn_curr, :g, [:false]],
                                 [:iter,
                                  [:call, [:dvar, :d], :h],
                                  [:masgn, [:array,
                                            [:dasgn_curr, :x],
                                            [:dasgn_curr, :i]], nil, nil],
                                  [:dasgn, :g, [:true]]]]]],
            "ParseTree"    => s(:iter,
                                s(:call,
                                  s(:call, nil, :a, s(:arglist)),
                                  :b, s(:arglist)),
                                s(:masgn, s(:array,
                                            s(:lasgn, :c),
                                            s(:lasgn, :d))),
                                s(:if,
                                  s(:call, s(:call, nil, :e, s(:arglist)), :f,
                                    s(:arglist, s(:lvar, :c))),
                                  nil,
                                  s(:block,
                                    s(:lasgn, :g, s(:false)),
                                    s(:iter,
                                      s(:call, s(:lvar, :d), :h, s(:arglist)),
                                      s(:masgn, s(:array,
                                                  s(:lasgn, :x),
                                                  s(:lasgn, :i))),
                                      s(:lasgn, :g, s(:true)))))))

  add_tests("structure_remove_begin_1",
            "Ruby"         => "a << begin\n       b\n     rescue\n       c\n     end",
            "RawParseTree" => [:call, [:vcall, :a], :<<,
                               [:array, [:rescue, [:vcall, :b],
                                         [:resbody, nil, [:vcall, :c]]]]],
            "ParseTree"    => s(:call, s(:call, nil, :a, s(:arglist)), :<<,
                                s(:arglist,
                                  s(:rescue,
                                    s(:call, nil, :b, s(:arglist)),
                                    s(:resbody, s(:array),
                                      s(:call, nil, :c, s(:arglist)))))),
            "Ruby2Ruby"    => "(a << b rescue c)")

  add_tests("structure_remove_begin_2",
            "Ruby"         => "a = if c\n      begin\n        b\n      rescue\n        nil\n      end\n    end\na",
            "RawParseTree" => [:block,
                               [:lasgn,
                                :a,
                                [:if, [:vcall, :c],
                                 [:rescue,
                                  [:vcall, :b],
                                  [:resbody, nil, [:nil]]],
                                 nil]],
                               [:lvar, :a]],
            "ParseTree"    => s(:block,
                                s(:lasgn,
                                  :a,
                                  s(:if, s(:call, nil, :c, s(:arglist)),
                                    s(:rescue, s(:call, nil, :b, s(:arglist)),
                                      s(:resbody,
                                        s(:array), s(:nil))),
                                    nil)),
                                s(:lvar, :a)),
            "Ruby2Ruby"    => "a = b rescue nil if c\na\n") # OMG that's awesome

  add_tests("structure_unused_literal_wwtt",
            "Ruby"         => "\"prevent the above from infecting rdoc\"\n\nmodule Graffle\nend",
            "RawParseTree" => [:module, :Graffle, [:scope]],
            "ParseTree"    => s(:module, :Graffle, s(:scope)),
            "Ruby2Ruby"    => "module Graffle\nend")

  add_tests("super_0",
            "Ruby"         => "def x\n  super()\nend",
            "RawParseTree" => [:defn, :x,
                               [:scope,
                                [:block, [:args], [:super]]]],
            "ParseTree"    => s(:defn, :x,
                                s(:args),
                                s(:scope, s(:block, s(:super)))))

  add_tests("super_1",
            "Ruby"         => "def x\n  super(4)\nend",
            "RawParseTree" => [:defn, :x,
                               [:scope,
                                [:block,
                                 [:args],
                                 [:super, [:array, [:lit, 4]]]]]],
            "ParseTree"    => s(:defn, :x,
                                s(:args),
                                s(:scope,
                                  s(:block,
                                    s(:super, s(:lit, 4))))))

  add_tests("super_1_array",
            "Ruby"         => "def x\n  super([24, 42])\nend",
            "RawParseTree" => [:defn, :x,
                               [:scope,
                                [:block,
                                 [:args],
                                 [:super,
                                  [:array,
                                   [:array, [:lit, 24], [:lit, 42]]]]]]],
            "ParseTree"    => s(:defn, :x,
                                s(:args),
                                s(:scope,
                                  s(:block,
                                    s(:super, s(:array,
                                                s(:lit, 24),
                                                s(:lit, 42)))))))

  add_tests("super_block_pass",
            "Ruby"         => "super(a, &b)",
            "RawParseTree" => [:block_pass,
                               [:vcall, :b], [:super, [:array, [:vcall, :a]]]],
            "ParseTree"    => s(:super,
                                s(:call, nil, :a, s(:arglist)),
                                s(:block_pass,
                                  s(:call, nil, :b, s(:arglist)))))

  add_tests("super_block_splat",
            "Ruby"         => "super(a, *b)",
            "RawParseTree" => [:super,
                               [:argscat,
                                [:array, [:vcall, :a]],
                                [:vcall, :b]]],
            "ParseTree"    => s(:super,
                                s(:call, nil, :a, s(:arglist)),
                                s(:splat, s(:call, nil, :b, s(:arglist)))))

  add_tests("super_n",
            "Ruby"         => "def x\n  super(24, 42)\nend",
            "RawParseTree" => [:defn, :x,
                               [:scope,
                                [:block,
                                 [:args],
                                 [:super, [:array, [:lit, 24], [:lit, 42]]]]]],
            "ParseTree"    => s(:defn, :x,
                                s(:args),
                                s(:scope,
                                  s(:block,
                                    s(:super, s(:lit, 24), s(:lit, 42))))))

  add_tests("svalue",
            "Ruby"         => "a = *b",
            "RawParseTree" => [:lasgn, :a, [:svalue, [:splat, [:vcall, :b]]]],
            "ParseTree"    => s(:lasgn, :a,
                                s(:svalue,
                                  s(:splat, s(:call, nil, :b, s(:arglist))))))

  add_tests("to_ary",
            "Ruby"         => "a, b = c",
            "RawParseTree" => [:masgn,
                               [:array, [:lasgn, :a], [:lasgn, :b]], nil,
                               [:to_ary, [:vcall, :c]]],
            "ParseTree"    => s(:masgn,
                                s(:array, s(:lasgn, :a), s(:lasgn, :b)),
                                s(:to_ary, s(:call, nil, :c, s(:arglist)))))

  add_tests("true",
            "Ruby"         => "true",
            "RawParseTree" => [:true],
            "ParseTree"    => s(:true))

  add_tests("undef",
            "Ruby"         => "undef :x",
            "RawParseTree" => [:undef, [:lit, :x]],
            "ParseTree"    => s(:undef, s(:lit, :x)))

  add_tests("undef_2",
            "Ruby"         => "undef :x, :y",
            "RawParseTree" => [:block,
                               [:undef, [:lit, :x]],
                               [:undef, [:lit, :y]]],
            "ParseTree"    => s(:block,
                                s(:undef, s(:lit, :x)),
                                s(:undef, s(:lit, :y))),
            "Ruby2Ruby"    => "undef :x\nundef :y\n")

  add_tests("undef_3",
            "Ruby"         => "undef :x, :y, :z",
            "RawParseTree" => [:block,
                               [:undef, [:lit, :x]],
                               [:undef, [:lit, :y]],
                               [:undef, [:lit, :z]]],
            "ParseTree"    => s(:block,
                                s(:undef, s(:lit, :x)),
                                s(:undef, s(:lit, :y)),
                                s(:undef, s(:lit, :z))),
            "Ruby2Ruby"    => "undef :x\nundef :y\nundef :z\n")

  add_tests("undef_block_1",
            "Ruby"         => "f1\nundef :x\n", # TODO: don't like the extra return
            "RawParseTree" => [:block,
                               [:vcall, :f1],
                               [:undef, [:lit, :x]]],
            "ParseTree"    => s(:block,
                                s(:call, nil, :f1, s(:arglist)),
                                s(:undef, s(:lit, :x))))

  add_tests("undef_block_2",
            "Ruby"         => "f1\nundef :x, :y",
            "RawParseTree" => [:block,
                               [:vcall, :f1],
                               [:block,
                                [:undef, [:lit, :x]],
                                [:undef, [:lit, :y]],
                               ]],
            "ParseTree"    => s(:block,
                                s(:call, nil, :f1, s(:arglist)),
                                s(:block,
                                  s(:undef, s(:lit, :x)),
                                  s(:undef, s(:lit, :y)))),
            "Ruby2Ruby"    => "f1\n(undef :x\nundef :y)\n")

  add_tests("undef_block_3",
            "Ruby"         => "f1\nundef :x, :y, :z",
            "RawParseTree" => [:block,
                               [:vcall, :f1],
                               [:block,
                                [:undef, [:lit, :x]],
                                [:undef, [:lit, :y]],
                                [:undef, [:lit, :z]],
                               ]],
            "ParseTree"    => s(:block,
                                s(:call, nil, :f1, s(:arglist)),
                                s(:block,
                                  s(:undef, s(:lit, :x)),
                                  s(:undef, s(:lit, :y)),
                                  s(:undef, s(:lit, :z)))),
            "Ruby2Ruby"    => "f1\n(undef :x\nundef :y\nundef :z)\n")

  add_tests("undef_block_3_post",
            "Ruby"         => "undef :x, :y, :z\nf2",
            "RawParseTree" => [:block,
                               [:undef, [:lit, :x]],
                               [:undef, [:lit, :y]],
                               [:undef, [:lit, :z]],
                               [:vcall, :f2]],
            "ParseTree"    => s(:block,
                                s(:undef, s(:lit, :x)),
                                s(:undef, s(:lit, :y)),
                                s(:undef, s(:lit, :z)),
                                s(:call, nil, :f2, s(:arglist))),
            "Ruby2Ruby"    => "undef :x\nundef :y\nundef :z\nf2\n")

  add_tests("undef_block_wtf",
            "Ruby"         => "f1\nundef :x, :y, :z\nf2",
            "RawParseTree" => [:block,
                               [:vcall, :f1],
                               [:block,
                                [:undef, [:lit, :x]],
                                [:undef, [:lit, :y]],
                                [:undef, [:lit, :z]]],
                               [:vcall, :f2]],
            "ParseTree"    => s(:block,
                                s(:call, nil, :f1, s(:arglist)),
                                s(:block,
                                  s(:undef, s(:lit, :x)),
                                  s(:undef, s(:lit, :y)),
                                  s(:undef, s(:lit, :z))),
                                s(:call, nil, :f2, s(:arglist))),
            "Ruby2Ruby"    => "f1\n(undef :x\nundef :y\nundef :z)\nf2\n")


  add_tests("unless_post",
            "Ruby"         => "a unless b",
            "RawParseTree" => [:if, [:vcall, :b], nil, [:vcall, :a]],
            "ParseTree"    => s(:if, s(:call, nil, :b, s(:arglist)), nil,
                                s(:call, nil, :a, s(:arglist))))

  add_tests("unless_post_not",
            "Ruby"         => "a unless not b",
            "RawParseTree" => [:if, [:vcall, :b], [:vcall, :a], nil],
            "ParseTree"    => s(:if, s(:call, nil, :b, s(:arglist)),
                                s(:call, nil, :a, s(:arglist)), nil),
            "Ruby2Ruby"    => "a if b")

  add_tests("unless_pre",
            "Ruby"         => "unless b then a end",
            "RawParseTree" => [:if, [:vcall, :b], nil, [:vcall, :a]],
            "ParseTree"    => s(:if, s(:call, nil, :b, s(:arglist)), nil,
                                s(:call, nil, :a, s(:arglist))),
            "Ruby2Ruby"    => "a unless b")

  add_tests("unless_pre_not",
            "Ruby"         => "unless not b then a end",
            "RawParseTree" => [:if, [:vcall, :b], [:vcall, :a], nil],
            "ParseTree"    => s(:if, s(:call, nil, :b, s(:arglist)),
                                s(:call, nil, :a, s(:arglist)), nil),
            "Ruby2Ruby"    => "a if b")

  add_tests("until_post",
            "Ruby"         => "begin\n  (1 + 1)\nend until false",
            "RawParseTree" => [:until, [:false],
                               [:call, [:lit, 1], :+,
                                [:array, [:lit, 1]]], false],
            "ParseTree"    => s(:until, s(:false),
                                s(:call, s(:lit, 1), :+,
                                  s(:arglist, s(:lit, 1))), false))

  add_tests("until_post_not",
            "Ruby"         => "begin\n  (1 + 1)\nend until not true",
            "RawParseTree" => [:while, [:true],
                               [:call, [:lit, 1], :+,
                                [:array, [:lit, 1]]], false],
            "ParseTree"    => s(:while, s(:true),
                                s(:call, s(:lit, 1), :+,
                                  s(:arglist, s(:lit, 1))), false),
            "Ruby2Ruby"    => "begin\n  (1 + 1)\nend while true")

  add_tests("until_pre",
            "Ruby"         => "until false do\n  (1 + 1)\nend",
            "RawParseTree" => [:until, [:false],
                               [:call, [:lit, 1], :+,
                                [:array, [:lit, 1]]], true],
            "ParseTree"    => s(:until, s(:false),
                                s(:call, s(:lit, 1), :+,
                                  s(:arglist, s(:lit, 1))), true))

  add_tests("until_pre_mod",
            "Ruby"         => "(1 + 1) until false",
            "RawParseTree" => [:until, [:false],
                               [:call, [:lit, 1], :+,
                                [:array, [:lit, 1]]], true],
            "ParseTree"    => s(:until, s(:false),
                                s(:call, s(:lit, 1), :+,
                                  s(:arglist, s(:lit, 1))), true),
            "Ruby2Ruby"    => "until false do\n  (1 + 1)\nend")

  add_tests("until_pre_not",
            "Ruby"         => "until not true do\n  (1 + 1)\nend",
            "RawParseTree" => [:while, [:true],
                               [:call, [:lit, 1], :+,
                                [:array, [:lit, 1]]], true],
            "ParseTree"    => s(:while, s(:true),
                                s(:call, s(:lit, 1), :+,
                                  s(:arglist, s(:lit, 1))), true),
            "Ruby2Ruby"    => "while true do\n  (1 + 1)\nend")

  add_tests("until_pre_not_mod",
            "Ruby"         => "(1 + 1) until not true",
            "RawParseTree" => [:while, [:true],
                               [:call, [:lit, 1], :+,
                                [:array, [:lit, 1]]], true],
            "ParseTree"    => s(:while, s(:true),
                                s(:call, s(:lit, 1), :+,
                                  s(:arglist, s(:lit, 1))), true),
            "Ruby2Ruby"    => "while true do\n  (1 + 1)\nend")

  add_tests("valias",
            "Ruby"         => "alias $y $x",
            "RawParseTree" => [:valias, :$y, :$x],
            "ParseTree"    => s(:valias, :$y, :$x))

  add_tests("vcall",
            "Ruby"         => "method",
            "RawParseTree" => [:vcall, :method],
            "ParseTree"    => s(:call, nil, :method, s(:arglist)))

  add_tests("while_post",
            "Ruby"         => "begin\n  (1 + 1)\nend while false",
            "RawParseTree" => [:while, [:false],
                               [:call, [:lit, 1], :+,
                                [:array, [:lit, 1]]], false],
            "ParseTree"    => s(:while, s(:false),
                                s(:call, s(:lit, 1), :+,
                                  s(:arglist, s(:lit, 1))), false))

  add_tests("while_post2",
            "Ruby"         => "begin\n  (1 + 2)\n  (3 + 4)\nend while false",
            "RawParseTree" => [:while, [:false],
                               [:block,
                                [:call, [:lit, 1], :+, [:array, [:lit, 2]]],
                                [:call, [:lit, 3], :+, [:array, [:lit, 4]]]],
                               false],
            "ParseTree"    => s(:while, s(:false),
                                s(:block,
                                  s(:call, s(:lit, 1), :+,
                                    s(:arglist, s(:lit, 2))),
                                  s(:call, s(:lit, 3), :+,
                                    s(:arglist, s(:lit, 4)))),
                                false))

  add_tests("while_post_not",
            "Ruby"         => "begin\n  (1 + 1)\nend while not true",
            "RawParseTree" => [:until, [:true],
                               [:call, [:lit, 1], :+,
                                [:array, [:lit, 1]]], false],
            "ParseTree"    => s(:until, s(:true),
                                s(:call, s(:lit, 1), :+,
                                  s(:arglist, s(:lit, 1))), false),
            "Ruby2Ruby"    => "begin\n  (1 + 1)\nend until true")

  add_tests("while_pre",
            "Ruby"         => "while false do\n  (1 + 1)\nend",
            "RawParseTree" => [:while, [:false],
                               [:call, [:lit, 1], :+,
                                [:array, [:lit, 1]]], true],
            "ParseTree"    => s(:while, s(:false),
                                s(:call, s(:lit, 1), :+,
                                  s(:arglist, s(:lit, 1))), true))

  add_tests("while_pre_mod",
            "Ruby"         => "(1 + 1) while false",
            "RawParseTree" => [:while, [:false],
                               [:call, [:lit, 1], :+,
                                [:array, [:lit, 1]]], true],
            "ParseTree"    => s(:while, s(:false),
                                s(:call, s(:lit, 1), :+,
                                  s(:arglist, s(:lit, 1))), true),
            "Ruby2Ruby"    => "while false do\n  (1 + 1)\nend") # FIX can be one liner

  add_tests("while_pre_nil",
            "Ruby"         => "while false do\nend",
            "RawParseTree" => [:while, [:false], nil, true],
            "ParseTree"    => s(:while, s(:false), nil, true))

  add_tests("while_pre_not",
            "Ruby"         => "while not true do\n  (1 + 1)\nend",
            "RawParseTree" => [:until, [:true],
                               [:call, [:lit, 1], :+,
                                [:array, [:lit, 1]]], true],
            "ParseTree"    => s(:until, s(:true),
                                s(:call, s(:lit, 1), :+,
                                  s(:arglist, s(:lit, 1))), true),
            "Ruby2Ruby"    => "until true do\n  (1 + 1)\nend")

  add_tests("while_pre_not_mod",
            "Ruby"         => "(1 + 1) while not true",
            "RawParseTree" => [:until, [:true],
                               [:call, [:lit, 1], :+,
                                [:array, [:lit, 1]]], true],
            "ParseTree"    => s(:until, s(:true),
                                s(:call, s(:lit, 1), :+,
                                  s(:arglist, s(:lit, 1))), true),
            "Ruby2Ruby"    => "until true do\n  (1 + 1)\nend") # FIX

  add_tests("xstr",
            "Ruby"         => "`touch 5`",
            "RawParseTree" => [:xstr, 'touch 5'],
            "ParseTree"    => s(:xstr, 'touch 5'))

  add_tests("yield_0",
            "Ruby"         => "yield",
            "RawParseTree" => [:yield],
            "ParseTree"    => s(:yield))

  add_tests("yield_1",
            "Ruby"         => "yield(42)",
            "RawParseTree" => [:yield, [:lit, 42]],
            "ParseTree"    => s(:yield, s(:lit, 42)))

  add_tests("yield_array_0",
            "Ruby"         => "yield([])",
            "RawParseTree" => [:yield, [:zarray], true],
            "ParseTree"    => s(:yield, s(:array)))

  add_tests("yield_array_1",
            "Ruby"         => "yield([42])",
            "RawParseTree" => [:yield, [:array, [:lit, 42]], true],
            "ParseTree"    => s(:yield, s(:array, s(:lit, 42))))

  add_tests("yield_array_n",
            "Ruby"         => "yield([42, 24])",
            "RawParseTree" => [:yield, [:array, [:lit, 42], [:lit, 24]], true],
            "ParseTree"    => s(:yield, s(:array, s(:lit, 42), s(:lit, 24))))

  add_tests("yield_n",
            "Ruby"         => "yield(42, 24)",
            "RawParseTree" => [:yield, [:array, [:lit, 42], [:lit, 24]]],
            "ParseTree"    => s(:yield, s(:lit, 42), s(:lit, 24)))

  add_tests("zarray",
            "Ruby"         => "a = []",
            "RawParseTree" => [:lasgn, :a, [:zarray]],
            "ParseTree"    => s(:lasgn, :a, s(:array)))

  add_tests("zsuper",
            "Ruby"         => "def x\n  super\nend",
            "RawParseTree" => [:defn, :x,
                               [:scope, [:block, [:args], [:zsuper]]]],
            "ParseTree"    => s(:defn, :x, s(:args),
                                s(:scope, s(:block, s(:zsuper)))))
end
