# -*- encoding: us-ascii -*-

module Signal
  Rubinius::Config.section 'rbx.platform.signal.' do |key, value|
    Names[key[23, key.length]] = value.to_i
  end

  # special case of signal.c
  if Names["CHLD"]
    Names["CLD"] = Names["CHLD"]
  end
end
