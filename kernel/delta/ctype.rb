# -*- encoding: us-ascii -*-

module Rubinius::CType
  def self.toprint(num)
    # The character literals (?x) are Fixnums in 1.8 and Strings in 1.9
    # so we use literal values instead so this is 1.8/1.9 compatible.
    case num
    when 7;  '\a'
    when 8;  '\b'
    when 9;  '\t'
    when 10; '\n'
    when 11; '\v'
    when 12; '\f'
    when 13; '\r'
    when 27; '\e'
    when 34; '\"'
    when 35;  Rubinius::Tuple['#$', '\#$', '#@', '\#@', '#{', '\#{', '#', '#']
    when 92; '\\\\'
    else
      if num < 32 || num > 126
        unprintable_chr(num)
      else
        num.chr
      end
    end
  end

  Printed = Rubinius::Tuple.new 256
  i = 0
  while i < 256
    Printed[i] = toprint(i)
    i += 1
  end

  def toprint
    Printed[self]
  end

  Lowered = Rubinius::Tuple.new 256

  i = 0
  while i < 256
    Lowered[i] = tolower(i).chr
    i += 1
  end
end
