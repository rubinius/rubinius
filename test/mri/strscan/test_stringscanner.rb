#
# test/strscan/test_stringscanner.rb
#

require 'strscan'
require 'test/unit'

RUBINIUS = defined? RUBY_ENGINE and RUBY_ENGINE == "rbx"

class TestStringScanner < Test::Unit::TestCase
  UNINIT_ERROR = ArgumentError

  def test_AREF
    s = StringScanner.new('stra strb strc')

    assert_equal 'stra', s.scan(/\w+/)
    assert_nil           s[-2]
    assert_equal 'stra', s[-1]
    assert_equal 'stra', s[0]
    assert_nil           s[1]

    assert_equal false,  s[-1].tainted?
    assert_equal false,  s[0].tainted?

    assert_equal 1, s.skip(/\s+/)
    assert_nil           s[-2]
    assert_equal ' ',    s[-1]
    assert_equal ' ',    s[0]
    assert_nil           s[1]

    assert_equal 'strb', s.scan(/(s)t(r)b/)
    assert_nil           s[-100]
    assert_nil           s[-4]
    assert_equal 'strb', s[-3]
    assert_equal 's',    s[-2]
    assert_equal 'r',    s[-1]
    assert_equal 'strb', s[0]
    assert_equal 's',    s[1]
    assert_equal 'r',    s[2]
    assert_nil           s[3]
    assert_nil           s[100]

    assert_equal ' ', s.scan(/\s+/)

    assert_equal 's', s.getch
    assert_nil           s[-2]
    assert_equal 's',    s[-1]
    assert_equal 's',    s[0]
    assert_nil           s[1]

    assert_equal 't', s.get_byte
    assert_nil           s[-2]
    assert_equal 't',    s[-1]
    assert_equal 't',    s[0]
    assert_nil           s[1]

    assert_equal 'rc', s.scan(/.*/)
    assert_nil           s.scan(/./)
    assert_nil           s[0]
    assert_nil           s[0]


    # HACK: rubinius currently doesn't do KCODE at all
    kc_backup = $KCODE
    begin
      $KCODE = 'EUC'
      s = StringScanner.new("\244\242")
      s.getch
      assert_equal "\244\242", s[0]
    ensure
      $KCODE = kc_backup
    end unless RUBINIUS

    str = 'test'
    str.taint
    s = StringScanner.new(str)
    assert_equal 'test', s.scan(/(t)(e)(s)(t)/)
    assert_equal true, s[0].tainted?
    assert_equal true, s[1].tainted?
    assert_equal true, s[2].tainted?
    assert_equal true, s[3].tainted?
    assert_equal true, s[4].tainted?
  end

  def test_bol?
    s = StringScanner.new("a\nbbb\n\ncccc\nddd\r\neee")
    assert_equal true, s.bol?
    assert_equal true, s.bol?
    assert_equal 'a', s.scan(/a/)
    assert_equal false, s.bol?
    assert_equal false, s.bol?
    assert_equal "\n", s.scan(/\n/)
    assert_equal true, s.bol?
    assert_equal 'b', s.scan(/b/)
    assert_equal false, s.bol?
    assert_equal 'b', s.scan(/b/)
    assert_equal false, s.bol?
    assert_equal 'b', s.scan(/b/)
    assert_equal false, s.bol?
    assert_equal "\n", s.scan(/\n/)
    assert_equal true, s.bol?
    assert_equal s, s.unscan
    assert_equal false, s.bol?
    assert_equal "\n", s.scan(/\n/)
    assert_equal "\n", s.scan(/\n/)
    assert_equal true, s.bol?
    assert_equal "cccc\n", s.scan(/c+\n/)
    assert_equal true, s.bol?
    assert_equal "ddd\r\n", s.scan(/d+\r\n/)
    assert_equal true, s.bol?
    assert_equal "eee", s.scan(/e+/)
    assert_equal false, s.bol?
  end

  def test_concat
    s = StringScanner.new('a')
    assert_equal 'a', s.scan(/a/)
    assert_equal s, s.concat('b')
    assert_equal false, s.eos?
    assert_equal 'b', s.scan(/b/)
    assert_equal true, s.eos?
    assert_equal s, s.concat('c')
    assert_equal false, s.eos?
    assert_equal 'c', s.scan(/c/)
    assert_equal true, s.eos?
  end

  def test_const_Id
    assert_instance_of String, StringScanner::Id
    assert_equal true, StringScanner::Id.frozen?
  end

  def test_const_Version
    assert_instance_of String, StringScanner::Version
    assert_equal true, StringScanner::Version.frozen?
  end

  def test_dup
    s = StringScanner.new('test string')
    d = s.dup
    assert_equal s.inspect, d.inspect
    assert_equal s.string, d.string
    assert_equal s.pos, d.pos
    assert_equal s.matched?, d.matched?
    assert_equal s.eos?, d.eos?

    s = StringScanner.new('test string')
    assert_equal 'test', s.scan(/test/)
    d = s.dup
    assert_equal s.inspect, d.inspect
    assert_equal s.string, d.string
    assert_equal s.pos, d.pos
    assert_equal s.matched?, d.matched?
    assert_equal s.eos?, d.eos?

    s = StringScanner.new('test string')
    assert_equal 'test', s.scan(/test/)
    assert_nil s.scan(/NOT MATCH/)

    d = s.dup
    assert_equal s.inspect, d.inspect
    assert_equal s.string, d.string
    assert_equal s.pos, d.pos
    assert_equal s.matched?, d.matched?
    assert_equal s.eos?, d.eos?

    s = StringScanner.new('test string')
    assert_equal s, s.terminate
    d = s.dup
    assert_equal s.inspect, d.inspect
    assert_equal s.string, d.string
    assert_equal s.pos, d.pos
    assert_equal s.matched?, d.matched?
    assert_equal s.eos?, d.eos?
  end

  def test_eos?
    s = StringScanner.new('test string')
    assert_equal false, s.eos?
    assert_equal false, s.eos?
    assert_equal 'test', s.scan(/\w+/)
    assert_equal false, s.eos?
    assert_equal false, s.eos?
    assert_equal ' ', s.scan(/\s+/)
    assert_equal 'string', s.scan(/\w+/)
    assert_equal true, s.eos?
    assert_equal true, s.eos?
    assert_nil s.scan(/\w+/)
    assert_equal true, s.eos?

    s = StringScanner.new('test')
    assert_equal 'te', s.scan(/te/)

    s.string.replace ''
    assert_equal true, s.eos?
  end

  def test_get_byte
    s = StringScanner.new('abcde')
    assert_equal 'a', s.get_byte
    assert_equal 'b', s.get_byte
    assert_equal 'c', s.get_byte
    assert_equal 'd', s.get_byte
    assert_equal 'e', s.get_byte
    assert_nil        s.get_byte
    assert_nil        s.get_byte

    str = 'abc'
    str.taint
    s = StringScanner.new(str)
    assert_equal true, s.get_byte.tainted?
    assert_equal true, s.get_byte.tainted?
    assert_equal true, s.get_byte.tainted?
    assert_nil s.get_byte

    # HACK: rubinius currently doesn't do KCODE at all
    kc_backup = $KCODE
    begin
      $KCODE = 'EUC'
      s = StringScanner.new("\244\242")
      assert_equal "\244", s.get_byte
      assert_equal "\242", s.get_byte
      assert_nil s.get_byte
    ensure
      $KCODE = kc_backup
    end unless RUBINIUS

    s = StringScanner.new('test')
    assert_equal 'te', s.scan(/te/)

    s.string.replace ''
    assert_nil s.get_byte
  end

  def test_getch
    s = StringScanner.new('abcde')
    assert_equal 'a', s.getch
    assert_equal 'b', s.getch
    assert_equal 'c', s.getch
    assert_equal 'd', s.getch
    assert_equal 'e', s.getch
    assert_nil        s.getch

    str = 'abc'
    str.taint
    s = StringScanner.new(str)
    assert_equal true, s.getch.tainted?
    assert_equal true, s.getch.tainted?
    assert_equal true, s.getch.tainted?
    assert_nil s.getch

    # HACK: rubinius currently doesn't do KCODE at all
    kc_backup = $KCODE
    begin
      $KCODE = 'EUC'
      s = StringScanner.new("\244\242")
      assert_equal "\244\242", s.getch
      assert_nil s.getch
    ensure
      $KCODE = kc_backup
    end unless RUBINIUS

    s = StringScanner.new('test')
    assert_equal 'te', s.scan(/te/)

    s.string.replace ''
    assert_nil s.getch
  end

  def test_inspect
    str = 'test string'
    str.taint
    s = StringScanner.new(str, false)
    assert_instance_of String, s.inspect
    assert_equal s.inspect, s.inspect
    assert_equal '#<StringScanner 0/11 @ "test ...">', s.inspect.sub(/StringScanner_C/, 'StringScanner')
    assert_equal 't', s.get_byte
    assert_equal '#<StringScanner 1/11 "t" @ "est s...">', s.inspect.sub(/StringScanner_C/, 'StringScanner')
    assert_equal true, s.inspect.tainted?

    s = StringScanner.new("\n")
    assert_equal '#<StringScanner 0/1 @ "\n">', s.inspect
  end

  def test_matched
    s = StringScanner.new('stra strb strc')
    assert_equal 'stra', s.scan(/\w+/)
    assert_equal 'stra', s.matched
    assert_equal false, s.matched.tainted?
    assert_equal ' ', s.scan(/\s+/)
    assert_equal ' ', s.matched
    assert_equal 'strb', s.scan(/\w+/)
    assert_equal 'strb', s.matched
    assert_equal ' ', s.scan(/\s+/)
    assert_equal ' ', s.matched
    assert_equal 'strc', s.scan(/\w+/)
    assert_equal 'strc', s.matched
    assert_nil s.scan(/\w+/)
    assert_nil s.matched
    assert_nil s.getch
    assert_nil s.matched

    s = StringScanner.new('stra strb strc')
    assert_equal 's', s.getch
    assert_equal 's', s.matched
    assert_equal false, s.matched.tainted?
    assert_equal 't', s.get_byte
    assert_equal 't', s.matched
    assert_equal 't', s.matched
    assert_equal false, s.matched.tainted?

    str = 'test'
    str.taint
    s = StringScanner.new(str)
    assert_equal 'test', s.scan(/\w+/)
    assert_equal true, s.matched.tainted?
    assert_equal true, s.matched.tainted?
  end

  def test_matched_size
    s = StringScanner.new('test string')
    assert_nil s.matched_size
    assert_equal 'test', s.scan(/test/)
    assert_equal 4, s.matched_size
    assert_equal 4, s.matched_size
    assert_equal '', s.scan(//)
    assert_equal 0, s.matched_size
    assert_nil s.scan(/x/)
    assert_nil s.matched_size
    assert_nil s.matched_size

    assert_equal s, s.terminate
    assert_nil s.matched_size

    # obsolete
    s = StringScanner.new('test string')
    assert_nil s.matchedsize
    assert_equal 'test', s.scan(/test/)
    assert_equal 4, s.matched_size

    assert_equal s, s.terminate
    assert_nil s.matched_size
  end

  def test_pos
    s = StringScanner.new('test string')
    assert_equal 0, s.pos
    assert_equal 't', s.get_byte
    assert_equal 1, s.pos
    assert_equal 'e', s.get_byte
    assert_equal 2, s.pos

    assert_equal s, s.terminate
    assert_equal 11, s.pos
  end

  def test_post_match
    s = StringScanner.new('a b c d e')
    assert_equal 'a', s.scan(/\w/)
    assert_equal ' b c d e', s.post_match
    assert_equal 1, s.skip(/\s/)
    assert_equal 'b c d e', s.post_match
    assert_equal 'b', s.scan(/\w/)
    assert_equal ' c d e', s.post_match
    assert_equal ' c', s.scan_until(/c/)
    assert_equal ' d e', s.post_match
    assert_equal ' ', s.getch
    assert_equal 'd e', s.post_match
    assert_equal 'd', s.get_byte
    assert_equal ' e', s.post_match
    assert_equal ' ', s.get_byte
    assert_equal 'e', s.post_match
    assert_nil s.scan(/never match/)
    assert_nil s.post_match
    assert_equal 'e', s.scan(/./)
    assert_equal '', s.post_match
    assert_nil s.scan(/./)
    assert_nil s.post_match

    str = 'test string'
    str.taint
    s = StringScanner.new(str)
    assert_equal 'test', s.scan(/\w+/)
    assert_equal true, s.post_match.tainted?
    assert_equal ' ', s.scan(/\s+/)
    assert_equal true, s.post_match.tainted?
    assert_equal 'string', s.scan(/\w+/)
    assert_equal true, s.post_match.tainted?
  end

  def test_pre_match
    s = StringScanner.new('a b c d e')
    assert_equal 'a', s.scan(/\w/)
    assert_equal '', s.pre_match                        # xxx
    assert_equal false, s.pre_match.tainted?
    assert_equal 1, s.skip(/\s/)
    assert_equal 'a', s.pre_match                       # xxx
    assert_equal false, s.pre_match.tainted?
    assert_equal 'b', s.scan(/\w/)
    assert_equal 'a ', s.pre_match                      # xxx
    assert_equal ' c', s.scan_until(/c/)
    assert_equal 'a b ', s.pre_match                    # xxx
    assert_equal ' ', s.getch
    assert_equal 'a b c', s.pre_match                   # xxx
    assert_equal 'd', s.get_byte
    assert_equal 'a b c ', s.pre_match                  # xxx
    assert_equal ' ', s.get_byte
    assert_equal 'a b c d', s.pre_match                 # xxx
    assert_nil s.scan(/never match/)
    assert_nil s.pre_match

    str = 'test string'
    str.taint
    s = StringScanner.new(str)
    assert_equal 'test', s.scan(/\w+/)
    assert_equal true, s.pre_match.tainted?
    assert_equal ' ', s.scan(/\s+/)
    assert_equal true, s.pre_match.tainted?
    assert_equal 'string', s.scan(/\w+/)
    assert_equal true, s.pre_match.tainted?
  end

  def test_reset
    s = StringScanner.new('ssss')
    assert_equal 's', s.getch
    assert_equal s, s.reset
    assert_equal 0, s.pos
    assert_equal 'ssss', s.scan(/\w+/)
    assert_equal s, s.reset
    assert_equal 0, s.pos
    assert_equal s, s.reset
    assert_equal 0, s.pos
  end

  def test_s_allocate
    s = StringScanner.allocate
    assert_equal '#<StringScanner (uninitialized)>', s.inspect.sub(/StringScanner_C/, 'StringScanner')
    assert_raises(UNINIT_ERROR) { s.eos? }
    assert_raises(UNINIT_ERROR) { s.scan(/a/) }

    s.string = 'test'
    assert_equal '#<StringScanner 0/4 @ "test">', s.inspect.sub(/StringScanner_C/, 'StringScanner')
    assert_nothing_raised(UNINIT_ERROR) { s.eos? }
    assert_equal false, s.eos?
  end

  def test_s_mustc
    assert_nothing_raised(NotImplementedError) {
        StringScanner.must_C_version
    }
  end

  def test_s_new
    s = StringScanner.new('test string')
    assert_instance_of StringScanner, s
    assert_equal false, s.eos?
    assert_equal false, s.tainted?

    str = 'test string'
    str.taint
    s = StringScanner.new(str, false)
    assert_instance_of StringScanner, s
    assert_equal false, s.eos?
    assert_same str, s.string
    assert_equal true, s.string.tainted?

    str = 'test string'
    str.taint
    s = StringScanner.new(str)
    assert_equal true, s.string.tainted?
  end

  def test_scan
    s = StringScanner.new('stra strb strc', true)
    tmp = s.scan(/\w+/)
    assert_equal 'stra', tmp
    assert_equal false, tmp.tainted?

    tmp = s.scan(/\s+/)
    assert_equal ' ', tmp
    assert_equal false, tmp.tainted?

    assert_equal 'strb', s.scan(/\w+/)
    assert_equal ' ',    s.scan(/\s+/)

    tmp = s.scan(/\w+/)
    assert_equal 'strc', tmp
    assert_equal false, tmp.tainted?

    assert_nil           s.scan(/\w+/)
    assert_nil           s.scan(/\w+/)


    str = 'stra strb strc'
    str.taint
    s = StringScanner.new(str, false)
    tmp = s.scan(/\w+/)
    assert_equal 'stra', tmp
    assert_equal true, tmp.tainted?

    tmp = s.scan(/\s+/)
    assert_equal ' ', tmp
    assert_equal true, tmp.tainted?

    assert_equal 'strb', s.scan(/\w+/)
    assert_equal ' ',    s.scan(/\s+/)

    tmp = s.scan(/\w+/)
    assert_equal 'strc', tmp
    assert_equal true, tmp.tainted?

    assert_nil           s.scan(/\w+/)
    assert_nil           s.scan(/\w+/)

    s = StringScanner.new('test')
    assert_equal 'te', s.scan(/te/)

    # This assumes #string does not duplicate string,
    # but it is implementation specific issue.
    # DO NOT RELY ON THIS FEATURE.
    s.string.replace ''
    # unspecified: assert_equal 2, s.pos
    assert_nil s.scan(/test/)

    # [ruby-bugs:4361]
    s = StringScanner.new("")
    assert_equal "", s.scan(//)
    assert_equal "", s.scan(//)

    # [ruby-dev:29914]
    # HACK: rubinius currently doesn't do KCODE at all
    %w( NONE EUC SJIS UTF8 ).each do |kcode|
      begin
        $KCODE = kcode
        assert_equal "a", StringScanner.new("a:b").scan(/[^\x01\:]+/n)
        assert_equal "a", StringScanner.new("a:b").scan(/[^\x01\:]+/e)
        assert_equal "a", StringScanner.new("a:b").scan(/[^\x01\:]+/s)
        assert_equal "a", StringScanner.new("a:b").scan(/[^\x01\:]+/u)
      ensure
        $KCODE = 'NONE'
      end
    end unless RUBINIUS
  end

  def test_skip
    s = StringScanner.new('stra strb strc', true)
    assert_equal 4, s.skip(/\w+/)
    assert_equal 1, s.skip(/\s+/)
    assert_equal 4, s.skip(/\w+/)
    assert_equal 1, s.skip(/\s+/)
    assert_equal 4, s.skip(/\w+/)
    assert_nil      s.skip(/\w+/)
    assert_nil      s.skip(/\s+/)
    assert_equal true, s.eos?

    s = StringScanner.new('test')
    assert_equal 'te', s.scan(/te/)

    s.string.replace ''
    assert_nil s.skip(/./)

    # [ruby-bugs:4361]
    s = StringScanner.new("")
    assert_equal 0, s.skip(//)
    assert_equal 0, s.skip(//)
  end

  def test_string
    s = StringScanner.new('test')
    assert_equal 'test', s.string

    s.string = 'a'
    assert_equal 'a', s.string
    assert_equal 'a', s.scan(/a/)

    s.string = 'b'
    assert_equal 0, s.pos
  end

  def test_terminate
    s = StringScanner.new('ssss')
    assert_equal 's', s.getch

    assert_equal s, s.terminate
    assert_equal true, s.eos?

    assert_equal s, s.terminate
    assert_equal true, s.eos?
  end
end
