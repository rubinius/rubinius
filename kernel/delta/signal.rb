# -*- encoding: us-ascii -*-

module Signal
  Rubinius::Config.section 'rbx.platform.signal.' do |key, value|
    name = key[23, key.length]
    number = value.to_i
    Names[name] = number
    Numbers[number] = name
  end

  # special case of signal.c
  if Names["CHLD"]
    Names["CLD"] = Names["CHLD"]
  end
end
