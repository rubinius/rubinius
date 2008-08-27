$: << 'lib' # so we can run in MRI

require File.dirname(__FILE__) + "/spec_helper"

$-w = true

##
# Two dimensions:
# + one vs many
# + splat on end vs not
#
# Other edge cases:
# + no block args
# + empty block args
# + paren args (for like, each_with_index on a hash)
# + nested paren args
# + unnamed splat
# + trailing comma "arg"

describe Compiler do
  it "compiles 'ary.each do; end'" do
    ruby = <<-EOC
        ary.each do; end
      EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :ary), :each),
             nil)

    sexp.should == parse(ruby) if $unified && $new

    gen_iter sexp do |d|
      # do nothing
    end
  end

  it "compiles 'ary.each do ||; end'" do
    ruby = <<-EOC
        ary.each do ||; end
      EOC

    # FIX: we don't distinguish between no and empty args
    sexp = s(:iter,
             s(:call, s(:vcall, :ary), :each),
             nil)

    sexp.should == parse(ruby) if $unified && $new

    gen_iter sexp do |d|
      # do nothing
    end
  end

  it "compiles 'ary.each do |a|; end'" do
    ruby = <<-EOC
        ary.each do |a|; end
      EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :ary), :each),
             s(:lasgn, :a))

    sexp.should == parse(ruby) if $unified && $new

    gen_iter sexp do |d|
      d.cast_for_single_block_arg
      d.set_local_depth 0, 0
    end
  end

  it "compiles 'ary.each do |a, |; end'" do
    ruby = <<-EOC
        ary.each do |a, |; end
      EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :ary), :each),
             s(:masgn, s(:array, s(:lasgn, :a)), nil, nil))

    sexp.should == parse(ruby) if $unified && $new

    gen_iter sexp do |d|
      d.cast_for_multi_block_arg
      d.lvar_at 0
    end
  end

  it "compiles 'ary.each do |*a|; end'" do
    ruby = <<-EOC
        ary.each do |*a|; end
      EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :ary), :each),
             s(:masgn, s(:lasgn, :a), nil))

    sexp.should == parse(ruby) if $unified && $new

    gen_iter sexp do |d|
      d.cast_array
      d.set_local_depth 0, 0
    end
  end

  it "compiles 'ary.each do |a, b, c|; end'" do
    ruby = <<-EOC
        ary.each do |a, b, c|; end
      EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :ary), :each),
             s(:masgn,
               s(:array,
                 s(:lasgn, :a),
                 s(:lasgn, :b),
                 s(:lasgn, :c)), nil, nil))

    sexp.should == parse(ruby) if $unified && $new

    gen_iter sexp do |d|
      d.cast_for_multi_block_arg
      3.times do |slot|
        d.lvar_at slot
      end
    end
  end

  it "compiles 'ary.each do |a, b, *c|; end'" do
    ruby = <<-EOC
        ary.each do |a, b, *c|; end
      EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :ary), :each),
             s(:masgn,
               s(:array,
                 s(:lasgn, :a),
                 s(:lasgn, :b)),
               s(:lasgn, :c), nil))

    sexp.should == parse(ruby) if $unified && $new

    gen_iter sexp do |d|
      d.cast_for_multi_block_arg
      d.lvar_at 0
      d.lvar_at 1
    end
  end

  it "compiles 'ary.each do |(a, b), c|; end'" do
    ruby = <<-EOC
        ary.each do |(a, b), c|; end
      EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :ary), :each),
             s(:masgn,
               s(:array,
                 s(:masgn,
                   s(:array,
                     s(:lasgn, :a),
                     s(:lasgn, :b)), nil, nil),
                 s(:lasgn, :c)), nil, nil))

    sexp.should == parse(ruby) if $unified && $new

    gen_iter sexp do |d|
      d.cast_for_multi_block_arg

      # Pull the first element out and use it like a tuple.
      d.shift_tuple
      d.cast_tuple
      d.lvar_at 0
      d.lvar_at 1
      d.pop
      d.push :true

      d.pop

      d.lvar_at 2
    end
  end

  it "compiles 'ary.each do |(a, b), *c|; end'" do
    ruby = <<-EOC
        ary.each do |(a, b), *c|; end
      EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :ary), :each),
             s(:masgn,
               s(:array,
                 s(:masgn,
                   s(:array,
                     s(:lasgn, :a),
                     s(:lasgn, :b)), nil, nil)),
               s(:lasgn, :c), nil))

    sexp.should == parse(ruby) if $unified && $new

    gen_iter sexp do |d|
      d.cast_for_multi_block_arg
      d.shift_tuple
      d.cast_tuple

      d.lvar_at 0
      d.lvar_at 1

      d.pop
      d.push :true
      d.pop
    end
  end

  it "compiles 'ary.each do |(a, (b, c)), d|; end'" do
    ruby = <<-EOC
        ary.each do |(a, (b, c)), d|; end
      EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :ary), :each),
             s(:masgn,
               s(:array,
                 s(:masgn,
                   s(:array,
                     s(:lasgn, :a),
                     s(:masgn,
                       s(:array,
                         s(:lasgn, :b),
                         s(:lasgn, :c)), nil, nil)), nil, nil),
                 s(:lasgn, :d)), nil, nil))

    sexp.should == parse(ruby) if $unified && $new

    gen_iter sexp do |d|
      d.cast_for_multi_block_arg
      d.shift_tuple
      d.cast_tuple

      d.lvar_at 0

      d.shift_tuple
      d.cast_tuple

      d.lvar_at 1
      d.lvar_at 2

      2.times do
        d.pop
        d.push :true
        d.pop
      end

      d.lvar_at 3
    end
  end

  it "compiles 'ary.each do |*|; end'" do
    ruby = <<-EOC
        ary.each do |*|; end
      EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :ary), :each),
             s(:masgn, true, nil))

    sexp.should == parse(ruby) if $unified && $new

    gen_iter sexp do |d|
      d.cast_for_multi_block_arg
    end
  end

  it "compiles 'ary.each do |a, *|; end'" do
    ruby = <<-EOC
        ary.each do |a, *|; end
      EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :ary), :each),
             s(:masgn, s(:array, s(:lasgn, :a)), true, nil))

    sexp.should == parse(ruby) if $unified && $new

    gen_iter sexp do |d|
      d.cast_for_multi_block_arg
      d.lvar_at 0
    end
  end

  # TODO: move these to spec/ruby/1.8/language once `rake spec:update` works
  it "runs 'ary.each do; end'" do
    i = 0
    %w(a b c d e f g).each { i += 1 }
    i.should == 7
  end

  it "runs 'ary.each do ||; end'" do
    i = 0
    %w(a b c d e f g).each { || i += 1 } # TODO: check || should warn or not
    i.should == 7
  end

  it "runs 'ary.each do |a|; end'" do
    data = [[1, 2, 3], [4, 5], 6]

    data.dup.map { |a| a }.should == data.dup
  end

  it "runs 'ary.each do |a, |; end'" do
    data = [[1, 2, 3], [4, 5, 6], 7]

    data.dup.map { |a, | a }.should == [1, 4, 7]
  end

  it "runs 'ary.each do |*a|; end'" do
    data = [[1, 2, 3], [4, 5], 6]
    expected = [[[1, 2, 3]], [[4, 5]], [6]]

    data.dup.map { |*a| a }.should == expected
  end

  it "runs 'ary.each do |a, b, c|; end'" do
    data = [[1, 2, 3], [4, 5], 6]
    expected = [[1, 2, 3], [4, 5, nil], [6, nil, nil]]

    data.dup.map { |a, b, c| [a, b, c] }.should == expected
  end

  it "runs 'ary.each do |a, b, *c|; end'" do
    data = [[1, 2, 3], [4, 5], 6]
    expected = [[1, 2, [3]], [4, 5, []], [6, nil, []]]

    data.dup.map { |a, b, *c| [a, b, c] }.should == expected
  end

  it "runs 'ary.each do |(a, b), c|; end'" do
    data = [[[1, 2], 3], [[4], 5], 6]
    expected = [[1, 2, 3], [4, nil, 5], [6, nil, nil]]

    data.dup.map { |(a, b), c| [a, b, c] }.should == expected
  end

  it "runs 'ary.each do |(a, b), *c|; end'" do
    data = [[[1, 2], 3], [[4], 5], 6]
    expected = [[1, 2, [3]], [4, nil, [5]], [6, nil, []]]

    data.dup.map { |(a, b), *c| [a, b, c] }.should == expected
  end

  it "runs 'ary.each do |(a, (b, c)), d|; end'" do
    data     = [[[1, [2, 3]], 4], [[5, 6], 7], [[8], 9], [[10]]]
    expected = [[1, 2, 3, 4], [5, 6, nil, 7],
                [8, nil, nil, 9], [10, nil, nil, nil]]

    data.dup.map { |(a, (b, c)), d| [a, b, c, d] }.should == expected
  end

  it "runs 'ary.each do |*|; end'" do
    i = 0
    %w(a b c d e f g).each { |*| i += 1 }
    i.should == 7
  end

  it "runs 'ary.each do |a, *|; end'" do
    data = [[1, 2, 3], [4, 5], 6]
    expected = [1, 4, 6]

    data.dup.map { |a, *| a }.should == expected
  end
end
