#!/usr/bin/ruby -ws

$p ||= false

$: << File.expand_path("~/Work/p4/zss/src/ParseTree/dev/lib")
require 'parse_tree'
require 'sexp_processor'

class HashHash < Hash
  def initialize
    super { |h,k| h[k] = HashHash.new }
  end

  def + n
    n # RAD cheat eliminating all ||= 0 (since I can't use ||= with HashHash)
  end
end

class CallLogger < SexpProcessor
  attr_reader :calls
  attr_accessor :klass, :meth

  def initialize
    super
    self.unsupported.delete :newline
    self.require_empty = false
    self.auto_shift_type = true

    # @calls[klass][meth][call_type][called_method] = count
    @calls = HashHash.new
  end

  def process_class(exp)
    self.klass = exp.shift
    superklass = process exp.shift
    body       = process exp.shift
    s()
  end

  def process_module(exp)
    self.klass = exp.shift
    body       = process exp.shift
    self.klass = nil
    s()
  end

  def process_defn(exp)
    self.meth = exp.shift
    body      = process exp.shift
    self.meth = nil
    s()
  end

  def process_defs(exp)
    recv      = process exp.shift
    self.meth = :"#{recv.first}.#{exp.shift}"
    body      = process exp.shift
    self.meth = nil
    s()
  end

  def process_fcall(exp)
    if klass and meth then
      mesg = exp.shift
      args = process exp.shift
      calls[klass][meth][:fcall][mesg] += 1 unless $p
    end
    s()
  end

  def process_vcall(exp)
    if klass and meth then
      mesg = exp.shift
      calls[klass][meth][:fcall][mesg] += 1 unless $p
    end
    s()
  end

  def process_call(exp)
    recv = exp.shift
    mesg = exp.shift
    args = process exp.shift

    type = case recv
           when [:const, :Ruby] then
             old_mesg, mesg = mesg, args.last.last
             :primitive if old_mesg == :primitive
           when [:self] then
             :self unless $p
           when [:call, [:self], :class] then
             :class unless $p
           end

    calls[klass][meth][type][mesg] += 1 if type

    s()
  end
end

if ARGV.empty? then
  %w(array hash string).each do |klass|
    ARGV << "kernel/core/#{klass}.rb"
    ARGV << "kernel/bootstrap/#{klass}.rb"
  end
end

cl = CallLogger.new
pt = ParseTree.new(false)

ARGV.each do |path|
  sexp = pt.parse_tree_for_string(File.read(path), path)
  cl.process(sexp.first)
end

require 'yaml'
puts cl.calls.to_yaml.gsub(/ !map:HashHash/, '')
