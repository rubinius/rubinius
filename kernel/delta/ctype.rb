module Rubinius::CType
  def self.toprint(num)
    case num
    when ?\n: '\n'
    when ?\t: '\t'
    when ?\a: '\a'
    when ?\v: '\v'
    when ?\f: '\f'
    when ?\r: '\r'
    when ?\e: '\e'
    when ?\b: '\b'
    when ?":  '\"'
    when ?\\: '\\\\'
    when ?#:  Rubinius::Tuple['#$', '\#$', '#@', '\#@', '#{', '\#{', '#', '#']
    else
      if num < 32 || num > 126
        str = "\\000"
        str.modify!

        c = num.to_s 8
        str.copy_from c, 0, c.size, 4-c.size
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
