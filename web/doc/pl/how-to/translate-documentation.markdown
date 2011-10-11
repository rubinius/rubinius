---
layout: doc_pl
title: Jak przetłumaczyć dokumentację
previous: Jak napisać dokumentację
previous_url: how-to/write-documentation
next: Dodatek A - Słowniczek
next_url: appendix-a-glossary
---

There are two translation tasks:

1. updating an existing translation
1. creating a completely new translation

Begin by reading [How-To - Write
Documentation](/doc/pl/how-to/write-documentation/)


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

