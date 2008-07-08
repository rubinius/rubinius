#!/usr/bin/env ruby -w

require 'test/unit'
require 'stringio'

require 'akira_bot'

class TestAkiraBot < Test::Unit::TestCase
  def setup
    @bot = AkiraBot.new :channel => "#channel"
    @bot.socket = @io = StringIO.new
  end

  def test_cmd_help
    @bot.cmd_help nil

    exp = "PRIVMSG #channel :Commands I know: \0037 help, quit\r\n"
    assert_equal exp, @io.string
  end

  def test_cmd_quit
    @bot.cmd_quit []

    exp = "QUIT :\r\n"
    assert_equal exp, @io.string
    assert_equal true, @bot.quit
  end

  def test_say
    @bot.say "text"

    assert_equal "PRIVMSG #channel :text\r\n", @io.string
  end
end
