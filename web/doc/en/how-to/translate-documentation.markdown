---
layout: doc_en
title: How-To - Translate Documentation
previous: Write Documentation
previous_url: how-to/write-documentation
next: Appendix A - Glossary
next_url: appendix-a-glossary
---

There are two translation tasks:

1. updating an existing translation
1. creating a completely new translation

Begin by reading [How-To - Write
Documentation](/doc/en/how-to/write-documentation/)


### Updating an Existing Translation

To update an existing translation, open the topic file under `web/doc/LANG`
and edit the existing text or add newly translated text.


### Creating a New Translation

To create a new language translation:

1. Copy `web/doc/en` to `web/doc/LANG` where _LANG_ is the
   [ISO-639-1](http://en.wikipedia.org/wiki/List_of_ISO_639-2_codes) code for
   the language translation you are creating.
1. Edit the links in the Table of Contents to point to the location of the
   files for the translation. (Note that for some reason the `page.base_dir`
   attribute is not available when these files are being rendered by Jekyll.
   This needs to be investigated.)
1. Translate the English text to the target language.
1. Add a corresponding value to `languages` attribute in YAML front-matter of
   `web/_layouts/doc.html`.
1. Add a corresponding value to `TRANS` array in `rakelib/web.rake`(see below).

### Translator's Tool

We've made some tiny tools to aid translators. Technically, there's only one
such tool yet. It's **Translator Consistency Checker**.

#### Short Synopsis

    $ cd rubinius
    $ rake -T
    
    ...

    rake web:translations:check                # Check web documentation translations for existence and mtime consistency.

    $ rake web:translations:check
    (in /usr/home/paul/develop/ruby/rubinius)
    
    TRANSLATION CHECK: Checking 'web/doc/en' recursively against ["de", "es", "fr", "ja", "pl", "pt-br", "ru"] versions
    
    NB:
    
      LOST IN TRANSLATION:
        web/doc/en/how-to/write-benchmarks.markdown: doesn't exist in 'ru' edition
        web/doc/en/bytecode-compiler/customization.markdown: doesn't exist in 'ru' edition
        web/doc/en/bytecode-compiler/encoder.markdown: doesn't exist in 'ru' edition
        web/doc/en/bytecode-compiler/packager.markdown: doesn't exist in 'ru' edition
        web/doc/en/bytecode-compiler/writer.markdown: doesn't exist in 'ru' edition
    
    
      LAG IN TRANSLATION:
        web/doc/en/garbage-collector/large-objects.markdown: is younger than its 'pl' version
        web/doc/en/garbage-collector/mature-generation.markdown: is younger than its 'ru' version
        web/doc/en/garbage-collector/nursery.markdown: is younger than its 'ru' version
        web/doc/en/garbage-collector/young-generation.markdown: is younger than its 'ru' version
        web/doc/en/systems/concurrency.markdown: is younger than its 'pl' version
        web/doc/en/ruby/instance-variables.markdown: is younger than its 'ru' version
        web/doc/en/bytecode-compiler/ast.markdown: is younger than its 'ru' version
        web/doc/en/bytecode-compiler/parser.markdown: is younger than its 'pl' version
        web/doc/en/garbage-collector.markdown: is younger than its 'ru' version

#### Explanation

`web:translations:check` rake task recursively checks "Leader" docs version (which is
now `web/doc/en` subtree) against the list of translated "followers". Both
leader and translations list are defined in `rakelib/web.rake`:

    # This is the 'leader' version: all is translated from here.
    LEADER = "web/doc/en"

    # Here we MANUALLY list all official doc translations
    TRANS  = [
      'de',
      'es',
      'fr',
      'ja',
      'pl',
      'pt-br',
      'ru'
    ]

For each Leader file the script checks whether the corresponding translation
file exist and has `mtime` younger (less) than the Leader's. If translation is
older than the original Leader file or doesn't exist you'll see that very nice
annoying message as shown in the example. If all files are OK you'll see and recognize
that.

#### Conclusion

This mega duper tiny stupid tool was written to help the translator to
understand which files might obviously need alteration. No more. There's no
intellectual analysis like translation validity check, etc. --- at all. If you
are the smartest Rubinius contributor ever you are welcome to expand this tool
thus helping the society and saving the world!
