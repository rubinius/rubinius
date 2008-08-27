module Signal
  Rubinius::RUBY_CONFIG.section 'rbx.platform.signal.' do |key, value|
    Names[key[23, key.length]] = value
  end

  # special case of signal.c
  if Names["CHLD"]
    Names["CLD"] = Names["CHLD"]
  end
end
