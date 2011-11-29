class Time

  def inspect
    if @is_gmt
      strftime("%a %b %d %H:%M:%S UTC %Y")
    else
      strftime("%a %b %d %H:%M:%S %z %Y")
    end
  end

  alias_method :to_s,       :inspect

  def self.local(first, *args)
    if args.size == 9
      second = second.kind_of?(String)  ? first.to_i   : Rubinius::Type.num2long(first)
      minute = args[0].kind_of?(String) ? args[0].to_i : Rubinius::Type.num2long(args[0])
      hour =   args[1].kind_of?(String) ? args[1].to_i : Rubinius::Type.num2long(args[1])
      day =    args[2].kind_of?(String) ? args[2].to_i : Rubinius::Type.num2long(args[2])
      month =  args[3].kind_of?(String) ? args[3].to_i : Rubinius::Type.num2long(args[3])
      year =   args[4].kind_of?(String) ? args[4].to_i : Rubinius::Type.num2long(args[4])
      usec =   0
      isdst =  args[7] ? 1 : 0
    else
      # resolve month names to numbers
      month = args[0]
      if month
        if month.kind_of?(String) or month.respond_to?(:to_str)
          month = StringValue(month)

          month = MonthValue[month.upcase] || month.to_i

          raise ArgumentError, "month argument out of range" unless month
        else
          month = Rubinius::Type.num2long(month)
        end
      else
        month = 1
      end

      year =   first.kind_of?(String)   ? first.to_i   : Rubinius::Type.num2long(first)
      day =    args[1].kind_of?(String) ? args[1].to_i : Rubinius::Type.num2long(args[1] || 1)
      hour =   args[2].kind_of?(String) ? args[2].to_i : Rubinius::Type.num2long(args[2] || 0)
      minute = args[3].kind_of?(String) ? args[3].to_i : Rubinius::Type.num2long(args[3] || 0)
      second = args[4].kind_of?(String) ? args[4].to_i : Rubinius::Type.num2long(args[4] || 0)
      usec =   args[5].kind_of?(String) ? args[5].to_i : Rubinius::Type.num2long(args[5] || 0)
      isdst =  -1
    end

    # This logic is taken from MRI, on how to deal with 2 digit dates.
    if year < 200
      if 0 <= year and year < 39
        warn "2 digit year used: #{year}" if $VERBOSE
        year += 2000
      elsif 69 <= year and year < 139
        warn "2 or 3 digit year used: #{year}" if $VERBOSE
        year += 1900
      end
    end

    from_array(second, minute, hour, day, month, year, usec, isdst, false)
  end

  class << self
    alias_method :mktime, :local
  end

end
