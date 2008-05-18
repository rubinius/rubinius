#!/usr/bin/ruby -w

require 'mini/test'

class Module
  def infect_with_assertions pos_prefix, neg_prefix, map = {}
    Mini::Assertions.public_instance_methods(false).each do |meth|
      new_name = case meth
                 when /^assert/ then
                   meth.to_s.sub(/^assert/, pos_prefix.to_s)
                 when /^refute/ then
                   meth.to_s.sub(/^refute/, neg_prefix.to_s)
                 end
      next unless new_name

      regexp, replacement = map.find { |regexp, _| new_name =~ regexp }
      next if replacement == :skip
      new_name.sub! regexp, replacement if replacement

      # warn "%-20s -> %s" % [meth, new_name]
      self.class_eval <<-EOM
        def #{new_name} *args, &block
          return Mini::Spec.current.#{meth}(*args, &self)     if Proc === self
          return Mini::Spec.current.#{meth}(args.first, self) if args.size == 1
          return Mini::Spec.current.#{meth}(self, *args)
        end
      EOM
    end
  end
end

Object.infect_with_assertions(:must, :wont,
                              /(throw)s/                  => '\1',
                              /_same/                     => '_be_same_as',
                              /must_(.*_of|nil)/          => 'must_be_\1',
                              /must_not_(nil|same)/       => 'must_not_be_\1',
                              /wont_(nil)/                => 'wont_be_\1',
                              /must_in_delta/             => 'must_be_close_to',
                              /must_no_match/             => 'must_not_match',
                              /must_operator/             => 'must_be',
/^(wont|must(_(block|raises|nothing_(raised|thrown)))?)$/ => :skip)

module Kernel
  def describe desc, &block
    cls = Class.new(Mini::Spec)
    Object.const_set desc.to_s.split(/\W+/).map { |s| s.capitalize }.join, cls

    cls.class_eval(&block)
  end
end

class Mini::Spec < Mini::Test::TestCase
  def self.current
    @@current_spec
  end

  def initialize name
    super
    @@current_spec = self
  end

  def self.before(type = :each, &block)
    raise "unsupported before type: #{type}" unless type == :each
    define_method :setup, &block
  end

  def self.after(type = :each, &block)
    raise "unsupported after type: #{type}" unless type == :each
    define_method :teardown, &block
  end

  def self.it desc, &block
    define_method "test_#{desc.gsub(/\W+/, '_').downcase}", &block
  end
end
