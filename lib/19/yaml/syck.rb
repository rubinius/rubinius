# $Id$
#
# = yaml/syck.rb:
#

require 'stringio'
require 'syck/ext/syck'
require 'syck/error'
require 'syck/syck'
require 'syck/tag'
require 'syck/stream'
require 'syck/constants'
require 'syck/rubytypes'
require 'syck/types'

module Syck
  #--
  # For Rubinius, replaces the rb_iterate call to syck_set_ivars.
  #++
  def self.set_ivars(hsh, obj)
    hsh.each do |key, value|
      obj.instance_variable_set :"@#{key}", value
    end
  end

  #--
  # For Rubinius, replaces the rb_iterate call to syck_merge_i.
  #++
  def self.merge_i(ary, hsh)
    ary.each do |entry|
      begin
        entry = Rubinius::Type.coerce_to entry, Hash, :to_hash
        hsh.update entry
      rescue
        # ignore coercion errors
      end
    end

    nil
  end

  #--
  # For Rubinius, replaces rb_syck_mktime.
  #++
  def self.mktime(str)
    require "date"
    begin
      DateTime.parse(str).to_time
    rescue ArgumentError
      # nothing
    end
  end
end
