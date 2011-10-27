dir, definitions = ARGV

File.open definitions, "wb" do |f|
  f.puts <<-EOD
    // Encodings
    // This is really a place holder
    define(state, "ASCII-8BIT", ONIG_ENCODING_ASCII);
    define(state, "ISO-8859-1", ONIG_ENCODING_ISO_8859_1);
    define(state, "ISO-8859-2", ONIG_ENCODING_ISO_8859_2);
    define(state, "ISO-8859-3", ONIG_ENCODING_ISO_8859_3);
    define(state, "ISO-8859-4", ONIG_ENCODING_ISO_8859_4);
    define(state, "ISO-8859-5", ONIG_ENCODING_ISO_8859_5);
    define(state, "ISO-8859-6", ONIG_ENCODING_ISO_8859_6);
    define(state, "ISO-8859-7", ONIG_ENCODING_ISO_8859_7);
    define(state, "ISO-8859-8", ONIG_ENCODING_ISO_8859_8);
    define(state, "ISO-8859-9", ONIG_ENCODING_ISO_8859_9);
    define(state, "ISO-8859-10", ONIG_ENCODING_ISO_8859_10);
    define(state, "ISO-8859-11", ONIG_ENCODING_ISO_8859_11);
    define(state, "ISO-8859-13", ONIG_ENCODING_ISO_8859_13);
    define(state, "ISO-8859-14", ONIG_ENCODING_ISO_8859_14);
    define(state, "ISO-8859-15", ONIG_ENCODING_ISO_8859_15);
    define(state, "ISO-8859-16", ONIG_ENCODING_ISO_8859_16);
    // This is really a place holder
    define(state, "UTF-7", ONIG_ENCODING_UTF8);
    define(state, "UTF-16BE", ONIG_ENCODING_UTF16_BE);
    define(state, "UTF-16LE", ONIG_ENCODING_UTF16_LE);
    define(state, "UTF-32BE", ONIG_ENCODING_UTF32_BE);
    define(state, "UTF-32LE", ONIG_ENCODING_UTF32_LE);
    define(state, "EUC-JP", ONIG_ENCODING_EUC_JP);
    define(state, "EUC-TW", ONIG_ENCODING_EUC_TW);
    define(state, "EUC-KR", ONIG_ENCODING_EUC_KR);
    define(state, "EUC-CN", ONIG_ENCODING_EUC_CN);
    define(state, "Shift_JIS", ONIG_ENCODING_SJIS);
    define(state, "KOI8-R", ONIG_ENCODING_KOI8_R);
    define(state, "CP1251", ONIG_ENCODING_CP1251);
    define(state, "Big5", ONIG_ENCODING_BIG5);
    define(state, "GB18030", ONIG_ENCODING_GB18030);
  EOD
end
