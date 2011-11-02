---
layout: doc_es
title: Cómo - traducir documentación
previous: Escribir documentación
previous_url: how-to/write-documentation
next: Apéndice A - Glosario
next_url: appendix-a-glossary
---

Hay dos tareas de traducción:

1. Actualizar una traducción existente
1. La creación de una traducción completamente nueva

Comience por leer [Cómo Escribir Documentación](/doc/es/how-to/write-documentation/)


### Actualizar una Traducción Existente

Para actualizar una traducción existente, abra el archivo de tema en
`web/doc/LANG` y edite el texto existente o agregue texto recién traducido.


### Creación de una Nueva Traducción

Para crear un nuevo idioma de traducción:

1. Copiar `web/doc/en` a `web/doc/LANG` donde _LANG_ es un código
   [ISO-639-1](http://en.wikipedia.org/wiki/List_of_ISO_639-2_codes)
   para la traducción del idioma que está creando.
1. Editar los enlaces en la tabla de contenido para que apunte a la ubicación
   de los archivos para la traducción. (Nota que por alguna razón el
   atributo `page.base_dir` no está disponible cuando estos archivos están
   siendo renderizados por Jekyll.  Esto debe ser investigado.)
1. Traducir el texto en Inglés al idioma nuevo.
